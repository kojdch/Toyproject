#include "../pch.h"
#include "../Object/Object.h"
#include "Action.h"
#include "Map.h"

namespace Map
{
	Map::Map()
	{
		_uid.store(0);
		Clear();
	}

	void Map::Clear()
	{
		_objects.clear();
		_players.clear();
		_nPlayers.clear();
	}

	void Map::Update()
	{

	}

	void Map::AddObject(const Object::MapObject object)
	{
		_objects.emplace(_uid, object);
		if(object._type == Object::ObjectType::player)
			_players.emplace(_uid, object);
		else
			_nPlayers.emplace(_uid, object);
		_uid.store(_uid + 1);
	}
}