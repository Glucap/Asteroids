#include "Ship.h"
#include <math.h>


void Ship::Draw()
{
	spriteList[frameNumber]->angle = angle;
	spriteList[frameNumber]->Blit(position.x, position.y, size, size);

	//Redraw if too close to the edge
	//left
	if (position.x < radius + 10)
		spriteList[frameNumber]->Blit(position.x + 1920, position.y, size, size);
	//right
	if (position.x > 1920 - (radius + 10.f))
		spriteList[frameNumber]->Blit(position.x - 1920.f, position.y, size, size);
	//bottom
	if (position.y < radius + 10)
		spriteList[frameNumber]->Blit(position.x, position.y + 1080, size, size);
	//top
	if (position.y > 1080 - (radius + 10.f))
		spriteList[frameNumber]->Blit(position.x, position.y - 1080, size, size);
	spriteList[frameNumber]->Blit(position.x + 1920, position.y + 1080, size, size);
	spriteList[frameNumber]->Blit(position.x - 1920, position.y + 1080, size, size);
	spriteList[frameNumber]->Blit(position.x + 1920, position.y - 1080, size, size);
	spriteList[frameNumber]->Blit(position.x - 1920, position.y - 1080, size, size);
}
void Ship::Update(float seconds)
{

	if (clockwiseTurn)
	{
		angle -= 180.f * seconds;
	}

	if (couterclockwiseTurn)
	{
		angle += 180.f * seconds;
	}

	if (thrusting)
	{
		//calculate facing vector
		float radians = (angle + 90) * (M_PI / 180);
		facing.x = std::cos(radians);
		facing.y = std::sin(radians);

		facing *= seconds * 200.f;

		velocity += facing;

		//check if over max speed

		if (velocity.length() > 500.f)
			velocity = glm::normalize(velocity) * 500.f;

		//animation timing

		if (thrustTimer < 1.f / 10.f)
			thrustTimer += seconds;

		else
		{
			frameNumber += 1;


			if (frameNumber > 2)
			{
				frameNumber = 1;
				thrustTimer -= 1.f / 10.f;
			}
		}
	}
	else frameNumber = 0;

	float scale = 1 - seconds * 0.5f;

	if (scale < 0) scale = 0;
	velocity *= scale;

	position += velocity * seconds;

	if (position.x < 0) position.x += 1920.f;
	if (position.x > 1920) position.x -= 1920.f;
	if (position.y < 0) position.y += 1080.f;
	if (position.y > 1080) position.y -= 1080.f;

	if (shotTimer > 0)
		shotTimer -= seconds;

}


bool Ship::Shoot(std::vector<Shot> &shotList)
{
	if (shotTimer > 0)
		return false;

	shotTimer = 1.f / 10.f;
	Shot shot;
	shot.sprite = shotSprite;
	glm::vec2 offset;
	offset.x = cos(glm::radians(angle + 90 + (68 * size)));
	offset.y = sin(glm::radians(angle + 90 + (68 * size)));
	shot.position = position + (offset * (93*size));
	shot.velocity.x = cos(glm::radians(angle + 90));
	shot.velocity.y = sin(glm::radians(angle + 90));
	shot.velocity *= 400.f;
	shot.velocity += velocity;
	shotList.push_back(shot);
	
	Shot shot2;
	shot2.sprite = shotSprite;
	offset.x = cos(glm::radians(angle + 90 - (67 * size)));
	offset.y = sin(glm::radians(angle + 90 - (67 * size)));
	shot2.position = position + (offset * (93 * size));
	shot2.velocity.x = cos(glm::radians(angle + 90));
	shot2.velocity.y = sin(glm::radians(angle + 90));
	shot2.velocity *= 400.f;
	shot2.velocity += velocity;
	shotList.push_back(shot2);
	
	return true;
}

void Shot::Draw()
{
	sprite->Blit(position.x, position.y, size, size);

}

bool Shot::Update(float seconds)
{	
	if (position.x < 0) position.x += 1920.f;
	if (position.x > 1920) position.x -= 1920.f;
	if (position.y < 0) position.y += 1080.f;
	if (position.y > 1080) position.y -= 1080.f;
	position += velocity* seconds;
	if (lifeTime > 0)
		lifeTime -= seconds;
	if (lifeTime > 0)
		return true;
	return false;


}
bool Collide(const Rock &rock, const Ship &ship)
{	
	float distance = glm::distance(rock.position, ship.position);
	glm::vec2 wrapPos;
	for (float xadd = -1920; xadd <= 1920; xadd += 1920)
	{
		for (float yadd = -1080; yadd <= 1080; yadd += 1080)
		{
			wrapPos = rock.position;
			wrapPos.x += xadd;
			wrapPos.y += yadd;
			if (distance <= rock.radius + ship.radius)
			{
				return true;
			}
		}
	}
	return false;	
}

bool Collide(const Rock &rock, const Shot &shot)
{
	float distance = glm::distance(rock.position, shot.position);
	glm::vec2 wrapPos;
	for (float xadd = -1920; xadd <= 1920; xadd += 1920)
	{
		for (float yadd = -1080; yadd <= 1080; yadd += 1080)
		{
			wrapPos = rock.position;
			wrapPos.x += xadd;
			wrapPos.y += yadd;
			if (distance <= rock.radius) return true;
		}
	}
	return false;
}



