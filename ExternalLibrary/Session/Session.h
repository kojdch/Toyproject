#pragma once

namespace TCPIO {

	enum class Type : byte
	{
		Recv,
		Send,
		Accept,
		Disconnect,
	};

	class Context : public OVERLAPPED
	{
	public:
		Context(SOCKET socket, Type type, int uid);
		~Context() = default;

		WSABUF _receiveBuffer;
		WSABUF _sendBuffer;
		SOCKET _socket;

		int _uid;
		Type _type;

		char receivebuf[256];
		char sendbuf[256];
	};

	class Session
	{
	public:
		Session(Context context);
		Session(SOCKET socket, int uid);
		~Session() = default;

		SOCKET _socket;

		Context _context;

		int _uid;
		Type _type;

	public:
		template<typename Ptr>
		void SetObjectPtr(Ptr ptr)
		{
			objectPtr = reinterpret_cast<void*>(ptr);
		}

		template<typename Ptr>
		Ptr GetObjectPtr()
		{
			return reinterpret_cast<Ptr>(objectPtr);
		}
	private:
		//SendBuffer _sendBuffer;

		std::atomic<void*> objectPtr = nullptr;
	};
}