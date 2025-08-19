#pragma once
// MiniTcpProto.h - 최소 의존성 TCP 프레이밍 + 구조체 바이너리 프로토콜
// MSVC 전제. 링크: Ws2_32.lib
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <ws2tcpip.h>
#include <cstdint>
#include <vector>
#include <thread>
#include <atomic>
#include <mutex>
#include <map>
#include <functional>
#include <chrono>
#include <cstring>
#include <type_traits>
#include "protocol.h"
#include <string>
#pragma comment(lib, "Ws2_32.lib")

namespace mini
{
    const int headerSize = 12; //헤더사이즈가 필수임. 변경시 수정필요.
    // 전송시 자동증가 커멘드인덱스번호
    inline uint32_t next_message_id()
    {
        static std::atomic<uint32_t> g{ 1 };
        return g.fetch_add(1, std::memory_order_relaxed);
    }

    // ----------- 공통 메시지 헤더(반드시 첫 멤버) ----------
    static_assert(sizeof(MessageHeader) == headerSize, "MessageHeader size miss matching"); 

    // ----------- 직렬화/복원 유틸(템플릿) -----------
    template <typename T>
    inline std::vector<uint8_t> SerializeMessage(const T& msg)
    {
        static_assert(std::is_standard_layout<T>::value, "T must be standard layout");
        static_assert(std::is_trivially_copyable<T>::value, "T must be trivially copyable");
        static_assert(offsetof(T, hdr) == 0, "First member must be 'hdr'");
        static_assert(sizeof(MessageHeader) == headerSize, "MessageHeader size miss matching");

        T tmp = msg;
        tmp.hdr.size = static_cast<uint32_t>(sizeof(T));
        if (tmp.hdr.uniqIdx == 0) tmp.hdr.uniqIdx = next_message_id();

        std::vector<uint8_t> out(sizeof(T));
        std::memcpy(out.data(), &tmp, sizeof(T));
        return out;
    }

    template <typename T>
    inline bool DeserializeMessage(const uint8_t* data, size_t len, T& out) 
    {
        if (len != sizeof(T)) return false;
        std::memcpy(&out, data, sizeof(T));
        return out.hdr.size == sizeof(T); // 헤더 크기 확인
    }

    // ----------- 프레임(길이+Payload) 송수신 저수준 ----------
    bool send_all(SOCKET s, const uint8_t* p, size_t n);
    bool recv_all(SOCKET s, uint8_t* p, size_t n);
    bool set_keepalive(SOCKET s);

    // ================= 클라이언트 =================
    class SimpleTcpClient
    {
    public:
        using RawHandler = std::function<void(const std::vector<uint8_t>&)>; // payload
        using EventHandler = std::function<void()>;

        SimpleTcpClient(const std::string& host, uint16_t port);
        ~SimpleTcpClient();

        void start();
        void stop();
        bool isConnected() const { return connected_.load(std::memory_order_acquire); }

        // 길이 프레임 전송(상위는 구조체→바이너리 후 이걸 호출)
        bool sendFrame(const void* data, size_t len);

        // 이벤트 (setter는 콜백 교체 레이스 방지)
        void onConnect(EventHandler h) { std::lock_guard<std::mutex> lg(cbMtx_); onConn_ = std::move(h); }
        void onDisconnect(EventHandler h) { std::lock_guard<std::mutex> lg(cbMtx_); onDisc_ = std::move(h); }
        void onRaw(RawHandler h) { std::lock_guard<std::mutex> lg(cbMtx_); onRaw_ = std::move(h); }

        // ---- 구조체 전송: 공통 헤더를 첫 멤버로 갖는 T만 허용 ----
        template <typename T>
        bool sendMessage(const T& msg)
        {
            auto bin = SerializeMessage(msg);
            return sendFrame(bin.data(), bin.size());
        }

    private:
        std::string host_; uint16_t port_;
        std::atomic<bool> running_{ false }, connected_{ false };
        SOCKET sock_{ INVALID_SOCKET };
        std::thread th_;
        std::mutex txMtx_;     // 소켓/전송 보호
        std::mutex cbMtx_;     // 콜백 보호

        RawHandler onRaw_;
        EventHandler onConn_, onDisc_;

        bool connect_once();
        void loop();
        void handle_disconnect();
        void close_sock();
    };

    // ================= 서버(멀티클라) =================
    class SimpleTcpServer
    {
    public:
        using RawHandler = std::function<void(int /*cid*/, const std::vector<uint8_t>&)>;
        using EventHandlerI = std::function<void(int /*cid*/)>;

        explicit SimpleTcpServer(uint16_t port);
        ~SimpleTcpServer();

        void start();
        void stop();

        bool sendTo(int cid, const void* data, size_t len);
        void broadcast(const void* data, size_t len);

        template <typename T>
        bool sendMessageTo(int cid, const T& msg)
        {
            auto bin = SerializeMessage(msg);
            return sendTo(cid, bin.data(), bin.size());
        }

