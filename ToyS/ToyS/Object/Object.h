#pragma once

namespace Object 
{
	enum class ObjectType : byte {
		none,
		player,
		nplayer,
	};

	class MapObject
	{
	public:
		int _posX;
		int _posY;
		ObjectType _type;
	};

	class Player : MapObject
	{
	public:
		Player(ObjectType type);
		~Player() = default;

	private:
		
	};

	using PlayerPtr = Player*;
}