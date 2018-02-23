#pragma once

/*
Class to build Markov Chains for word generation. It can create a Markov probability
mapping for a file and then generate new words based on the mappings.
*/
#include<unordered_map>
#include<string>
#include<random>

class MarkovChain
{
	std::unordered_map<char, int> firstLetterMap; //frequency distribution list of starting letters for words
	std::unordered_map<char, std::unordered_map<char, int>> letterMap; //mapping of letter pairs: probability that a given first letter is followed by any certian second letter
	std::string randomLetters = "abcdefghijklmnopqrstuvwxyz";
	std::mt19937 rng;

public:
	bool ParseFile(std::string filename);//build Markov probablilities from a text file of words
	void ParseString(std::string aString); //add a word to the mapping
	void AddLetterPair(char firstLetter, char secondLetter); //add a letter pair to the mapping
	char GenerateNextLetter(char firstLetter); //generate the next letter in the chain (to follow firstLetter)
	std::string GenerateChain(int length); //Generate a word of a certian length 
	MarkovChain();//initializes the rng
};