        template <typename T>
        void broadcastMessage(const T& msg)
        {
            auto bin = SerializeMessage(msg);
            broadcast(bin.data(), bin.size());
        }

        // 이벤트(setter는 콜백 교체 레이스 방지)
        void onClientConnect(EventHandlerI h) { std::lock_guard<std::mutex> lg(cbMtx_); onConn_ = std::move(h); }
        void onClientDisconnect(EventHandlerI h) { std::lock_guard<std::mutex> lg(cbMtx_); onDisc_ = std::move(h); }
        void onClientRaw(RawHandler h) { std::lock_guard<std::mutex> lg(cbMtx_); onRaw_ = std::move(h); }

    private:
        uint16_t port_;
        std::atomic<bool> running_{ false };
        SOCKET lstn_{ INVALID_SOCKET };
        std::thread thAccept_;

        std::mutex mtx_;                  // 클라 맵 보호
        std::map<int, SOCKET> clients_;
        int nextId_{ 1 };

        std::mutex cbMtx_;                // 콜백 보호
        RawHandler   onRaw_;
        EventHandlerI onConn_, onDisc_;

        void accept_loop();
        void client_loop(int cid, SOCKET s);
        void drop(int cid);
        void drop_nolock(int cid);
    };

    // ================= (header,subheader) → 구조체 핸들러 디스패처 =================
    // 간단 잠금으로 등록/디스패치 동시성 보호(심플 버전)
    class ProtoDispatcher
    {
    public:
        // 클라이언트용
        template <typename T>
        void on(uint16_t header, uint16_t subheader, std::function<void(const T&)> handler)
        {
            static_assert(std::is_standard_layout<T>::value, "T must be standard layout");
            static_assert(std::is_trivially_copyable<T>::value, "T must be trivially copyable");
            Key k{ header, subheader };
            Entry e;
            e.size = static_cast<uint32_t>(sizeof(T));
            e.cb = [handler = std::move(handler)](const uint8_t* p, size_t n)
                {
                    T obj{};
                    if (!DeserializeMessage<T>(p, n, obj)) return;
                    handler(obj);
                };
            std::lock_guard<std::mutex> lg(mx_);
            table_[k] = std::move(e);
        }

        // 서버용(클라이언트 id 포함)
        template <typename T>
        void onServer(uint16_t header, uint16_t subheader, std::function<void(int /*cid*/, const T&)> handler)
        {
            static_assert(std::is_standard_layout<T>::value, "T must be standard layout");
            static_assert(std::is_trivially_copyable<T>::value, "T must be trivially copyable");
            Key k{ header, subheader };
            Entry e;
            e.size = static_cast<uint32_t>(sizeof(T));
            e.cb_srv = [handler = std::move(handler)](int cid, const uint8_t* p, size_t n)
                {
                    T obj{};
                    if (!DeserializeMessage<T>(p, n, obj)) return;
                    handler(cid, obj);
                };
            std::lock_guard<std::mutex> lg(mx_);
            table_[k] = std::move(e);
        }

        // 디스패치 (클라이언트)
        bool dispatch(const std::vector<uint8_t>& payload) const
        {
            if (payload.size() < sizeof(MessageHeader)) return false;

            MessageHeader hdr{};
            std::memcpy(&hdr, payload.data(), sizeof(hdr));

            std::lock_guard<std::mutex> lg(mx_);
            auto it = table_.find(Key{ hdr.header, hdr.subheader });
            if (it == table_.end()) return false;
            if (hdr.size != it->second.size) return false;
            if (!it->second.cb) return false;

            it->second.cb(payload.data(), payload.size());
            return true;
        }

        // 디스패치 (서버)
        bool dispatchServer(int cid, const std::vector<uint8_t>& payload) const
        {
            if (payload.size() < sizeof(MessageHeader)) return false;

            MessageHeader hdr{};
            std::memcpy(&hdr, payload.data(), sizeof(hdr));

            std::lock_guard<std::mutex> lg(mx_);
            auto it = table_.find(Key{ hdr.header, hdr.subheader });
            if (it == table_.end()) return false;
            if (hdr.size != it->second.size) return false;
            if (!it->second.cb_srv) return false;

            it->second.cb_srv(cid, payload.data(), payload.size());
            return true;
        }

    private:
        struct Key
        {
            uint16_t h, sh;
            bool operator<(const Key& o) const
            {
                return (h < o.h) || (h == o.h && sh < o.sh);
            }
        };
        struct Entry
        {
            uint32_t size{};
            std::function<void(const uint8_t*, size_t)> cb;                 // client
            std::function<void(int, const uint8_t*, size_t)> cb_srv;        // server
        };

        // mutable로 const 디스패치에서 잠금 허용(심플버전)
        mutable std::mutex mx_;
        std::map<Key, Entry> table_;
    };

} // namespace mini
