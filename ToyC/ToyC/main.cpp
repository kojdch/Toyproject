
#include <Session\pch.h>
#include <Session\TCPIO.h>

int main(int _argc, wchar_t* _argv[])
{
	wchar_t title[256] = L"ToyC";
	SetConsoleTitle(title);

	struct sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_port = htons(12345);
	std::string message;

	WORD wVersionRequested;
	WSADATA wsaData;
	wVersionRequested = MAKEWORD(2, 2);

	auto errorcode = WSAStartup(wVersionRequested, &wsaData);
	if (errorcode != 0)
	{
		return 0;
	}

	SOCKET sock;
	if (!TCPIO::ConnectSendSocket(address, sock))
	{
		return 0;
	}

	while (true)
	{
		std::cin >> message;
		TCPIO::Send(sock, message.c_str());
	}

	return 0;
}