#ifndef NET_CONNECTION_H
#define NET_CONNECTION_H

#include "net_common.h"
#include "net_tsqueue.h"
#include "net_message.h"

namespace grater 
{
	namespace network 
	{
		class Connection : public std::enable_shared_from_this<Connection> 
		{
		public:
			enum class Owner {
				SERVER,
				CLIENT
			};

			Connection(Owner parent, asio::io_context& asioContext, asio::ip::tcp::socket socket, tsqueue<owned_message>& messagesIn)
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

		private:
			std::vector<uint8_t> currentReadBuffer;
			std::vector<uint8_t> currentWriteBuffer;
			message tempMessageIn;

			void SerializeHeader(const message& msg) {
				currentWriteBuffer.clear();
				currentWriteBuffer.reserve(2);
				currentWriteBuffer.push_back(static_cast<uint8_t>(msg.header.id));
				currentWriteBuffer.push_back(msg.header.bodySize);
			}

			void SerializeBody(const message& msg) {
				currentWriteBuffer.clear();
				currentWriteBuffer.reserve(msg.header.bodySize);
				for (uint8_t byte : msg.body) {
					currentWriteBuffer.push_back(byte);
				}
			}

			void DeserializeHeader(const std::vector<uint8_t> buffer) {
				if (buffer.size() != 2) {
					std::cout << "Invalid Header Size" << std::endl;
					exit(101);
				}
				tempMessageIn.header.id = static_cast<GraterMessageTypes>(buffer[0]);
				tempMessageIn.header.bodySize = buffer[1];
			}

			void DeserializeBody(const std::vector<uint8_t> buffer) {
				for (uint8_t byte : buffer) {
					tempMessageIn.body.push_back(byte);
				}
			}

		public:

			void ConnectToClient(uint32_t uid = 0) {
				if (ownerType == Owner::SERVER) {
					if (socket.is_open()) {
						id = uid;
						ReadHeader();
					}
				}
			}

			void ConnectToServer(const asio::ip::tcp::resolver::results_type& endpoints) {
				if (ownerType == Owner::CLIENT) {
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
					asio::post(asioContext, [this]() { socket.close(); });
				}
			}

			bool IsConnected() const
			{
				return socket.is_open();
			}

			void Send(const message& msg) {
				asio::post(asioContext, 
					[this, msg]() {
						bool isWriting = !messagesOut.empty();
						messagesOut.push_back(msg);
						if (!isWriting) {
							WriteHeader();
						}
					}
				);
			}

		private:

			void ReadHeader() {
				tempMessageIn.reset();
				currentReadBuffer.clear();
				currentReadBuffer.resize(HEADER_SIZE);
				asio::async_read(socket, asio::buffer(currentReadBuffer),
					[this](std::error_code ec, std::size_t length) {
						if (!ec) {
							DeserializeHeader(currentReadBuffer);
							if (tempMessageIn.header.bodySize > 0) {
								ReadBody();
							}
							else {
								messagesIn.push_back({ this->shared_from_this(), tempMessageIn });
								ReadHeader();
							}
						}
						else {
							std::cout << "[" << id << "] Read Header Fail: " << ec.message() << "\n";
							socket.close();
						}
					}
				);
			}

			void ReadBody() {
				currentReadBuffer.resize(tempMessageIn.header.bodySize);
				asio::async_read(socket, asio::buffer(currentReadBuffer),
					[this](std::error_code ec, std::size_t length) {
						if (!ec) {
							DeserializeBody(currentReadBuffer);
							messagesIn.push_back({ this->shared_from_this(), tempMessageIn });
							ReadHeader();
						}
						else {
							std::cout << "[" << id << "] Read Body Fail: " << ec.message() << "\n";
							socket.close();
						}
					}
				);
			}

			void WriteHeader() {
				if (!messagesOut.empty()) {
					message currentMessage = messagesOut.front();
					SerializeHeader(messagesOut.front());
					asio::async_write(socket, asio::buffer(currentWriteBuffer), 
						[this](std::error_code ec, std::size_t length) {
							if (!ec) {
								message msgToWrite = messagesOut.pop_front();
								if (msgToWrite.header.bodySize > 0) {
									WriteBody(msgToWrite);
								}
								else {
									if (!messagesOut.empty()) {
										WriteHeader();
									}
								}
							}
							else {
								std::cout << "[" << id << "] Write Header Fail." << std::endl;
								socket.close();
							}
						}
					);
				}
			}

			void WriteBody(message messageToWrite) {
				SerializeBody(messageToWrite);
				asio::async_write(socket, asio::buffer(currentWriteBuffer),
					[this](std::error_code ec, std::size_t length) {
						if (!ec) {
							if (!messagesOut.empty()) {
								WriteHeader();
							}
						}
						else {
							std::cout << "[" << id << "] Write Body Fail." << std::endl;
							socket.close();
						}
					}
				);
			}
			
		protected:
			asio::ip::tcp::socket socket;

			asio::io_context& asioContext;

			tsqueue<message> messagesOut;

			//this queue will be provided by its client or server owner
			tsqueue<owned_message>& messagesIn;

			Owner ownerType = Owner::SERVER;
			uint32_t id = 0;
		};
	}
}

#endif // !NET_CONNECTION_H
