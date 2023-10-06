
#include "pch.h"
#include <Session\TCPIO.h>

int main(int _argc, wchar_t* _argv[])
{
	wchar_t title[256] = L"ToyS";
	SetConsoleTitle(title);

	TCPIO::TCP tcp(1, 12345);
	tcp.InitReceive();

	while (true)
	{
		TCPIO::ConsoleOut("Loop\n");
		std::this_thread::sleep_for(std::chrono::seconds(10));
	}

	return 0;
}