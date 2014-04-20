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

	//-----------------------------------------------------------------------------------
	void Connection::Receive(ConsoleWidget* console)
	{
		int recvbuflen = SNUFF_DEFAULT_BUFFER;

		char *sendbuf = "Welcome message from console";
		char recvbuf[SNUFF_DEFAULT_BUFFER];

		int iResult;

		do {
			send(socket_, sendbuf, SNUFF_DEFAULT_BUFFER, 0);
			iResult = recv(socket_, recvbuf, recvbuflen, 0);
			if (iResult > 0)
			{
				std::string severity = "";
				severity += recvbuf[0];
				severity += recvbuf[1];

				LogSeverity sev = LogSeverity::kInfo;

				if (strcmp(severity.c_str(), "/d") == 0)
				{
					sev = LogSeverity::kDebug;
				}
				if (strcmp(severity.c_str(), "/w") == 0)
				{
					sev = LogSeverity::kWarning;
				}
				if (strcmp(severity.c_str(), "/s") == 0)
				{
					sev = LogSeverity::kSuccess;
				}
				if (strcmp(severity.c_str(), "/e") == 0)
				{
					sev = LogSeverity::kError;
				}
				if (strcmp(severity.c_str(), "/f") == 0)
				{
					sev = LogSeverity::kFatal;
				}
				console->AddLine(sev, &recvbuf[2]);
			}
			else
			{
				console->AddLine(LogSeverity::kWarning, "The connection with the server was closed");
			}
		} while (iResult > 0);

		console->set_connected(false);
	}

	//-----------------------------------------------------------------------------------
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

		console.set_connected(true);

		return "Success";
	}
