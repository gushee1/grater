#ifndef NET_CONNECTION_H
#define NET_CONNECTION_H

#include "net_common.h"
#include "net_tsqueue.h"
#include "net_message.h"

namespace grater 
{
	namespace network 
	{
		template <typename T>
		class Connection : public std::enable_shared_from_this<Connection<T>> 
		{
		public:
			enum class Owner {
				server,
				client
			};

			Connection(Owner parent, asio::io_context& asioContext, asio::ip::tcp::socket socket, tsqueue<owned_message<T>>& messagesIn)
				: asioContext(asioContext), socket(std::move(socket)), messagesIn(messagesIn)
			{
				ownerType = parent;
			}

			~Connection() 
			{

			}

			uint32_t GetID() const {
				return id;
			}

			void ConnectToClient(uint32_t uid = 0) {
				if (ownerType == Owner::server) {
					if (socket.is_open()) {
						id = uid;
						ReadHeader();
					}
				}
			}

			void ConnectToServer(const asio::ip::tcp::resolver::results_type& endpoints) {
				if (ownerType == Owner::client) {
					asio::async_connect(socket, endpoints,
						[this](std::error_code ec, asio::ip::tcp::endpoint endpoint) {
							if (!ec) {
								ReadHeader();
							}
							else {
								std::cout << "[" << id << "] Failed Connecting to Server." << std::endl;
							}
						});
				}
			}

			void Disconnect() {
				if (IsConnected()) {
					asio::post(asioContext, [this]() {socket.close(); });
				}
			}

			bool IsConnected() const
			{
				return socket.is_open();
			}

			void Send(const message<T>& msg) {
				asio::post(asioContext,
					[this, msg]() {
						bool isWritingMessage = !messagesOut.empty();
						messagesOut.push_back(msg);
						if (!isWritingMessage) {
							WriteHeader();
						}
					});
			}

		private:

			void ReadHeader() {
				asio::async_read(socket, asio::buffer(&tempMessageIn.header, sizeof(message_header<T>)),
					[this](std::error_code ec, std::size_t length) {
						if (!ec) {
							if (tempMessageIn.header.size > 0) {
								tempMessageIn.body.resize(tempMessageIn.header.size);
								ReadBody();
							}
							else {
								AddToIncomingMessageQueue();
							}
						}
						else {
							std::cout << "[" << id << "] Read Header Fail." << std::endl;
							socket.close();
						}
					});
			}

			void ReadBody() {
				asio::async_read(socket, asio::buffer(tempMessageIn.body.data(), tempMessageIn.body.size()),
					[this](std::error_code ec, std::size_t length) {
						if (!ec) {
							AddToIncomingMessageQueue();
						}
						else {
							std::cout << "[" << id << "] Read Body Fail." << std::endl;
							socket.close();
						}
					});
			}

			void WriteHeader() {
				asio::async_write(socket, asio::buffer(&messagesOut.front().header, sizeof(message_header<T>)),
					[this](std::error_code ec, std::size_t length) {
						if (!ec) {
							if (messagesOut.front().body.size() > 0) {
								WriteBody();
							}
							else {
								messagesOut.pop_front();

								if (!messagesOut.empty()) {
									WriteHeader();
								}
							}
						}
						else {
							std::cout << "[" << id << "] Write Header Fail." << std::endl;
							socket.close();
						}
					});
			}

			void WriteBody() {
				asio::async_write(socket, asio::buffer(messagesOut.front().body.data(), messagesOut.front().body.size()),
					[this](std::error_code ec, std::size_t length) {
						if (!ec) {
							messagesOut.pop_front();

							if (!messagesOut.empty()) {
								WriteHeader();
							}
						}
						else {
							std::cout << "[" << id << "] Write Body Fail." << std::endl;
							socket.close();
						}
					});
			}

			void AddToIncomingMessageQueue() {
				if (ownerType == Owner::server) {
					messagesIn.push_back({ this->shared_from_this(), tempMessageIn });
				}
				else {
					messagesIn.push_back({ nullptr, tempMessageIn });
				}

				ReadHeader();
			}
			
		protected:
			asio::ip::tcp::socket socket;

			asio::io_context& asioContext;

			tsqueue<message<T>> messagesOut;

			//this queue will be provided by its client or server owner
			tsqueue<owned_message<T>>& messagesIn;
			message<T> tempMessageIn;

			Owner ownerType = Owner::server;
			uint32_t id = 0;

		private:
			std::vector<uint8_t> rawDataIn;
			tsqueue<std::vector<uint8_t>> rawDataOut;
		};
	}
}

#endif // !NET_CONNECTION_H

