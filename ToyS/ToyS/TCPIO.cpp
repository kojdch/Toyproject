
#include "pch.h"
#include "TCPIO.h"


namespace TCPIO {
	const void TCPIO::ConsoleOut(const char* a)
	{
		std::cout << a;
	}

	const void TCPIO::ConsoleOut(const wchar_t* a)
	{
		//std::cout << a;
	}

	TCP::TCP()
		: port(12345)
		, backLogSize(SOMAXCONN)
	{
	}

	const void TCP::Init()
	{
		struct sockaddr_in address;

		address.sin_addr.S_un.S_addr = INADDR_ANY;
		address.sin_family = AF_INET;
		address.sin_port = htons(port);
		
		const char* opt = "";
		char buf[1024] = {};

		auto socketID = socket(AF_INET, SOCK_STREAM, 0);
		if (socketID == 0)
		{
			return;
		}

		if (setsockopt(socketID, SOL_SOCKET, SO_REUSEADDR, opt, sizeof(opt)))
		{
			return;
		}

		if (bind(socketID, (struct sockaddr*)&address, sizeof(address)))
		{
			return;
		}

		if (listen(socketID, backLogSize) < 0)
		{
			return;
		}

		auto socket = accept(socketID, (struct sockaddr*)&address, NULL);
		if (socket == INVALID_SOCKET)
		{
			auto ec = WSAGetLastError();
			return;
		}

		auto ec = recv(socket, buf, sizeof(buf), 0);
		if (ec != 0)
		{
			return;
		}

		std::cout << buf << std::endl;

		ec = send(socket, buf, sizeof(buf), 0);
		if (ec != 0)
		{
			return;
		}

		/*acceptor_ = shared_ptr<tcp::acceptor>(new tcp::acceptor(context_, tcp::endpoint(tcp::v6(), (unsigned short)port)));
		error_code ec;
		acceptor_->set_option(ip::v6_only(false), ec);

		thread_group threads;

		int count = 32;

		for (int i = 0; i < count; ++i)
		{
			threads.create_thread(boost::bind(&io_context::run, &context_));
		}*/

	}
}
