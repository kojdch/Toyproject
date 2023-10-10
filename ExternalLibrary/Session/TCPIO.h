#pragma once

namespace TCPIO {
	const void ConsoleOut(const char* a);
	const void ConsoleOut(const wchar_t* a);


	class TCP
	{
	public:
		TCP(const int threadCount, const int port);
		virtual ~TCP() = default;
		const void InitReceive();

	private:
		SOCKET _serviceSocket;
		struct sockaddr_in _address;
		HANDLE _ioHandle;

		std::map<int, std::thread> _thread;
		HANDLE* _threadHandle;
		int _threadCount;

		int _port;
		int _backLogSize;

		int _sessionCount;
		std::map<int, int> sessions;
	};

	const bool ConnectSendSocket(struct sockaddr_in& address, SOCKET& sock);
	const bool Send(SOCKET sock, const char* message);
	const void echo(const SOCKET& socket, const struct sockaddr_in& address);
}