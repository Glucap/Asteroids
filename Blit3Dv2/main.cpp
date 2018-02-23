/*
	Simple example of loading/rotating/displaying sprites in Blit3D
*/
#include "Blit3D.h"
Blit3D *blit3D = NULL;
#include "MarkovChain.h";
//memory leak detection
#define CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include "AudioEngine.h"
#include "Ship.h"
#include "Rock.h"
#include <lua.hpp>
//GLOBAL DATA

AudioEngine * audioE = NULL;
AkGameObjectID mainGameID = 1;
enum GameStateEnum { PLAY, GAMEOVER, PAUSE };
Sprite* shieldBar = NULL;
Sprite* lifeSprite = NULL;
Sprite* largeRockSprite = NULL;
Sprite* medRockSprite = NULL;
Sprite* smallRockSprite = NULL;
Sprite* backgroundSprite = NULL; //a pointer to a background sprite
Sprite* bubbleSprite = NULL;
Sprite* gameOverSprite = NULL;
Ship *ship = NULL;
lua_State *L;



double timeSlice = 1.f / 120.f;
double elapsedTime;

bool newLevel = true;
double newGameTime = 0;
int level = 0;

std::vector<Shot> shotList;
std::vector<Rock> rockList;

GameStateEnum gameState = PLAY;

AngelcodeFont *debugFont = NULL;
std::mt19937 rng;
bool ErrorCheck(int errorNum)
{
	if (errorNum)
	{
		//There was an error!
		std::cout << "ERROR!\n";
		std::cout << "There was an error calling Lua.\n";
		switch (errorNum)
		{
		case LUA_ERRRUN:
			std::cout << "There was a runtime error.\n";
			break;

		case LUA_ERRMEM:
			std::cout << "There was a memory allocation error (this should be unusual!).\n";
			break;

		case LUA_ERRERR:
			std::cout << "There was a error-handler error (this should be unusual!).\n";
			break;

		case LUA_ERRSYNTAX:
			std::cout << "There was a syntax error.\n";
			break;

		default:
			std::cout << "There was an unknown error.\n";
		}

		std::cout << "Error string from Lua:\n";

		//get error string off the stack
		std::string errorString = lua_tostring(L, -1);
		std::cout << errorString << "\n";

		return false;
	}

	return true;
}
/* function to be called from a Lua script,
rolls a random integer between 1 and range*/
int MakeName(lua_State* L)
{
	MarkovChain M;
	std::string filename;
	filename.append(lua_tostring(L, -2));
	int nameLenght = lua_tonumber(L, -1);

	M.ParseFile(filename);

	std::string name = M.GenerateChain(nameLenght);

	lua_pushstring(L, name.c_str());
	//we pushed one item on the stack, so return 1
	return 1;
}

int RandomRange(lua_State* L)
{
	//get bottom item off the stack,
	//because we want the one argument Lua was
	//passing us (range)
	int low = lua_tonumber(L, -2);
	int high = lua_tonumber(L, -1);
	std::uniform_int_distribution<int> distribution(low, high);

	int num = distribution(rng);

	//put the random number onto the stack
	lua_pushnumber(L, num);

	//we pushed one item on the stack, so return 1
	return 1;
}

