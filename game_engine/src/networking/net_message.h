#ifndef NET_MESSAGE_H
#define NET_MESSAGE_H

//Internal
#include "net_common.h"

namespace grater {
	namespace network {
		const static size_t HEADER_SIZE = sizeof(GraterMessageTypes) + sizeof(uint8_t);

		struct message_header 
		{
			GraterMessageTypes id = GraterMessageTypes::Default;
			uint8_t bodySize = 0;
		};

		struct message
		{
			message_header header;
			std::vector<uint8_t> body;

			size_t size() const
			{
				return sizeof(message_header) + body.size();
			}

			/*
			friend std::ostream& operator << (std::ostream& os, const message& msg)
			{
				os << "ID:" << int(msg.header.id) << " Size:" << msg.header.bodySize;
				return os;
			}

			template <typename DataType>
			friend message& operator << (message& msg, const DataType& data)
			{
				static_assert(std::is_standard_layout<DataType>::value, "Data is too complex to be pushed into a vector");

				size_t i = msg.body.size();

				msg.body.resize(msg.body.size() + sizeof(DataType));
				std::memcpy(msg.body.data() + i, &data, sizeof(DataType));
				
				msg.header.bodySize = static_cast<uint8_t>(msg.size());

				return msg;
			}

			template <typename DataType>
			friend message& operator >> (message& msg, DataType& data)
			{
				static_assert(std::is_standard_layout<DataType>::value, "Data is too complex to be pushed into a vector");

				size_t i = msg.body.size() - sizeof(DataType);

				std::memcpy(&data, msg.body.data() + i, sizeof(DataType));
				msg.body.resize(i);

				msg.header.bodySize = msg.size();

				return msg;
			}
			*/

			void reset() {
				header.id = GraterMessageTypes::Default;
				header.bodySize = 0;
				body.clear();
			}
		};

		class Connection;

		struct owned_message
		{
			std::shared_ptr<Connection> owner;
			message msg;

			/*
			friend std::ostream& operator << (std::ostream& os, const owned_message<T>& msg)
			{
				os << msg.msg;
				return os;
			}
			*/
		};
	}
}

#endif // !NET_MESSAGE_H
