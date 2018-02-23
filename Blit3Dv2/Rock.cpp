#include "Rock.h"

extern Sprite* largeRockSprite;
extern Sprite* medRockSprite;
extern Sprite* smallRockSprite;

void Rock::Draw()
{
	sprite->angle = angle;
	sprite -> Blit(position.x, position.y,scale, scale);
	//wrap around
	//left
	if (position.x < radius + 10)
	{
		sprite->Blit(position.x + 1920, position.y, scale, scale);
		
	}
	//right
	if (position.x > 1920 - (radius + 10.f))
	{
		sprite->Blit(position.x - 1920.f, position.y, scale, scale);
	}
	//bottom
	if (position.y < radius + 10)
	{
		sprite->Blit(position.x, position.y + 1080, scale, scale);
	}
	//top
	if (position.y > 1080 - (radius + 10.f))
	{
		sprite->Blit(position.x, position.y - 1080, scale, scale);
	}
}

void Rock::Update(float seconds)
{
	position += velocity*seconds;
	angle += rotationSpeed;
	if (position.x < 0) position.x += 1920.f;
	if (position.x > 1920) position.x -= 1920.f;
	if (position.y < 0) position.y += 1080.f;
	if (position.y > 1080) position.y -= 1080.f;
}

void MakeRocks(std::vector<Rock> &RockList, int level, lua_State *Lua)
{
	
	RockList.clear();
	int numRocks = level + 2;
	float rockSpeed = 200.f + (20.f*level);

	for (int i = 0; i < numRocks; ++i)
	{
		Rock a;
		
		a.angle = a.rockRoll.RollFloat(0, 360);
		lua_getglobal(Lua, "AsteroidNamer");
		lua_call(Lua, 0, 1);
		a.name = lua_tostring(Lua, 1);
		lua_pop(Lua, 1);
		//place the rock NOT in the center
		float randomAngle = glm::radians(a.rockRoll.RollFloat(0, 360));
		float distance = a.rockRoll.RollFloat(200, 440);
		a.position.x = 1920/2 + cos(randomAngle) * distance;
		a.position.y = 1080/2 + sin(randomAngle) * distance;
		a.radius = 50.f * a.scale;
		a.sprite = largeRockSprite;
		a.rotationSpeed = a.rockRoll.RollFloat(-2, 2);
		//randomize each asteroid's speed
		randomAngle = glm::radians(a.rockRoll.RollFloat(0, 360));
		a.velocity.x = cos(randomAngle) * rockSpeed;
		a.velocity.y = sin(randomAngle) * rockSpeed;
		RockList.push_back(a);
		
	}
}