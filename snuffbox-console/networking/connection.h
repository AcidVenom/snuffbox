#pragma once

#define NOMINMAX
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <qapplication.h>
#include <thread>

#define SNUFF_DEFAULT_PORT "1337"
#define SNUFF_DEFAULT_BUFFER 64

class Terminal;
class SocketThread;

	/**
	* @class console::Connection
	* @brief Used to connect to the engine
	* @author Daniël Konings
	*/
	class Connection : public QObject
	{
	public:
		Q_OBJECT
	public:
		/// Default constructor
		Connection(Terminal* parent);

		/// Default destructor
		~Connection();

		/// Initialises the connection for use
		void Initialise();

		/// Connects to the engine
		void Connect();

		/// Receives messages
		void Receive();

		std::thread& thread(){ return thread_; }
		Terminal* parent(){ return parent_; }
		bool connected(){ return connected_; }

	private:
		WSADATA data_; ///< Holds WinSock data
		SOCKET socket_; ///< The socket the server is on
		addrinfo* info_; ///< Holds address info
		bool connected_; ///< Are we connected?
		Terminal* parent_; ///< The terminal widget to stream to
		int lastSeverity_;
		QString lastMessage_;
		std::thread thread_;

	signals:
		void SendLog(int sev, QString msg);
	};