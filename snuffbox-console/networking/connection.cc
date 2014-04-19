#include "../../snuffbox-console/networking/connection.h"
#include "../console_widget.h"


	//-----------------------------------------------------------------------------------
	Connection::Connection() : socket_(INVALID_SOCKET)
	{

	}

	//-----------------------------------------------------------------------------------
	Connection::~Connection()
	{

	}

	//-----------------------------------------------------------------------------------
	const char* Connection::Initialise()
	{
		socket_ = INVALID_SOCKET;

		int result = WSAStartup(MAKEWORD(2, 2), &data_);
		if (result != 0)
		{
			return "Could not setup WinSock!";
		}

		addrinfo hints;
		
		ZeroMemory(&hints, sizeof (hints));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;

		result = getaddrinfo("127.0.0.1", SNUFF_DEFAULT_PORT, &hints, &info_);
		if (result != 0)
		{
			return "Failed getting address info!";
		}

		return "Success";
	}

	const char* Connection::Connect(ConsoleWidget& console, QApplication& app)
	{
		app.processEvents();
		addrinfo* ip;

		for (addrinfo* p = info_; p != NULL && socket_ == INVALID_SOCKET; p = p->ai_next)
		{
			ip = p;
			socket_ = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
			if (socket_ == INVALID_SOCKET)
			{
				return "Error creating socket!";
			}
			else if (connect(socket_, p->ai_addr, p->ai_addrlen) < 0)
			{
				closesocket(socket_);
				return "Could not resolve connection with engine!";
			}
		}

		connected_ = true;
		return "Success";
	}
