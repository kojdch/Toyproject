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
		const int PostRecv(Session& session);

	private:
		SOCKET _serviceSocket;
		struct sockaddr_in _address;
		HANDLE _ioHandle;
		LPFN_ACCEPTEX _acceptEx;

		std::map<int, std::thread> _thread;
		HANDLE* _threadHandle;
		int _threadCount;

		int _port;
		int _backLogSize;

		Session _session;
		int _lastSessionID;
		std::map<int, Session> _sessions;
	};

	const bool ConnectSendSocket(struct sockaddr_in& address, SOCKET& sock);
	const bool Send(SOCKET sock, const char* message);
}