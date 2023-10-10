
#include "pch.h"
#include "TCPIO.h"
#include "Session.h"

namespace TCPIO {
	const void TCPIO::ConsoleOut(const char* a)
	{
		std::cout << a;
	}

	const void TCPIO::ConsoleOut(const wchar_t* a)
	{
		//std::cout << a;
	}

	TCP::TCP(const int threadCount, const int port)
		: _port(port)
		, _backLogSize(SOMAXCONN)
		, _threadCount(threadCount)
	{
		_address.sin_addr.S_un.S_addr = INADDR_ANY;
		_address.sin_family = AF_INET;
		_address.sin_port = htons(port);
		_ioHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, _threadCount);
		_threadHandle = new HANDLE[threadCount];
	}

	const void TCP::InitReceive()
	{
		const char* opt = "";

		WORD wVersionRequested;
		WSADATA wsaData;
		wVersionRequested = MAKEWORD(2, 2);

		auto errorcode = WSAStartup(wVersionRequested, &wsaData);
		if (errorcode != 0)
		{
			return;
		}

		_serviceSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
			//socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (_serviceSocket == INVALID_SOCKET)
		{
			auto ec = WSAGetLastError();
			std::cout << ec << '\n';
			return;
		}

		if (setsockopt(_serviceSocket, SOL_SOCKET, SO_REUSEADDR, opt, sizeof(opt)))
		{
			auto ec = WSAGetLastError();
			std::cout << ec << '\n';
			return;
		}

		if (bind(_serviceSocket, (struct sockaddr*)&_address, sizeof(_address)))
		{
			auto ec = WSAGetLastError();
			std::cout << ec << '\n';
			return;
		}

		if (listen(_serviceSocket, _backLogSize) < 0)
		{
			auto ec = WSAGetLastError();
			std::cout << ec << '\n';
			return;
		}

		int ec = 0;
		int addLen = sizeof(sockaddr_in);

		_ioHandle = CreateIoCompletionPort((HANDLE)_serviceSocket, _ioHandle, 0, 0);

		LPFN_ACCEPTEX acceptEx = NULL;
		DWORD dwBytes = 0;
		GUID GuidAcceptEx = WSAID_ACCEPTEX;
		ec = WSAIoctl(_serviceSocket, SIO_GET_EXTENSION_FUNCTION_POINTER, &GuidAcceptEx, sizeof(GuidAcceptEx), &acceptEx, sizeof(acceptEx), &dwBytes, NULL, NULL);
		if (ec == SOCKET_ERROR)
		{
			std::cout << ec << '\n';
			return;
		}

		auto recvSocket = WSAAccept(_serviceSocket, (struct sockaddr*)&_address, &addLen, NULL, NULL);
		if (recvSocket == INVALID_SOCKET)
		{
			auto ec = WSAGetLastError();
			std::cout << ec << '\n';
			return;
		}

		auto session = new Session(recvSocket); 

		CreateIoCompletionPort((HANDLE)recvSocket, _ioHandle, 0, 0);

		/*OVERLAPPED overlapped = { 0 };
		overlapped.hEvent = WSACreateEvent();*/

		//while (0)
		//{
		//	DWORD recvbyte;
		//	DWORD flag = 0;
		//	auto result = WSARecv
		//	(recvSocket, &session->_receiveBuffer, 1, &recvbyte, &flag, &overlapped, NULL);
		//	if (result == SOCKET_ERROR)
		//	{
		//		ec = WSAGetLastError();

		//		if (WSA_IO_PENDING != ec) {
		//			std::cout << ec << '\n';
		//			return;
		//		}
		//	}

		//	int ec = 0;
		//	OVERLAPPED lpOverlapped;
		//	int compKey = 0;
		//	if (!GetQueuedCompletionStatus
		//	(_ioHandle, &recvbyte, (PULONG_PTR)&compKey, (LPOVERLAPPED*)&lpOverlapped, INFINITE))
		//	{
		//		ec = WSAGetLastError();
		//		std::cout << ec << '\n';
		//		if (ec == WSA_IO_PENDING)
		//			continue;
		//		return;
		//	}

		//	session->_receiveBuffer.len = recvbyte;
		//	std::cout << std::this_thread::get_id() << ':' << session->_receiveBuffer.buf << std::endl;

		//	session->_sendBuffer = session->_receiveBuffer;
		//	/*auto ec = recv(recvSocket, buf, sizeof(buf), 0);
		//	if (ec < 0)
		//	{
		//		auto ec = WSAGetLastError();
		//		std::cout << ec << '\n';
		//		return;
		//	}*/

		//	ec = WSASend(session->_socket, &session->_sendBuffer, 1, &session->_sendBuffer.len, flag, NULL, NULL);
		//	if (ec < 0)
		//	{
		//		auto ec = WSAGetLastError();
		//		std::cout << ec << '\n';
		//		return;
		//	}
		//}

		for (int i = 0; i < _threadCount; ++i)
		{
			std::thread thread(
				[&]() 
				{
					while (1)
					{
						DWORD recvbyte;
						DWORD flag = 0;
						auto result = WSARecv
						(recvSocket, &session->_receiveBuffer, 1, &recvbyte, &flag, session, NULL);
						if (result == SOCKET_ERROR)
						{
							ec = WSAGetLastError();

							if (WSA_IO_PENDING != ec) {
								std::cout << ec << '\n';
								return;
							}
						}

						int ec = 0;
						OVERLAPPED* lpOverlapped = NULL;
						int compKey = 0;
						if (!GetQueuedCompletionStatus
						(_ioHandle, &recvbyte, (PULONG_PTR)&compKey, (LPOVERLAPPED*)&lpOverlapped, INFINITE))
						{
							ec = WSAGetLastError();
							std::cout << ec << '\n';
							if (ec == WSA_IO_PENDING)
								continue;
							return;
						}
						Session &getSession = *reinterpret_cast<Session *>(lpOverlapped);

						getSession._receiveBuffer.len = recvbyte;
						std::cout << std::this_thread::get_id() << ':' << getSession._receiveBuffer.buf << std::endl;

						getSession._sendBuffer = getSession._receiveBuffer;
						/*auto ec = recv(recvSocket, buf, sizeof(buf), 0);
						if (ec < 0)
						{
							auto ec = WSAGetLastError();
							std::cout << ec << '\n';
							return;
						}*/

						ec = WSASend(getSession._socket, &getSession._sendBuffer, 1, &getSession._sendBuffer.len, flag, NULL, NULL);
						if (ec < 0)
						{
							auto ec = WSAGetLastError();
							std::cout << ec << '\n';
							return;
						}
					}
				});
			//_thread.emplace(i, thread);
			auto thread_id = thread.get_id();
			std::cout << thread_id << '\n';
			thread.detach();
		}

		while (1)
		{
			int ec = 0;
			int addLen = sizeof(sockaddr_in);
			auto recvSocket = WSAAccept(_serviceSocket, (struct sockaddr*)&_address, &addLen, NULL, NULL);
			if (recvSocket == INVALID_SOCKET)
			{
				auto ec = WSAGetLastError();
				std::cout << ec << '\n';
				return;
			}
		}

		//while (true)
		//{
		//	OVERLAPPED lpOverlapped;
		//	int compKey = 0;
		//	if (!GetQueuedCompletionStatus
		//	(_ioHandle, &recvbyte, (PULONG_PTR)&compKey, (LPOVERLAPPED*)&lpOverlapped, INFINITE))
		//	{
		//		ec = WSAGetLastError();
		//		std::cout << ec << '\n';
		//		if(ec == WSA_IO_PENDING)
		//			continue;
		//		return;
		//	}

		//	session->_receiveBuffer.len = recvbyte;
		//	std::cout << std::this_thread::get_id() << ':' << session->_receiveBuffer.buf << std::endl;

		//	session->_sendBuffer = session->_receiveBuffer;
		//	/*auto ec = recv(recvSocket, buf, sizeof(buf), 0);
		//	if (ec < 0)
		//	{
		//		auto ec = WSAGetLastError();
		//		std::cout << ec << '\n';
		//		return;
		//	}*/


		//	ec = WSASend(recvSocket, &session->_sendBuffer, 1, &session->_sendBuffer.len, flag, NULL, NULL);
		//	if (ec < 0)
		//	{
		//		auto ec = WSAGetLastError();
		//		std::cout << ec << '\n';
		//		return;
		//	}
		//}
	}

	const void TCPIO::echo(const SOCKET& socketID, const struct sockaddr_in& address)
	{
		int ec = 0;
		int addLen = sizeof(sockaddr_in);
		auto recvSocket = WSAAccept(socketID, (struct sockaddr*)&address, &addLen, NULL, NULL);
		if (recvSocket == INVALID_SOCKET)
		{
			auto ec = WSAGetLastError();
			std::cout << ec << '\n';
			return;
		}
		
		auto session = new Session(recvSocket);

		HANDLE handle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
		handle = CreateIoCompletionPort((HANDLE)recvSocket, handle, (DWORD) session, 0);

		WSAOVERLAPPED a = { 0 };
		a.hEvent = WSACreateEvent();

		DWORD recvbyte;
		DWORD flag = 0;
		auto result = WSARecv
		(recvSocket, &session->_receiveBuffer, 1, &recvbyte, &flag, &a, NULL);
		if (result == SOCKET_ERROR)
		{
			ec = WSAGetLastError();

			if (WSA_IO_PENDING != ec) {
				std::cout << ec << '\n';
				return;
			}
		}

		unsigned long long* lpCompletionKey = (unsigned long long *)1;
		OVERLAPPED lpOverlapped;

		if (!GetQueuedCompletionStatus
		(&a.hEvent, &recvbyte, (PULONG_PTR)&session, (LPOVERLAPPED*)&lpOverlapped, INFINITE))
		{
			ec = WSAGetLastError();
			std::cout << ec << '\n';
			return;
		}

		session->_receiveBuffer.len = recvbyte;
		std::cout << session->_receiveBuffer.buf << '\n';

		int index;
		WSANETWORKEVENTS ioEvent;
		char buf[1024] = {};
		while (true)
		{

			/*auto ec = recv(recvSocket, buf, sizeof(buf), 0);
			if (ec < 0)
			{
				auto ec = WSAGetLastError();
				std::cout << ec << '\n';
				return;
			}*/

			std::cout << std::this_thread::get_id() << ':' << buf << std::endl;

			ec = WSASend(recvSocket, &session->_sendBuffer, 1, &session->_sendBuffer.len, flag, NULL, NULL);
			if (ec < 0)
			{
				auto ec = WSAGetLastError();
				std::cout << ec << '\n';
				return;
			}
		}
	}

	const bool TCPIO::ConnectSendSocket(struct sockaddr_in& address, SOCKET& sock)
	{
		sock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
		//socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (sock == INVALID_SOCKET)
		{
			auto ec = WSAGetLastError();
			std::cout << ec << '\n';
			return false;
		}

		if (inet_pton(AF_INET, "127.0.0.1", &address.sin_addr) <= 0) {
			return false;
		}

		if (connect(sock, (struct sockaddr*)&address, sizeof(address)) != 0)
		{
			auto ec = WSAGetLastError();
			std::cout << ec << '\n';
			return false;
		}
		return true;
	}

	const bool TCPIO::Send(SOCKET sock, const char* message)
	{
		auto ec = send(sock, message, sizeof(message), 0);
		if (ec < 0)
		{
			auto ec = WSAGetLastError();
			std::cout << ec << '\n';
			return false;
		}

		char buf[1024];
		ec = recv(sock, buf, sizeof(buf), 0);
		if (ec < 0)
		{
			auto ec = WSAGetLastError();
			std::cout << ec << '\n';
			return false;
		}

		std::cout << buf << '\n';
		return true;
	}
}
