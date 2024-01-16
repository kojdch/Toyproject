#pragma once
#include "../Object/Object.h"
#include "Action.h"

namespace Map
{

	class Map
	{
	public:
		Map();
		~Map() = default;

		void Clear();
		void Update();

		void AddObject(const Object::MapObject object);

	private:
		std::queue<int> _queue;

		std::map<int, Object::MapObject> _objects;
		std::map<int, Object::MapObject> _players;
		std::map<int, Object::MapObject> _nPlayers;

		std::atomic<int> _uid;
	};
}