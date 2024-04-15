#ifndef NET_CLIENT_H
#define NET_CLIENT_H

#include "net_common.h"
#include "net_message.h"
#include "net_tsqueue.h"
#include "net_connection.h"

namespace grater {
	namespace network {
		template <typename T>
		class Client 
		{
		public:
			Client() : socket(asioContext) 
			{
				//initialize socket with io context
			}

			~Client() 
			{
				Disconnect();
			}

			bool Connect(const std::string& host, const uint16_t port) 
			{
				try 
				{
					asio::ip::tcp::resolver resolver(asioContext);
					asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(host, std::to_string(port));

					connectionRef = std::make_unique<Connection<T>>(
						Connection<T>::Owner::client,
						asioContext,
						asio::ip::tcp::socket(asioContext),
						messagesIn
					);

					connectionRef->ConnectToServer(endpoints);

					contextThread = std::thread([this]() { asioContext.run(); });

					std::cout << "[CLIENT] Connected to server at " << host << " on port " << port << std::endl;
				}
				catch (std::exception e) 
				{
					std::cout << "Client Exception: " << e.what() << std::endl;
					return false;
				}
			}

			void Disconnect() 
			{
				if (IsConnected()) 
				{
					connectionRef->Disconnect();
				}

				asioContext.stop();
				if (contextThread.joinable()) 
				{
					contextThread.join();
				}

				connectionRef.release();
			}

			bool IsConnected() 
			{
				if (connectionRef) 
				{
					return connectionRef->IsConnected();
				}
				else
				{
					return false;
				}
			}

			void Update() {

				while (!messagesIn.empty())
				{
					std::cout << "[" << connectionRef->GetID() << "] ";
					owned_message<T> msg = messagesIn.pop_front();

					OnMessage(msg.owner, msg.msg);
				}
			}

			tsqueue<owned_message<T>>& GetIncomingMessages() 
			{
				return messagesIn;
			}

		public:

			void SendKeyboardInput(int key) {
				message<GraterMessageTypes> msg;
				msg.header.id = GraterMessageTypes::Input;

				msg << key;
				connectionRef->Send(msg);
			}

		protected:
			void OnMessage(std::shared_ptr<Connection<T>> server, message<T> msg)
			{
				switch (msg.header.id) {
				case GraterMessageTypes::MessageAll:
					std::cout << "Receiving render calls from server." << std::endl;
					break;
				default:
					break;
				}
			}

			//asio needs a context to manage data transfer, and the context will work in its own thread
			asio::io_context asioContext;
			std::thread contextThread;
			//hardware socket to connect to server
			asio::ip::tcp::socket socket;

			std::unique_ptr<Connection<T>> connectionRef;

		private:
			tsqueue<owned_message<T>> messagesIn;
		};
	}
}

#endif // !NET_CLIENT_H