void Init()
{
	
	L = lua_open();
	//load Lua base libraries
	luaL_openlibs(L);

	//register our C++ function with Lua
	lua_register(L, "RandomRange", RandomRange);
	lua_register(L, "MakeName", MakeName);
	//load the script...this loads the Add2Random() function, but
	//doesn't execute it yet.

	if (!ErrorCheck(luaL_dofile(L, "AsteroidNamer.lua")))
	{
		//DO NOT CLEAR THE ERROR STRING OFF THE STACK
		//AFTER luaL_dofile() OR AN EXCEPTION GETS THROWN

		std::cout << "ERROR processing AsteroidNamer.lua\n\n";
	}


	audioE = new AudioEngine;
	audioE->Init();
	audioE->SetBasePath("Media\\");

	//load banks
	audioE->LoadBank("Init.bnk");
	audioE->LoadBank("MainBank.bnk");

	//register game objects
	audioE->RegisterGameObject(mainGameID);

	backgroundSprite = blit3D->MakeSprite(0, 0, 1920, 1080, "Media\\bg5.jpg");

	ship = new Ship;
	ship->spriteList.push_back(blit3D->MakeSprite(0, 0, 372, 372, "Media\\Skorpio-ship.png"));
	ship->spriteList.push_back(blit3D->MakeSprite(0, 0, 372, 372, "Media\\Skorpio-shipthrust1.png"));
	ship->spriteList.push_back(blit3D->MakeSprite(0, 0, 372, 372, "Media\\Skorpio-shipthrust2.png"));
	ship->spriteList.push_back(blit3D->MakeSprite(0, 0, 372, 372, "Media\\Skorpio-shipthrust1shot1.png"));
	ship->spriteList.push_back(blit3D->MakeSprite(0, 0, 372, 372, "Media\\Skorpio-shipthrust2shot1.png"));
	ship->shotSprite = blit3D->MakeSprite(0, 0, 30, 30, "Media\\Shot.png");
	ship->position = glm::vec2(1920.f / 2, 1080.f / 2);
	
	bubbleSprite = blit3D->MakeSprite(0, 156.45, 143, 137.5, "Media\\Spritesheet\\sheet.png");

	shieldBar = blit3D->MakeSprite(0, 78, 222, 39, "Media\\Spritesheet\\sheet.png");
	lifeSprite = blit3D->MakeSprite(0, 0, 33, 26, "Media\\PNG\\UI\\playerlife1_blue.png");

	largeRockSprite = blit3D->MakeSprite(327,452,98,96, "Media\\Spritesheet\\sheet.png");
	medRockSprite = blit3D->MakeSprite(237, 453, 45, 39, "Media\\Spritesheet\\sheet.png");
	smallRockSprite = blit3D->MakeSprite(406, 235, 27, 26, "Media\\Spritesheet\\sheet.png");
	gameOverSprite = blit3D->MakeSprite(0, 0, 632, 153, "Media\\gameover.png");
	debugFont = blit3D->MakeAngelcodeFontFromBinary32("debugfont30.bin");

	audioE->PlayEvent("BGMusic", mainGameID);
}

void DeInit(void)
{

	if (audioE != NULL) delete audioE;

	if (ship != NULL) delete ship;
	
	if (debugFont != NULL) delete debugFont;
}

