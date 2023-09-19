#pragma once

namespace TCPIO {
	const void ConsoleOut(const char* a);
	const void ConsoleOut(const wchar_t* a);

	class TCP
	{
	public:
		TCP();
		virtual ~TCP() = default;
		const void Init();

	private:
		int port;
		int backLogSize;

		std::map<int, int> sessions_;
	};
}