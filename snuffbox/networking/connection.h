#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>

#define SNUFF_DEFAULT_PORT "50"

namespace snuffbox
{
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

	private:
		WSADATA data_; ///< Holds WinSock data
		addrinfo* info_; ///< Holds WinSock address info
		SOCKET socket_; ///< The socket clients will listen to
		SOCKET client_; ///< The client socket
		bool connected_; ///< Is there a connection with a client?
	};
}