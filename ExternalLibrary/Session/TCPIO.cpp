
#include "pch.h"
#include "Session.h"
#include "TCPIO.h"

namespace TCPIO {
	const void TCPIO::ConsoleOut(const char* a)
	{
		std::cout << a;
	}

	TCP::TCP(const int threadCount, const int port)
		: _port(port)
		, _backLogSize(SOMAXCONN)
		, _threadCount(threadCount)
		, _lastSessionID(0)
		, _acceptEx(NULL)

	{
		_address.sin_addr.S_un.S_addr = INADDR_ANY;
		_address.sin_family = AF_INET;
		_address.sin_port = htons(port);
		_ioHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, _threadCount);
		_threadHandle = new HANDLE[threadCount];

		_serviceSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
		if (_serviceSocket == INVALID_SOCKET)
		{
			auto ec = WSAGetLastError();
			std::cout << ec << '\n';
			return;
		}

		SOCKET sock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
		if (sock == INVALID_SOCKET)
		{
			auto ec = WSAGetLastError();
			std::cout << ec << '\n';
			return;
		}
		_session = Session(sock);
	}

	const void TCP::InitReceive()
	{
		const char* opt = "";

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

		DWORD dwBytes = 0;
		GUID GuidAcceptEx = WSAID_ACCEPTEX;
		ec = WSAIoctl(_serviceSocket, SIO_GET_EXTENSION_FUNCTION_POINTER, &GuidAcceptEx, sizeof(GuidAcceptEx), &_acceptEx, sizeof(_acceptEx), &dwBytes, NULL, NULL);
		if (ec == SOCKET_ERROR)
		{
			std::cout << ec << '\n';
			return;
		}

		for (int i = 0; i < _threadCount; ++i)
		{
			auto thread = std::make_shared<std::thread>(std::thread(
				[&]() 
				{
					while (1)
					{
						void* key = NULL;
						OVERLAPPED* overlapped = NULL;
						DWORD recvbyte = 0;

						BOOL completionStatus = GetQueuedCompletionStatus(
							_ioHandle,
							&recvbyte,
							(PULONG_PTR)&key,
							&overlapped,
							INFINITE);

						if (FALSE == completionStatus)
						{
							if (overlapped != NULL)
							{
								Session getSession = *reinterpret_cast<Session*>(overlapped);
								continue;
							}

							ec = WSAGetLastError();
							if(ec == WAIT_TIMEOUT)
								continue;

						}

						Session getSession = *reinterpret_cast<Session*>(overlapped);

						if (recvbyte == 0)
						{
							CreateIoCompletionPort((HANDLE)getSession._socket, _ioHandle, 0, 0);

							SOCKET sock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
							if (sock == INVALID_SOCKET)
							{
								auto ec = WSAGetLastError();
								std::cout << ec << '\n';
								return;
							}
							_session = Session(sock);
							PostAccept();
							_lastSessionID++;
							_sessions.emplace(_lastSessionID, getSession);
						}
						else
						{
							DWORD flag = 0;
							getSession._receiveBuffer.len = recvbyte;
							std::cout << std::this_thread::get_id() << ':' << getSession._receiveBuffer.buf << std::endl;
							getSession._sendBuffer = getSession._receiveBuffer;
							ec = WSASend(getSession._socket, &getSession._sendBuffer, 1, &getSession._sendBuffer.len, flag, NULL, NULL);
							if (ec < 0)
							{
								auto ec = WSAGetLastError();
								std::cout << ec << '\n';
								return;
							}
						}

						PostRecv(getSession);
					}
				})); 
			_thread.emplace(i, thread);
			thread->detach();
		}

		while (0)
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

		PostAccept();

	}

	const void TCP::PostAccept()
	{
		DWORD recvSize = 0;
		DWORD addressSize = sizeof(sockaddr_in) + 16;
		void* key = NULL;
		int ec = 0;

		if (_acceptEx(
			_serviceSocket,
			_session._socket,
			&_session.receivebuf,
			0,
			addressSize,
			addressSize,
			&recvSize,
			&_session) == FALSE)
		{
			ec = GetLastError();
			if (ec != ERROR_IO_PENDING)
			{
				std::cout << "TCP::PostAccept()" << ec << '\n';
			}
		}
		else
		{
			PostQueuedCompletionStatus(
				_ioHandle,
				0,
				(ULONG_PTR)&key,
				&_session);
		}
	}
	const int TCP::PostRecv(Session& session)
	{
		DWORD bytes = 0;
		DWORD flags = 0;
		int ec = 0;
		auto result = WSARecv
		(session._socket, &session._receiveBuffer, 1, &bytes, &flags, &session, NULL);
		if (result == SOCKET_ERROR)
		{
			ec = WSAGetLastError();

			if (WSA_IO_PENDING != ec) {
				std::cout << ec << '\n';
				return ec;
			}
		}
		return ec;
	}

	const bool TCPIO::ConnectSendSocket(struct sockaddr_in& address, SOCKET& sock)
	{
		sock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
		if (sock == INVALID_SOCKET)
		{
			auto ec = WSAGetLastError();
			std::cout << "socket" << ec << '\n';
			return false;
		}

		if (inet_pton(AF_INET, "127.0.0.1", &address.sin_addr) <= 0) {
			return false;
		}

		if (connect(sock, (struct sockaddr*)&address, sizeof(address)) != 0)
		{
			auto ec = WSAGetLastError();
			std::cout << "connect" << ec << '\n';
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
			std::cout << "send" << ec << '\n';
			return false;
		}

		char buf[1024];
		ec = recv(sock, buf, sizeof(buf), 0);
		if (ec < 0)
		{
			auto ec = WSAGetLastError();
			std::cout << "recv" << ec << '\n';
			return false;
		}

		std::cout << buf << '\n';
		return true;
	}
}
