#include <pch.h>
#include "MiniTcpProto.h"

using namespace std::chrono_literals;

namespace mini
{
    // --------- WSA 보장 ----------
    static void winsock_shutdown()
    {
        WSACleanup();
    }

    static void EnsureWSA()
    {
        static std::once_flag once;
        std::call_once(once, []()
            {
                WSADATA w{};
                if (WSAStartup(MAKEWORD(2, 2), &w) != 0)
                    throw;

                std::atexit([]() { winsock_shutdown(); });
            });
    }

    // --------- 저수준 유틸 ----------
    bool send_all(SOCKET s, const uint8_t* p, size_t n)
    {
        while (n)
        {
            int sent = ::send(s, reinterpret_cast<const char*>(p), (int)n, 0);
            if (sent <= 0) return false;
            p += sent; n -= sent;
        }
        return true;
    }

    bool recv_all(SOCKET s, uint8_t* p, size_t n)
    {
        while (n)
        {
            int got = ::recv(s, reinterpret_cast<char*>(p), (int)n, 0);
            if (got == SOCKET_ERROR)
            {
                int e = WSAGetLastError();
                if (e == WSAETIMEDOUT) continue; // 타임아웃은 재시도
                return false;
            }
            if (got == 0) return false; // 정중 종료
            p += got; n -= got;
        }
        return true;
    }

