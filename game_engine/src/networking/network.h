#pragma once

#include "net_common.h"
#include "net_message.h"
#include "net_tsqueue.h"
#include "net_connection.h"
#include "net_client.h"
#include "net_server.h"

namespace grater {
	namespace network {
		extern std::unique_ptr<Server> server;
		extern std::unique_ptr<Client> client;

		enum MultiplayerState { SINGLEPLAYER, SERVER, CLIENT, BOTH };
		extern std::atomic<MultiplayerState> multiplayerState;

		MultiplayerState GetMultiplayerState();

		void SetMultiplayerState(MultiplayerState newState);
		
		void OpenServer(uint16_t port);

		void JoinServer(const std::string& host, uint16_t port);
	}
}
