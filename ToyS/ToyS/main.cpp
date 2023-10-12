
#include "pch.h"
#include <Session\TCPIO.h>

int main(int _argc, wchar_t* _argv[])
{
	wchar_t title[256] = L"ToyS";
	SetConsoleTitle(title);

	WORD wVersionRequested;
	WSADATA wsaData;
	wVersionRequested = MAKEWORD(2, 2);

	auto errorcode = WSAStartup(wVersionRequested, &wsaData);
	if (errorcode != 0)
	{
		return 0;
	}

	TCPIO::TCP tcp(2, 12345);
	tcp.InitReceive();

	while (true)
	{
		TCPIO::ConsoleOut("Loop\n");
		std::this_thread::sleep_for(std::chrono::seconds(10));
	}

	return 0;
}