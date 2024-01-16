#pragma once
#include "Session.h"

namespace TCPIO {
	const void ConsoleOut(const char* a);

	class Session;

	class TCP
	{
	public:

		TCP(const int threadCount, const int port);
		virtual ~TCP() = default;
		const void InitReceive();

		const void PostAccept();
		//const int PostRecv(std::shared_ptr<Session> session);
		const int PostRecv(Session session);

	private:
		SOCKET _serviceSocket;
		struct sockaddr_in _address;
		HANDLE _ioHandle;
		LPFN_ACCEPTEX _acceptEx;

		std::map<int, std::shared_ptr<std::thread>> _thread;
		HANDLE* _threadHandle;
		int _threadCount;

		int _port;
		int _backLogSize;

		std::shared_ptr<Session> _session;
		int _lastSessionID;
		std::map<int, std::shared_ptr<Session>> _sessions;
	};

	const bool ConnectSendSocket(struct sockaddr_in& address, SOCKET& sock);
	const bool Send(SOCKET sock, const char* message);
}