// MiniTcpProtoWrap14.h  (C++14)
#pragma once
#include "MiniTcpProto.h"
#include <string>
#include <functional>
#include <mutex>

namespace mini {
	namespace wrap {

		// ---- 로깅/훅 정의 ----
		enum class LogLevel { Debug, Info, Warn, Error };

		class ILogger {
		public:
			virtual ~ILogger() {}
			virtual void write(LogLevel lv, const std::string& msg) = 0;
		};

		// 서버 훅: cid 포함
		class IServerHook {
		public:
			virtual ~IServerHook() {}
			virtual void onEvent(int cid, const std::string& note) = 0; // Connected/Disconnected/SendFail/DispatchDrop 등 상황 메시지 전달
		};
		// 클라 훅: cid 없음
		class IClientHook {
		public:
			virtual ~IClientHook() {}
			virtual void onEvent(const std::string& note) = 0;
		};

		// ---------------- 서버 래퍼 ----------------
		class TcpProtoServer {
		public:
			class Handlers {
			public:
				std::function<void(int)> onConnect;
				std::function<void(int)> onDisconnect;
			};

			explicit TcpProtoServer(uint16_t port)
				: server_(port)
			{
				// raw → dispatcher
				server_.onClientRaw([this](int cid, const std::vector<uint8_t>& bin) {
					if (!disp_.dispatchServer(cid, bin)) {
						log(LogLevel::Warn, "[server] dispatch drop");
						hook(cid, "dispatch drop");
					}
					});

				// 이벤트 스냅샷 호출
				server_.onClientConnect([this](int cid) {
					log(LogLevel::Info, "[server] connected");
					hook(cid, "connected");
					Handlers h = snapshotHandlers();
					if (h.onConnect) h.onConnect(cid);
					});
				server_.onClientDisconnect([this](int cid) {
					log(LogLevel::Info, "[server] disconnected");
					hook(cid, "disconnected");
					Handlers h = snapshotHandlers();
					if (h.onDisconnect) h.onDisconnect(cid);
					});
			}

			~TcpProtoServer() { stop(); }

			// 수명
			void start() { server_.start(); }
			void stop() { server_.stop(); }

			// 설정
			void setHandlers(const Handlers& h) { std::lock_guard<std::mutex> g(mx_); handlers_ = h; }
			void setLogger(ILogger* lg) { std::lock_guard<std::mutex> g(mx_); logger_ = lg; }
			void setHook(IServerHook* hk) { std::lock_guard<std::mutex> g(mx_); hook_ = hk; }

			// 메시지 핸들러 등록(서버: cid 포함)
			template <typename T>
			void on(uint16_t header, uint16_t subheader, std::function<void(int, const T&)> fn) {
				disp_.onServer<T>(header, subheader, fn);
			}

			// 송신
			template <typename T>
			bool sendTo(int cid, const T& msg) {
				if (server_.sendMessageTo<T>(cid, msg)) return true;
				log(LogLevel::Warn, "[server] sendTo fail");
				hook(cid, "sendTo fail");
				return false;
			}
			template <typename T>
			void broadcast(const T& msg) {
				server_.broadcastMessage<T>(msg);
			}
			bool sendFrameTo(int cid, const void* data, size_t len) {
				if (server_.sendTo(cid, data, len)) return true;
				log(LogLevel::Warn, "[server] sendFrameTo fail");
				hook(cid, "sendFrameTo fail");
				return false;
			}
			void broadcastFrame(const void* data, size_t len) {
				server_.broadcast(data, len);
			}

		private:
			Handlers snapshotHandlers() const { std::lock_guard<std::mutex> g(mx_); return handlers_; }

			void log(LogLevel lv, const std::string& msg) {
				std::lock_guard<std::mutex> g(mx_);
				if (logger_) logger_->write(lv, msg);
			}
			void hook(int cid, const std::string& note) {
				std::lock_guard<std::mutex> g(mx_);
				if (hook_) hook_->onEvent(cid, note);
			}

			SimpleTcpServer  server_;
			ProtoDispatcher  disp_;

