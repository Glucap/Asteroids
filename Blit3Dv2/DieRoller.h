#pragma once
#include <random>
#include <vector>

extern int dieAmount;
extern int dieSide; //type of die

class DieRoller
{
public:

	std::mt19937 rng;
	
	DieRoller(); //constructor that seeds the rng with a std::random_device

	void SeedRNG(unsigned int seedVal); //seeds the rng with the seedVal
	int RollDn(int lowNum, int dieSide); // returns the result of an N-sided die roll (between 1 and N);
	int RollXDYDropLowest(int dieAmount, int lowNum, int dieSide); // rolls X amount of Y-sided dice and drops the lowest roll
	int RollXDY(int dieAmount, int lowNum, int dieSide); //returns the result of X amount of Y sisded dice
	int RollXDYDropZ(int dieAmount, int lowNum, int dieSide, int dropAmount);
	int Roll5D6Keep3();
	float RollFloat(float lowNum, float hiNum);
private:

	int rollResult;
	int nextLowRoll;
	int lowRoll;
	int rollSum;
	int looper;
};