void Update(double seconds)
{
	
	audioE->ProcessAudio();

	if (seconds < 0.15)
		elapsedTime += seconds;
	else
		elapsedTime += 0.15;

		while (gameState != GAMEOVER && elapsedTime >= timeSlice)
		{
			elapsedTime -= timeSlice;
			ship->Update(timeSlice);
			if (ship->shooting)
			{
				ship->Shoot(shotList);
				audioE->PlayEvent("Shoot", mainGameID);
			}
			for (auto &R : rockList) R.Update(timeSlice);


			for (int i = shotList.size() - 1; i >= 0; --i)
			{
				if (!shotList[i].Update(timeSlice))
					shotList.erase(shotList.begin() + i);
			}

			for (int i = rockList.size() - 1; i >= 0; --i)
			{
				for (int j = shotList.size() - 1; j >= 0; --j)
				{
					if (Collide(rockList[i], shotList[j]))
					{
						shotList.erase(shotList.begin() + j);
						audioE->PlayEvent("explosion", mainGameID);
						if (rockList[i].health <= 0)
						{
							audioE->PlayEvent("rockexplode", mainGameID);
							switch (rockList[i].size)
							{
							case AsteroidEnum::LARGE:
							{
								Rock a;
								lua_getglobal(L, "AsteroidNamer");
								lua_call(L, 0, 1);
								a.name = lua_tostring(L, 1);
								lua_pop(L, 1);
								a.angle = a.rockRoll.RollFloat(0, 360);
								a.position = rockList[i].position;
								a.rotationSpeed = a.rockRoll.RollFloat(-10, 10);
								float speed = glm::length(rockList[i].velocity);
								speed *= 1.3;
								float movementAngle = atan2f(rockList[i].velocity.y, rockList[i].velocity.x);
								//randomize each asteroid's speed
								a.velocity.x = cos(movementAngle + a.rockRoll.RollFloat(-1, 1))*speed;
								a.velocity.y = sin(movementAngle + a.rockRoll.RollFloat(-1, 1))*speed;
								a.size = rockList[i].size = AsteroidEnum::MEDIUM;
								rockList[i].velocity *= 1.3;
								a.sprite = rockList[i].sprite = medRockSprite;
								a.radius = rockList[i].radius = 20 * a.scale;
								a.health = rockList[i].health = 2 * static_cast<int>(rockList[i].size);
								rockList.push_back(a);
								ship->score += 50 * level;
								break;
							}
							case AsteroidEnum::MEDIUM:
							{
								Rock a;
								Rock b;

								float speed = glm::length(rockList[i].velocity);
								speed *= 1.3;
								float movementAngle = atan2f(rockList[i].velocity.y, rockList[i].velocity.x);
								rockList[i].velocity *= 1.3;
					
								lua_getglobal(L, "AsteroidNamer");
								lua_call(L, 0, 1);
								b.name = lua_tostring(L, 1);
								lua_pop(L, 1);
								b.angle = b.rockRoll.RollFloat(0, 360);
								b.position = rockList[i].position;
								b.rotationSpeed = b.rockRoll.RollFloat(-2, 2);
								b.velocity.x = cos(movementAngle + b.rockRoll.RollFloat(-1, 1))*speed;
								b.velocity.y = sin(movementAngle + b.rockRoll.RollFloat(-1, 1))*speed;

								lua_getglobal(L, "AsteroidNamer");
								lua_call(L, 0, 1);
								a.name = lua_tostring(L, 1);
								lua_pop(L, 1);
								a.angle = a.rockRoll.RollFloat(0, 360);
								a.position = rockList[i].position;
								a.rotationSpeed = a.rockRoll.RollFloat(-2, 2);
								a.velocity.x = cos(movementAngle + a.rockRoll.RollFloat(-1, 1))*speed;
								a.velocity.y = sin(movementAngle + a.rockRoll.RollFloat(-1, 1))*speed;
								//randomize each asteroid's speed

								b.size = a.size = rockList[i].size = AsteroidEnum::SMALL;

								b.sprite = a.sprite = rockList[i].sprite = smallRockSprite;
								b.radius = a.radius = rockList[i].radius = 13 * a.scale;
								b.health = a.health = rockList[i].health = 2 * static_cast<int>(rockList[i].size);
								rockList.push_back(a);
								rockList.push_back(b);
								ship->score += 100 * level;
								break;
							}
							case AsteroidEnum::SMALL:
								rockList.erase(rockList.begin() + i);
								ship->score += 200 * level;
								break;
							}
							break;
						}
						else
						{
							rockList[i].health -= 1;
							break;
						}
					}

				}
			}
			if (!ship->invincible)
			{
				for (auto R : rockList)
					if (Collide(R, *ship))
					{

						ship->health -= (5 * (int)R.size);
						if (ship->health < 1)
						{
							audioE->PlayEvent("death", mainGameID);
							ship->lives--;
							ship->health = 30;
							ship->position.x = blit3D->screenWidth / 2;
							ship->position.y = blit3D->screenHeight / 2;
							ship->velocity.y = 0;
							ship->velocity.x = 0;
						}
						else
							audioE->PlayEvent("shield", mainGameID);
						ship->invincible = true;
					}
			}
			if (ship->invincible)
			{

				if (ship->invincibleTime > 3)
				{
					ship->invincible = false;
					ship->invincibleTime = 0;
				}

				else
					ship->invincibleTime += elapsedTime;
			}
			if (rockList.size() <= 0)
			{
				level++;
				ship->invincible = true;
				newLevel = true;
				ship->position.y = 1080 / 2;
				ship->position.x = 1920 / 2;
				ship->velocity.y = 0;
				ship->velocity.x = 0;
				MakeRocks(rockList, level, L);

			}
			if (ship->lives < 1)
				gameState = GAMEOVER;
		}

	}

		

				
		


	void Draw(void)
{
	glClearColor(1.0f, 0.0f, 1.0f, 0.0f);	//clear colour: r,g,b,a 	
	// wipe the drawing surface clear
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//draw stuff here
	backgroundSprite->Blit(1920.f / 2, 1080.f / 2);
	//draw the background in the middle of the screen
	//the arguments to Blit(0 are the x, y pixel coords to draw the center of the sprite at, 
	//starting as 0,0 in the bottom-left corner.
	if (gameState == PLAY)
	{

		for (auto R : rockList)
		{
			R.Draw();
			debugFont->BlitText(R.position.x - (R.name.length()/2 * 20), R.position.y + ((int)R.size * 5), R.name);
		}
		for (auto S : shotList) S.Draw();
		ship->Draw();

		//debug health
		std::string lives = "LIVES";
		std::string Shield = "Shield ";
		std::string Score = "SCORE : ";
		std::string String_level = "LEVEL : ";

		for (int i = 0; i < ship->lives; i++) {
			lifeSprite->Blit(210 + (i * 36), 1013);
		}
		String_level += std::to_string(level);
		Score += std::to_string(ship->score);
		if (ship->invincible)
		{
			bubbleSprite->angle = ship->angle;
			bubbleSprite->Blit(ship->position.x, ship->position.y);
		}
		shieldBar->Blit((100 + ((ship->health) / 30.f) / 2.f * 222.f), 950, (ship->health) / 30.f, 1);
		debugFont->BlitText(100, 1080 - 50, lives);
		debugFont->BlitText(100, 135, Score);
		debugFont->BlitText(100, 100, String_level);
	}
	if (gameState == GAMEOVER)
	{
		gameOverSprite->Blit(blit3D->screenWidth / 2, blit3D->screenHeight - 200);
		debugFont->BlitText(blit3D->screenWidth / 2 -100, blit3D->screenHeight - 270, "HIGH SCORES");
	}
}

