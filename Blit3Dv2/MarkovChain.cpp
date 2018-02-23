#include "MarkovChain.h"
#include<fstream>

MarkovChain::MarkovChain()
{
	std::random_device rd;
	rng.seed(rd());
}

bool MarkovChain::ParseFile(std::string filename)
{
	//read file in
	std::ifstream inputFile(filename);

	std::string inputString;

	if (inputFile.is_open())
	{
		while (!inputFile.eof())
		{
			inputFile >> inputString;
			ParseString(inputString);
		}
		inputFile.close();
		return true;
	}

	//couldn't open file, so return error
	return false;
}

void MarkovChain::ParseString(std::string aString)
{
	//increment first letter frequency list
	auto search = firstLetterMap.find(aString[0]);
	if (search != firstLetterMap.end())
	{
		search->second++;
	}
	else
	{
		firstLetterMap[aString[0]] = 1;
	}

	//now add letter pairs
	for (unsigned int i = 0; i < aString.size() - 1; ++i)
	{
		AddLetterPair(aString[i], aString[i + 1]);
	}
}

void MarkovChain::AddLetterPair(char firstLetter, char secondLetter)
{
	//look up firstLetter in the map
	auto search = letterMap.find(firstLetter);
	if (search != letterMap.end())
	{
		//firstLetter is in the map, now see if it has secondLetter on it's list of following letters
		auto innerSearch = search->second.find(secondLetter);
		if (innerSearch != search->second.end())
		{
			//secondLetter already exists, so increment it's count
			innerSearch->second++;
		}
		else
		{
			//we've never seen secondLetter follow firstLetter before, so add it with a count of 1
			search->second[secondLetter] = 1;
		}
	}
	else
	{
		//firstLetter isn't in the list of first letters, so add it and secondLetter to the maps
		std::unordered_map<char, int> m;
		m[secondLetter] = 1;
		letterMap[firstLetter] = m;
	}
}

char MarkovChain::GenerateNextLetter(char firstLetter)
{
	char nextLetter = 'a';
	//look up first in the map
	auto search = letterMap.find(firstLetter);
	if (search != letterMap.end())
	{
		//loop over all of the following letters and add up their counts
		int total = 0;
		for each (auto letter in search->second)
		{
			total += letter.second;
		}

		//roll a random number based off the total
		std::uniform_int_distribution<int> dist(1, total);
		int randRoll = dist(rng);

		//find the letter that matches this roll
		int totalSoFar = 0;
		for each (auto letter in search->second)
		{
			totalSoFar += letter.second;
			if (totalSoFar >= randRoll)
			{
				nextLetter = letter.first;
				break;
			}
		}

	}
	else
	{
		//We didn't find this letter as a letter that has anything following it, ever.
		//er....what now, roll a random letter? Probably best, or just end here?
		std::uniform_int_distribution<int> dist26(0, 25);
		nextLetter = randomLetters[dist26(rng)];
	}

	return nextLetter;
}

std::string MarkovChain::GenerateChain(int length)
{
	std::string chain;
	//First, pick a first letter.
	char firstLetter = 'A';
	//look it up in a frequency list of first letters...
	//loop over all of the first letters and add up their counts
	int total = 0;
	for each (auto letter in firstLetterMap)
	{
		total += letter.second;
	}

	//roll a random number based off the total
	std::uniform_int_distribution<int> dist(1, total);
	int randRoll = dist(rng);

	//find the letter that matches this roll
	int totalSoFar = 0;
	for each (auto letter in firstLetterMap)
	{
		totalSoFar += letter.second;
		if (totalSoFar >= randRoll)
		{
			firstLetter = letter.first;
			break;
		}
	}

	//add the chosen first letter to our string
	chain += firstLetter;

	//generate the rest of the chain based on the Markov Chain
	for (int i = 0; i < length - 1; ++i)
		chain += GenerateNextLetter(chain[i]);

	return chain;
}