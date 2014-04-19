#include "../../snuffbox/networking/connection.h"
#include "../../snuffbox/logging.h"

namespace snuffbox
{
	//-----------------------------------------------------------------------------------
	Connection::Connection() : socket_(INVALID_SOCKET), client_(INVALID_SOCKET)
	{

	}

	//-----------------------------------------------------------------------------------
	Connection::~Connection()
	{

	}

	//-----------------------------------------------------------------------------------
	void Connection::Initialise()
	{
		int result = WSAStartup(MAKEWORD(2, 2), &data_);
		SNUFF_XASSERT(result == 0, "Could not setup WinSock!");

		addrinfo hints;

		ZeroMemory(&hints, sizeof (hints));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		
		result = getaddrinfo("127.0.0.1", SNUFF_DEFAULT_PORT, &hints, &info_);
		SNUFF_XASSERT(result == 0, "Failed getting address info!");
		
		addrinfo* ip;

		for (addrinfo* p = info_; p != NULL && socket_ == INVALID_SOCKET; p = p->ai_next)
		{
			ip = p;
			socket_ = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
			if (socket_ == INVALID_SOCKET)
			{
				SNUFF_ASSERT("Created an invalid socket!");
			}
			else if (bind(socket_, p->ai_addr, static_cast<int>(p->ai_addrlen)) == SOCKET_ERROR)
			{
				closesocket(socket_);
				SNUFF_ASSERT("Could not bind socket!");
			}
		}

		freeaddrinfo(info_);
	}

	void Connection::Listen()
	{
		int result = listen(socket_, 1);
		SNUFF_XASSERT(result == 0, "Failed listening the socket!");

		while (client_ == SOCKET_ERROR)
		{
			SNUFF_LOG_INFO("Awaiting console connection..");
			sockaddr incoming;
			int incomingSize = static_cast<int>(sizeof(incoming));
			client_ = accept(socket_, (sockaddr* )&incoming, &incomingSize);
		}

		SNUFF_LOG_SUCCESS("Successfully established a connection with the console");
	}
}