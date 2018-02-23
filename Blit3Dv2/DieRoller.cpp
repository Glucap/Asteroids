#include "DieRoller.h"
#include <iostream>

DieRoller::DieRoller()
{
	std::random_device rd; // assign random number to rd
	SeedRNG(rd());
}

void DieRoller::SeedRNG(unsigned int seedVal)
{
	
	rng.seed(seedVal);
}

int DieRoller::RollDn(int lowNum, int dieSide)
{
	std::uniform_int_distribution<int> Dn(lowNum, dieSide);
	return Dn(rng);
}

int DieRoller::RollXDYDropLowest(int dieAmount,int lowNum, int dieSide)
{
	rollSum = 0;
	lowRoll = dieSide+1;
	/*--------------------------------------
	setting my lowest roll higher than any
	possible roll on the die so the 
	first die will automatically register as the lowest.
	----------------------------------------*/
		
	for (looper = 0; looper < dieAmount; looper++) //loops as many times as you roll dice
	{
		rollResult = RollDn(lowNum ,dieSide); //roll 1 six sided die
		if (rollResult < lowRoll) lowRoll = rollResult;//check agains lowest roll to see which is lower and assings lowest value
		rollSum += rollResult; //adds my roll to the total

	}
	rollSum = rollSum - lowRoll; //remove the lowest rolled value from the total
		
	return rollSum;
	
}
int DieRoller::RollXDY(int dieAmount, int lowNum, int dieSide)
{
	rollSum = 0;

	for (looper = 0; looper < dieAmount; looper++) //loops 4 times
	{
		rollResult = RollDn(lowNum, dieSide);
		rollSum += rollResult;
	}
	return rollSum;
}

int DieRoller::Roll5D6Keep3()
{
	rollSum = 0;
	lowRoll = 7;
	nextLowRoll = 7;

	for (looper = 0; looper < 5; looper++) //loops as many times as you roll dice
	{
		rollResult = RollDn(1, 6); //roll 1 six sided die
		if (rollResult < lowRoll)
			lowRoll = rollResult;//check agains lowest roll to see which is lower and assings lowest value
		else if (rollResult < nextLowRoll)
			nextLowRoll = rollResult;

		rollSum += rollResult; //adds my roll to the total
	}
	rollSum = rollSum - lowRoll - nextLowRoll; //remove the lowest rolled value from the total
	return rollSum;

}

int DieRoller::RollXDYDropZ(int dieAmount, int lowNum, int dieSide, int dropAmount)
{
	return 0;
}

float DieRoller::RollFloat(float lowNum, float hiNum)
{
	std::uniform_real_distribution<float> Dn(lowNum, hiNum);
	return Dn(rng);
}