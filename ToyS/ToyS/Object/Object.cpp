#include "../pch.h"
#include "Object.h"

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
}