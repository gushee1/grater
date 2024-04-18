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
			Default,
			ServerAccept,
			ServerDeny,
			ServerPing,
			MessageAll,
			ServerMessage,
			Input,
			Render
		};

		template<typename T>
		class message;

		template<typename T>
		std::vector<uint8_t> serialize_message(const message<T>& msg) {
			std::vector<uint8_t> buffer;

			uint32_t header_id = static_cast<uint32_t>(msg.header.id);
			uint32_t header_size = static_cast<uint32_t>(msg.header.size);

			buffer.insert(buffer.end(), reinterpret_cast<uint8_t*>(&header_id), reinterpret_cast<uint8_t*>(&header_id) + sizeof(header_id));
			buffer.insert(buffer.end(), reinterpret_cast<uint8_t*>(&header_size), reinterpret_cast<uint8_t*>(&header_size) + sizeof(header_size));

			for (auto& val : msg.body) {
				buffer.insert(buffer.end(), reinterpret_cast<uint8_t*>(&val), reinterpret_cast<uint8_t*>(&val) + sizeof(int));
			}

			return buffer;
		}

		template<typename T>
		message<T> deserialize_message(const std::vector<uint8_t>& buffer) {
			message<T> msg;

			uint8_t const* ptr = buffer.data();

			uint32_t header_id;
			std::memcpy(&header_id, ptr, sizeof(header_id));
			msg.header.id = static_cast<GraterMessageTypes>(header_id);
			ptr += sizeof(header_id);

			std::memcpy(&msg.header.size, ptr, sizeof(msg.header.size));
			ptr += sizeof(msg.header.size);

			msg.body.resize(msg.header.size);

			for (int& val : msg.body) {
				std::memcpy(&val, ptr, sizeof(val));
				ptr += sizeof(val);
			}

			return msg;
		}
	}
}