			mutable std::mutex mx_;
			Handlers   handlers_;
			ILogger* logger_ = nullptr;
			IServerHook* hook_ = nullptr;
		};

		// ---------------- 클라이언트 래퍼 ----------------
		class TcpProtoClient
		{
		public:
			class Handlers
			{
			public:
				std::function<void()> onConnect;
				std::function<void()> onDisconnect;
			};

			TcpProtoClient() = default;


			TcpProtoClient(const std::string& host, uint16_t port)
				: client_(host, port)
			{
				client_.onRaw([this](const std::vector<uint8_t>& bin) {
					if (!disp_.dispatch(bin)) 
					{
						log(LogLevel::Warn, "[client] dispatch drop");
						hook("dispatch drop");
						
					}
					});
				client_.onConnect([this]() 
					{
						__debugbreak();
					log(LogLevel::Info, "[client] connected");
					hook("connected");
					Handlers h = snapshotHandlers();
					if (h.onConnect) h.onConnect();
					});
				client_.onDisconnect([this]() 
					{
					log(LogLevel::Info, "[client] disconnected");
					hook("disconnected");
					Handlers h = snapshotHandlers();
					if (h.onDisconnect) h.onDisconnect();
					});
			}

			~TcpProtoClient() { stop(); }

			// 수명
			void start() { client_.start(); }
			void stop() { client_.stop(); }
			bool isConnected() const
			{
				return client_.isConnected();
			}
			void SetEndpoint(std::string host, uint16_t port)
			{
				client_.setEndpoint(host, port);

				client_.onRaw([this](const std::vector<uint8_t>& bin) {
					if (!disp_.dispatch(bin))
					{
						log(LogLevel::Warn, "[client] dispatch drop");
						hook("dispatch drop");

					}
					});
				client_.onConnect([this]()
					{
						log(LogLevel::Info, "[client] connected");
						hook("connected");
						Handlers h = snapshotHandlers();
						if (h.onConnect) h.onConnect();
					});
				client_.onDisconnect([this]()
					{
						log(LogLevel::Info, "[client] disconnected");
						hook("disconnected");
						Handlers h = snapshotHandlers();
						if (h.onDisconnect) h.onDisconnect();
					});

			}

			// 설정
			void setHandlers(const Handlers& h) { std::lock_guard<std::mutex> g(mx_); handlers_ = h; }
			void setLogger(ILogger* lg) { std::lock_guard<std::mutex> g(mx_); logger_ = lg; }
			void setHook(IClientHook* hk) { std::lock_guard<std::mutex> g(mx_); hook_ = hk; }

			// 메시지 핸들러 등록(클라)
			template <typename T>
			void on(uint16_t header, uint16_t subheader, std::function<void(const T&)> fn) 
			{
				disp_.on<T>(header, subheader, fn);
			}

			// 송신
			template <typename T>
			bool send(const T& msg) 
			{
				if (client_.sendMessage<T>(msg)) return true;
				log(LogLevel::Warn, "[client] send fail");
				hook("send fail");
				return false;
			}

			bool sendFrame(const void* data, size_t len) 
			{
				if (client_.sendFrame(data, len)) return true;
				log(LogLevel::Warn, "[client] sendFrame fail");
				hook("sendFrame fail");
				return false;
			}

		private:
			Handlers snapshotHandlers() const { std::lock_guard<std::mutex> g(mx_); return handlers_; }

			void log(LogLevel lv, const std::string& msg) 
			{
				std::lock_guard<std::mutex> g(mx_);
				if (logger_) logger_->write(lv, msg);
			}

			void hook(const std::string& note) 
			{
				std::lock_guard<std::mutex> g(mx_);
				if (hook_) hook_->onEvent(note);
			}

			SimpleTcpClient  client_;
			ProtoDispatcher  disp_;

			mutable std::mutex mx_;
			Handlers   handlers_;
			ILogger* logger_ = nullptr;
			IClientHook* hook_ = nullptr;
		};

	}
} // namespace mini::wrap14