//the key codes/actions/mods for DoInput are from GLFW: check its documentation for their values
void DoInput(int key, int scancode, int action, int mods)
{
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		blit3D->Quit(); //start the shutdown sequence
	if (key == GLFW_KEY_A && action == GLFW_PRESS)
		ship->couterclockwiseTurn = true;
	if (key == GLFW_KEY_A && action == GLFW_RELEASE)
		ship->couterclockwiseTurn = false;
	if (key == GLFW_KEY_D && action == GLFW_PRESS)
		ship->clockwiseTurn = true;
	if (key == GLFW_KEY_D && action == GLFW_RELEASE)
		ship->clockwiseTurn = false;
	if (key == GLFW_KEY_W && action == GLFW_PRESS)
	{
		ship->thrusting = true;
		audioE->PlayEvent("EngineStart", mainGameID);
	}
	if (key == GLFW_KEY_W && action == GLFW_RELEASE)
	{
		ship->thrusting = false;
		audioE->PlayEvent("EngineStop", mainGameID);
	}
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
		ship->shooting = true;
	if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE)
		ship->shooting = false; 

}

int main(int argc, char *argv[])
{
	//memory leak detection
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	blit3D = new Blit3D(Blit3DWindowModel::BORDERLESSFULLSCREEN_1080P, 640, 400);

	//set our callback funcs
	blit3D->SetInit(Init);
	blit3D->SetDeInit(DeInit);
	blit3D->SetUpdate(Update);
	blit3D->SetDraw(Draw);
	blit3D->SetDoInput(DoInput);

	//Run() blocks until the window is closed
	blit3D->Run(Blit3DThreadModel::SINGLETHREADED);
	if (blit3D) delete blit3D;

	lua_close(L);
}