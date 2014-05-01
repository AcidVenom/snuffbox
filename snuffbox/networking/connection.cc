#include "../../snuffbox/networking/connection.h"
#include "../../snuffbox/logging.h"
#include "../../snuffbox/environment.h"
#include "../../snuffbox/game.h"

namespace snuffbox
{
	namespace environment {
		namespace
		{
			Connection* globalInstance = nullptr;
		}

		bool has_console() { return globalInstance != nullptr; }

		Connection& console()
		{
			SNUFF_ASSERT_NOTNULL(globalInstance);
			return *globalInstance;
		}
	}
}

namespace snuffbox
{
	//-----------------------------------------------------------------------------------
	Connection::Connection() : socket_(INVALID_SOCKET), client_(INVALID_SOCKET)
	{

	}

	//-----------------------------------------------------------------------------------
	Connection::~Connection()
	{
		Destroy();
	}

	//-----------------------------------------------------------------------------------
	void Connection::Destroy()
	{
		closesocket(socket_);
		WSACleanup();

		if (thread_.joinable())
			thread_.join();
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

		thread_ = std::thread(&Connection::Listen,this);
	}

	//-----------------------------------------------------------------------------------
	void Connection::Send(LogSeverity severity,const char* msg)
	{
		const char* sev = "";
		switch (severity)
		{
		case LogSeverity::kInfo:
			sev = "/i";
			break;
		case LogSeverity::kDebug:
			sev = "/d";
			break;
		case LogSeverity::kWarning:
			sev = "/w";
			break;
		case LogSeverity::kSuccess:
			sev = "/s";
			break;
		case LogSeverity::kError:
			sev = "/e";
			break;
		case LogSeverity::kFatal:
			sev = "/f";
			break;
		}
		std::string message(msg);
		std::string result = sev + message;

		send(client_, result.c_str(), SNUFF_DEFAULT_BUFFER, 0);
	}

	//-----------------------------------------------------------------------------------
	void Connection::Listen()
	{
		int result = listen(socket_, 1);
		SNUFF_XASSERT(result == 0, "Failed listening the socket!");

		while (client_ == SOCKET_ERROR && environment::has_game())
		{
			SNUFF_LOG_INFO("Awaiting console connection..");
			sockaddr incoming;
			int incomingSize = static_cast<int>(sizeof(incoming));
			client_ = accept(socket_, (sockaddr* )&incoming, &incomingSize);
		}

		if (client_ != SOCKET_ERROR)
		{
			environment::globalInstance = this;
			connected_ = true;
			SNUFF_LOG_INFO("Received welcome message from the server");
		}
	}
}