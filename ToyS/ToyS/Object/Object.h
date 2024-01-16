#pragma once

namespace Object 
{
	enum class objectType : byte {
		none,
		player,
		nplayer,
	};

	interface MapObject
	{
		int posX;
		int posY;
	};

	class Object : MapObject
	{
	public:
		Object();
		~Object() = default;

	private:
		
	};
}