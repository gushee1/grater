#pragma once

#include "net_common.h"
#include "net_tsqueue.h"
#include "net_message.h"
#include "net_connection.h"

namespace grater 
{
	namespace network
	{
		template <typename T>
		class Server 
		{
		public:
			Server(uint16_t port) 
				: asioAcceptor(asioContext, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port))
			{

			}

			~Server()
			{
				Stop();
			}

			bool Start()
			{
				bool connectionSuccess = true;

				try 
				{
					WaitForClientConnection();

					contextThread = std::thread([this]() { asioContext.run(); });
				}
				catch (std::exception& e)
				{
					std::cerr << "[SERVER] Exception: " << e.what() << std::endl;
					connectionSuccess = false;
				}

				if (connectionSuccess) 
				{
					std::cout << "[SERVER] Started!" << std::endl;
				}

				return connectionSuccess;
			}

			void Stop() 
			{
				asioContext.stop();

				if (contextThread.joinable()) 
				{
					contextThread.join();
				}

				std::cout << "[SERVER] Stopped!" << std::endl;
			}

			void WaitForClientConnection()
			{
				asioAcceptor.async_accept(
					[this](std::error_code ec, asio::ip::tcp::socket socket)
					{
						if (!ec)
						{
							std::cout << "[SERVER] New Connection: " << socket.remote_endpoint() << std::endl;

							std::shared_ptr<Connection<T>> newConnection =
								std::make_shared<Connection<T>>(Connection<T>::Owner::server,
									asioContext, std::move(socket), messagesIn);

							bool connectionAccepted = OnClientConnect(newConnection);
							if (connectionAccepted) 
							{
								connections.push_back(std::move(newConnection));
								std::shared_ptr<Connection<T>> newConnectionRef = connections.back();
								newConnectionRef->ConnectToClient(uniqueIDCounter++);

								std::cout << "[" << newConnectionRef->GetID() << "] Connection Approved\n";
							}
							else 
							{
								std::cout << "[-----] Connection Denied" << std::endl;
							}
						}
						else
						{
							std::cout << "[SERVER] New Connection Error: " << ec.message() << std::endl;
						}

						WaitForClientConnection();
					});
			}

			void MessageClient(std::shared_ptr<Connection<T>> client, const message<T>& msg) 
			{
				if (client && client->IsConnected()) 
				{
					client->Send(msg);
				}
				else 
				{
					OnClientDisconnect(client);
					client.reset();
					connections.erase(std::remove(connections.begin(), connections.end(), client), connections.end());
				}
			}

			void MessageAllClients(const message<T>& msg, std::shared_ptr<Connection<T>> clientToIgnore)
			{
				bool invalidClientExists = false;

				for (std::shared_ptr<Connection<T>>& client : connections) 
				{
					if (client && client->IsConnected())
					{
						if(client != clientToIgnore)
						{
							client->Send(msg);
						}
					}
					else
					{
						OnClientDisconnect(client);
						client.reset();
						invalidClientExists = true;
					}
				}

				if (invalidClientExists) 
				{
					connections.erase(std::remove(connections.begin(), connections.end(), nullptr));
				}
			}

			void Update() 
			{
				//std::cout << messagesIn.count() << std::endl;

				while (!messagesIn.empty()) 
				{
					owned_message<T> msg = messagesIn.pop_front();

					OnMessage(msg.owner, msg.msg);
				}

				message<GraterMessageTypes> pulse;
				pulse.header.id = GraterMessageTypes::MessageAll;
				//MessageAllClients(pulse, nullptr);
			}

		protected:
			//returning false will veto client from connecting to server
			bool OnClientConnect(std::shared_ptr<Connection<T>> client) 
			{
				return true;
			}

			void OnClientDisconnect(std::shared_ptr<Connection<T>> client) 
			{

			}
			
			void OnMessage(std::shared_ptr<Connection<T>> client, message<T> msg) 
			{
				int key = 0;

				switch (msg.header.id) {
				case GraterMessageTypes::Input:
					msg >> key;
					std::cout << "[" << client->GetID() << "]: Keycode " << key << " pressed." << std::endl;
					break;
				default:
					break;
				}
			}

			tsqueue<owned_message<T>> messagesIn;

			std::deque<std::shared_ptr<Connection<T>>> connections;

			asio::io_context asioContext;
			std::thread contextThread;

			asio::ip::tcp::acceptor asioAcceptor;

			uint32_t uniqueIDCounter = 10000;
		};
	}
}