    bool set_keepalive(SOCKET s)
    {
        BOOL on = TRUE; setsockopt(s, SOL_SOCKET, SO_KEEPALIVE, (char*)&on, sizeof(on));
        DWORD ms = 3000; setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (char*)&ms, sizeof(ms));
        return true;
    }

    // ================= 클라이언트 =================
    SimpleTcpClient::SimpleTcpClient(const std::string& host, uint16_t port)
        : host_(host), port_(port) {
    }
    SimpleTcpClient::~SimpleTcpClient() { stop(); }

    void SimpleTcpClient::start()
    {
        if (running_.exchange(true)) return;
        th_ = std::thread([this] { loop(); });
    }

    void SimpleTcpClient::stop()
    {
        running_ = false;
        close_sock();
        if (th_.joinable()) th_.join();
    }

    bool SimpleTcpClient::sendFrame(const void* data, size_t len)
    {
        // 전송/소켓/connected_ 동시 보호
        std::function<void()> onDiscCopy;

        {
            std::lock_guard<std::mutex> lk(txMtx_);
            if (!connected_ || sock_ == INVALID_SOCKET) return false;

            uint32_t L = static_cast<uint32_t>(len);
            uint8_t hdr[4] = {
                (uint8_t)(L & 0xFF), (uint8_t)((L >> 8) & 0xFF),
                (uint8_t)((L >> 16) & 0xFF), (uint8_t)((L >> 24) & 0xFF)
            };

            if (!send_all(sock_, hdr, 4) ||
                (L && !send_all(sock_, static_cast<const uint8_t*>(data), L)))
            {
                // 연결 끊김 처리(락 안에서 자원 정리)
                connected_ = false;
                if (sock_ != INVALID_SOCKET)
                {
                    ::shutdown(sock_, SD_BOTH);
                    ::closesocket(sock_);
                    sock_ = INVALID_SOCKET;
                }
                // 콜백 스냅샷
                std::lock_guard<std::mutex> cb(cbMtx_);
                onDiscCopy = onDisc_;
            }
            else
            {
                return true;
            }
        }

        // 락 밖에서 콜백 호출
        if (onDiscCopy) onDiscCopy();
        return false;
    }

    bool SimpleTcpClient::connect_once()
    {
        EnsureWSA();
        close_sock();

        addrinfo hints{}, * res = nullptr, * rp = nullptr;
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;

        const std::string portstr = std::to_string(port_);
        if (::getaddrinfo(host_.c_str(), portstr.c_str(), &hints, &res) != 0) {
            std::lock_guard<std::mutex> lk(txMtx_);
            sock_ = INVALID_SOCKET;
            connected_ = false;
            return false;
        }

        for (rp = res; rp != nullptr; rp = rp->ai_next) {
            SOCKET s = ::socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
            if (s == INVALID_SOCKET) continue;

            set_keepalive(s);

            if (::connect(s, rp->ai_addr, (int)rp->ai_addrlen) == 0) {
                set_keepalive(s);
                {
                    std::lock_guard<std::mutex> lk(txMtx_);
                    sock_ = s;
                    connected_ = true;
                }
                ::freeaddrinfo(res);
                return true;
            }
            ::closesocket(s);
        }

        ::freeaddrinfo(res);
        std::lock_guard<std::mutex> lk(txMtx_);
        sock_ = INVALID_SOCKET;
        connected_ = false;
        return false;
    }

    void SimpleTcpClient::loop()
    {
        while (running_)
        {
            if (!connect_once())
            {
                std::this_thread::sleep_for(500ms);
                continue;
            }

            // onConn 스냅샷
            std::function<void()> onConnCopy;
            { std::lock_guard<std::mutex> cb(cbMtx_); onConnCopy = onConn_; }
            if (onConnCopy) onConnCopy();

            // 수신 루프
            while (running_)
            {
                SOCKET s;
                { std::lock_guard<std::mutex> lk(txMtx_); s = sock_; }
                if (s == INVALID_SOCKET) break;

                uint8_t len4[4];
                if (!recv_all(s, len4, 4)) { handle_disconnect(); break; }

                uint32_t L = len4[0] | (len4[1] << 8) | (len4[2] << 16) | (len4[3] << 24);
                if (L > (128u << 20)) { handle_disconnect(); break; }

                std::vector<uint8_t> buf(L);
                if (L && !recv_all(s, buf.data(), L)) { handle_disconnect(); break; }

                // onRaw 스냅샷
                std::function<void(const std::vector<uint8_t>&)> onRawCopy;
                { std::lock_guard<std::mutex> cb(cbMtx_); onRawCopy = onRaw_; }
                if (onRawCopy) onRawCopy(buf);
            }
        }
    }

    void SimpleTcpClient::handle_disconnect()
    {
        std::function<void()> onDiscCopy;

        {
            std::lock_guard<std::mutex> lk(txMtx_);
            if (!connected_) return;
            connected_ = false;
            if (sock_ != INVALID_SOCKET)
            {
                ::shutdown(sock_, SD_BOTH);
                ::closesocket(sock_);
                sock_ = INVALID_SOCKET;
            }
            std::lock_guard<std::mutex> cb(cbMtx_);
            onDiscCopy = onDisc_;
        }

        if (onDiscCopy) onDiscCopy();
    }

    void SimpleTcpClient::close_sock()
    {
        std::lock_guard<std::mutex> lk(txMtx_);
        if (sock_ != INVALID_SOCKET)
        {
            ::shutdown(sock_, SD_BOTH);
            ::closesocket(sock_);
            sock_ = INVALID_SOCKET;
        }
        connected_ = false;
    }

    // ================= 서버 =================
    SimpleTcpServer::SimpleTcpServer(uint16_t port) : port_(port) {}
    SimpleTcpServer::~SimpleTcpServer() { stop(); }

    void SimpleTcpServer::start()
    {
        EnsureWSA();
        if (running_.exchange(true)) return;
        thAccept_ = std::thread([this] { accept_loop(); });
    }

    void SimpleTcpServer::stop()
    {
        running_ = false;

        if (lstn_ != INVALID_SOCKET)
        {
            ::shutdown(lstn_, SD_BOTH);
            ::closesocket(lstn_);
            lstn_ = INVALID_SOCKET;
        }

        {
            std::lock_guard<std::mutex> lk(mtx_);
            for (auto& kv : clients_) {
                ::shutdown(kv.second, SD_BOTH);
                ::closesocket(kv.second);
            }
            clients_.clear();
        }

        if (thAccept_.joinable()) thAccept_.join();
    }

    bool SimpleTcpServer::sendTo(int cid, const void* data, size_t len)
    {
        std::function<void(int)> onDiscCopy;

        {
            std::lock_guard<std::mutex> lk(mtx_);
            auto it = clients_.find(cid); if (it == clients_.end()) return false;
            SOCKET s = it->second;

            uint32_t L = static_cast<uint32_t>(len);
            uint8_t hdr[4] = { (uint8_t)(L), (uint8_t)(L >> 8), (uint8_t)(L >> 16), (uint8_t)(L >> 24) };

            if (!send_all(s, hdr, 4) ||
                (L && !send_all(s, static_cast<const uint8_t*>(data), L)))
            {
                // 연결 정리
                ::shutdown(s, SD_BOTH);
                ::closesocket(s);
                clients_.erase(it);

                // 콜백 스냅샷
                std::lock_guard<std::mutex> cb(cbMtx_);
                onDiscCopy = onDisc_;
            }
            else
            {
                return true;
            }
        }

        if (onDiscCopy) onDiscCopy(cid);
        return false;
    }

    void SimpleTcpServer::broadcast(const void* data, size_t len)
    {
        std::vector<int> dead;
        {
            std::lock_guard<std::mutex> lk(mtx_);
            for (auto& kv : clients_)
            {
                SOCKET s = kv.second;
                uint32_t L = static_cast<uint32_t>(len);
                uint8_t hdr[4] = { (uint8_t)(L), (uint8_t)(L >> 8), (uint8_t)(L >> 16), (uint8_t)(L >> 24) };

                if (!send_all(s, hdr, 4) ||
                    (L && !send_all(s, static_cast<const uint8_t*>(data), L)))
                {
                    dead.push_back(kv.first);
                }
            }

            for (int id : dead)
            {
                auto it = clients_.find(id);
                if (it != clients_.end())
                {
                    ::shutdown(it->second, SD_BOTH);
                    ::closesocket(it->second);
                    clients_.erase(it);
                }
            }
        }

        // 콜백은 락 밖에서
        if (!dead.empty())
        {
            std::function<void(int)> onDiscCopy;
            { std::lock_guard<std::mutex> cb(cbMtx_); onDiscCopy = onDisc_; }
            if (onDiscCopy)
                for (int id : dead) onDiscCopy(id);
        }
    }

    void SimpleTcpServer::accept_loop()
    {
        EnsureWSA();
        lstn_ = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (lstn_ == INVALID_SOCKET) return;

        set_keepalive(lstn_);

        int on = 1; setsockopt(lstn_, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(on));
        sockaddr_in a{}; a.sin_family = AF_INET; a.sin_addr.s_addr = INADDR_ANY; a.sin_port = htons(port_);
        if (::bind(lstn_, (sockaddr*)&a, sizeof(a)) == SOCKET_ERROR) return;
        if (::listen(lstn_, SOMAXCONN) == SOCKET_ERROR) return;

        while (running_)
        {
            SOCKET s = ::accept(lstn_, nullptr, nullptr);
            if (s == INVALID_SOCKET) { std::this_thread::sleep_for(100ms); continue; }

            set_keepalive(s);

            int cid;
            {
                std::lock_guard<std::mutex> lk(mtx_);
                cid = nextId_++;
                clients_[cid] = s;
            }

            // onConn 스냅샷 후 호출(락 밖)
            std::function<void(int)> onConnCopy;
            { std::lock_guard<std::mutex> cb(cbMtx_); onConnCopy = onConn_; }
            if (onConnCopy) onConnCopy(cid);

            std::thread(&SimpleTcpServer::client_loop, this, cid, s).detach();
        }
    }

    void SimpleTcpServer::client_loop(int cid, SOCKET s)
    {
        while (running_)
        {
            uint8_t len4[4];
            if (!recv_all(s, len4, 4)) { drop(cid); break; }

            uint32_t L = len4[0] | (len4[1] << 8) | (len4[2] << 16) | (len4[3] << 24);
            if (L > (128u << 20)) { drop(cid); break; }

            std::vector<uint8_t> buf(L);
            if (L && !recv_all(s, buf.data(), L)) { drop(cid); break; }

            // onRaw 스냅샷 후 호출(락 밖)
            std::function<void(int, const std::vector<uint8_t>&)> onRawCopy;
            { std::lock_guard<std::mutex> cb(cbMtx_); onRawCopy = onRaw_; }
            if (onRawCopy) onRawCopy(cid, buf);
        }
    }

    void SimpleTcpServer::drop(int cid)
    {
        std::function<void(int)> onDiscCopy;

        {
            std::lock_guard<std::mutex> lk(mtx_);
            auto it = clients_.find(cid);
            if (it != clients_.end())
            {
                ::shutdown(it->second, SD_BOTH);
                ::closesocket(it->second);
                clients_.erase(it);

                std::lock_guard<std::mutex> cb(cbMtx_);
                onDiscCopy = onDisc_;
            }
        }

        if (onDiscCopy) onDiscCopy(cid);
    }

    // drop_nolock: 내부 관리용. 콜백은 여기서 호출하지 않음(락 밖으로 이동).
    void SimpleTcpServer::drop_nolock(int cid)
    {
        auto it = clients_.find(cid);
        if (it != clients_.end())
        {
            ::shutdown(it->second, SD_BOTH);
            ::closesocket(it->second);
            clients_.erase(it);
        }
    }

} // namespace mini
