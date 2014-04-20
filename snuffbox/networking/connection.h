#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <thread>

#define SNUFF_DEFAULT_PORT "1337"
#define SNUFF_DEFAULT_BUFFER 4096
namespace snuffbox
{
	enum LogSeverity;
	/**
	* @class snuffbox::Connection
	* @brief Used to connect to the remote console
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
		void Initialise();

		/// Listens for incoming connections
		void Listen();

		/// Destroys the connection
		void Destroy();

		/// Sends a message to the remote console
		void Send(LogSeverity severity, const char* msg);

	private:
		WSADATA data_; ///< Holds WinSock data
		addrinfo* info_; ///< Holds WinSock address info
		SOCKET socket_; ///< The socket clients will listen to
		SOCKET client_; ///< The client socket
		bool connected_; ///< Is there a connection with a client?
		std::thread thread_; ///< The thread to work on
	};
}