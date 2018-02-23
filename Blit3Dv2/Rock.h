#pragma once
#include <Blit3D.h>
#include "DieRoller.h"
#include <lua.hpp>
enum  class AsteroidEnum
{
SMALL=1, MEDIUM, LARGE
};

class Rock
{
public:
	Sprite *sprite = NULL;
	std::string name;
	glm::vec2 velocity;
	glm::vec2 position;
	AsteroidEnum size = AsteroidEnum::LARGE;
	int health = 2 * static_cast<int>(size); 
	float angle;  //in degrees
	float radius;
	float rotationSpeed;
	void Draw();
	float scale = 1;
	void Update(float seconds);
	DieRoller rockRoll;
};

void MakeRocks(std::vector<Rock> &RockList, int level, lua_State *L);
