#pragma once
#include <Blit3D.h>
#include "Rock.h"


class Shot
{
public:

	glm::vec2 velocity, position;
	Sprite *sprite;
	void Draw();
	bool Update(float seconds); //return false if shot is dead
	float lifeTime = 2.f;
	float size = 0.3;
};

class Ship
{
public:

	Sprite *shotSprite;
	std::vector<Sprite*> spriteList;
	std::vector<Sprite*> bubbleList;
	glm::vec2 velocity;
	glm::vec2 position;
	glm::vec2 facing;

	float invincibleTime = 0;
	float size = 0.3;
	float angle = 0.f;					//degrees
	float shotTimer = 0;
	float radius = 127.f * size;
	float thrustTimer = 0.f;

	bool shooting = false;
	bool thrusting = false;
	bool clockwiseTurn = false;			//right turn
	bool couterclockwiseTurn = false;	//left turn
	bool invincible = true;
	
	int health = 30;
	int lives = 3;
	int frameNumber = 0;
	int score = 0;

	void Draw();
	void Update(float seconds);

	bool Shoot(std::vector<Shot> &shotList);

};

bool Collide(const Rock &rock, const Ship &ship);

bool Collide(const Rock &rock, const Shot &shot);