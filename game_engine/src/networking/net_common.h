#pragma once
//Based on OneLoneCoder's Networking in C++ series, found at https://www.youtube.com/watch?v=2hNdkYInj4g&t=31s

//STL
#include <vector>
#include <mutex>
#include <thread>
#include <iostream>
#include <memory>
#include <deque>
#include <string>
#include <atomic>

//Necessary for Asio
#ifdef _WIN32
#define _WIN32_WINNT 0x0A00
#endif

//Third Party
#define ASIO_STANDALONE
#include "asio.hpp"
#include "asio/ts/buffer.hpp"
#include "asio/ts/internet.hpp"
#include "SDL.h"

namespace grater {
	namespace network {
		enum class GraterMessageTypes : uint32_t
		{
			ServerAccept,
			ServerDeny,
			ServerPing,
			MessageAll,
			ServerMessage,
			Input,
			Render
		};
	}
}
