#include "../pch.h"
#include "../Map/Map.h"
#include "Object.h"

namespace Object
{
	Player::Player(ObjectType type)
	{
		_posX = 0;
		_posY = 0;
		_type = type;
	}
}