#pragma once

#define NOMINMAX
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <qapplication.h>

#define SNUFF_DEFAULT_PORT "1337"
#define SNUFF_DEFAULT_BUFFER 4096

class ConsoleWidget;

	/**
	* @class console::Connection
	* @brief Used to connect to the engine
	* @author Daniël Konings
	*/
	class Connection
	{
	public:
		/// Default constructor
		Connection();

		/// Default destructor
		~Connection();

		/// Initialises the connection for use
		const char* Initialise();

		/// Connects to the engine
		const char* Connect(ConsoleWidget& console, QApplication& app);

		/// Receives messages
		void Receive(ConsoleWidget* console);

	private:
		WSADATA data_; ///< Holds WinSock data
		SOCKET socket_; ///< The socket the server is on
		addrinfo* info_; ///< Holds address info
	};