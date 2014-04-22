#include "../../snuffbox-console/networking/connection.h"
#include "../terminal.h"
#include "../networking/moc_connection.cpp"
int(__stdcall *con)(SOCKET s, const sockaddr* name, int namelen) = &connect;
	//-----------------------------------------------------------------------------------
	Connection::Connection(Terminal* parent) : 
		socket_(INVALID_SOCKET), 
		connected_(false), 
		parent_(parent)
	{

	}

	//-----------------------------------------------------------------------------------
	Connection::~Connection()
	{
		closesocket(socket_);
		WSACleanup();
	}

	//-----------------------------------------------------------------------------------
	void Connection::Initialise()
	{
		socket_ = INVALID_SOCKET;

		int result = WSAStartup(MAKEWORD(2, 2), &data_);
		if (result != 0)
		{
			parent_->AddLine(LogSeverity::kFatal,"Could not setup WinSock!");
			return;
		}

		addrinfo hints;
		
		ZeroMemory(&hints, sizeof (hints));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;

		result = getaddrinfo("127.0.0.1", SNUFF_DEFAULT_PORT, &hints, &info_);
		if (result != 0)
		{
			parent_->AddLine(LogSeverity::kFatal, "Failed getting address info");
			return;
		}

		parent_->AddLine(LogSeverity::kSuccess, "Succesfully setup connection");
	}

	//-----------------------------------------------------------------------------------
	void Connection::Receive()
	{
		connect(this, SIGNAL(SendLog(int,QString)), parent_, SLOT(ReceiveMsg(int,QString)));
		int iResult;
		do {
			char recvbuf[SNUFF_DEFAULT_BUFFER];
			iResult = recv(socket_, recvbuf, SNUFF_DEFAULT_BUFFER, 0);
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
				if (recvbuf)
				{
					emit SendLog(static_cast<int>(sev), QString(&recvbuf[2]));
				}
			}
		} while (iResult > 0);

		emit SendLog(static_cast<int>(LogSeverity::kWarning), "Lost connection with the engine");
		connected_ = false;
	}

	//-----------------------------------------------------------------------------------
	void Connection::Connect()
	{
		addrinfo* ip;
		qApp->processEvents();
		for (addrinfo* p = info_; p != NULL && socket_ == INVALID_SOCKET; p = p->ai_next)
		{
			ip = p;
			socket_ = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
			if (socket_ == INVALID_SOCKET)
			{
				parent_->AddLine(LogSeverity::kFatal, "Error creating socket!");
				return;
			}
			else if (con(socket_, p->ai_addr, p->ai_addrlen) < 0)
			{
				closesocket(socket_);
				parent_->AddLine(LogSeverity::kFatal,"Could not resolve connection with engine!");
				return;
			}
		}
		parent_->AddLine(LogSeverity::kSuccess, "Successfully connected to the engine");
		connected_ = true;
		thread_ = std::thread(&Connection::Receive, this);
	}
