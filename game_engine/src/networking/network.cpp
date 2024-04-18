#include "network.h"

namespace grater {
	namespace network {
		extern std::unique_ptr<Server<GraterMessageTypes>> server = nullptr;
		extern std::unique_ptr<Client<GraterMessageTypes>> client = nullptr;

		extern std::atomic<MultiplayerState> multiplayerState(MultiplayerState::SINGLEPLAYER);


		MultiplayerState GetMultiplayerState()
		{
			return multiplayerState.load(std::memory_order_acquire);
		}

		void SetMultiplayerState(MultiplayerState newState)
		{
			multiplayerState.store(newState, std::memory_order_release);
		}

		void OpenServer(uint16_t port)
		{
			if (server != nullptr) {
				std::cout << "Error: server is already open!" << std::endl;
				return;
			}
			server = std::make_unique<Server<GraterMessageTypes>>(port);
			server->Start();
			SetMultiplayerState(MultiplayerState::SERVER);
		}

		void JoinServer(const std::string& host, uint16_t port)
		{
			SetMultiplayerState(MultiplayerState::CLIENT);
			client = std::make_unique<Client<GraterMessageTypes>>();
			client->Connect(host, port);
		}

	}
}