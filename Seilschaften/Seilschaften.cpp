#include "Seilschaften.h"
#include <stdio.h>
#include <utility>
#include <iostream>
#include <fstream>
#include <io.h>
#include <fcntl.h>
#include <string>
#include <ios>

#include "Tower.h"


int GetTowerFromFile(const char* filename, Tower** outTower, Tower::Situation** outSituation)
{
	Tower* tower = new Tower();
	*outTower = tower;

	Tower::Situation* situation = new Tower::Situation();
	*outSituation = situation;

	ifstream file(filename, ios::in | ios::binary);
	if (!file.is_open())
	{
		printf("Wasn't able to open %s, i'm sorry. :(\n", filename);
		return 1;
	}

	file >> tower->limit;

	char type, pos;
	uint32 weight;
	while (file >> std::uppercase >> type >> weight >> pos)
	{
		(type == 'P' ? tower->peopleWeights : tower->stoneWeights).push_back(weight);
		(type == 'P' ? situation->peoplePositions : situation->stonePositions).push_back(pos == '^' ? TOWER_TOP : TOWER_BOTTOM);
	}
	situation->stoneIsInBottomBasket = std::vector<bool>(situation->stonePositions.size(), false);

	return 0;
}


void PrintFancy(const Tower& tower, const Tower::Situation& startSituation, const std::vector<Tower::SolutionStep>& solution)
{
	auto PrintSituation = [&](const Tower::Situation& situation)
	{
		auto printy = [&](const std::vector<bool>& a, const std::vector<uint32>& b, bool c){for (int i = 0; i < a.size(); i++)if (a[i] == c)std::cout << b[i] << " "; };
		std::cout << "Top people: ";
		printy(situation.peoplePositions, tower.peopleWeights, TOWER_TOP);
		std::cout << "\nTop stones: ";
		printy(situation.stonePositions, tower.stoneWeights, TOWER_TOP);
		std::cout << "\nBottom people: ";
		printy(situation.peoplePositions, tower.peopleWeights, TOWER_BOTTOM);
		std::cout << "\nBottom stones: ";
		printy(situation.stonePositions, tower.stoneWeights, TOWER_BOTTOM);
		std::cout << "\n";
		fflush(stdout);
	};

	auto PrintStep = [&](const Tower::SolutionStep& step)
	{
		auto printy = [&](const std::vector<size_t>& a, const std::vector<uint32>& b){for (size_t i : a) std::cout << b[i] << " "; };
		std::cout << "People going down: ";
		printy(step.downPeople, tower.peopleWeights);
		std::cout << "\nStones going down: ";
		printy(step.downStones, tower.stoneWeights);
		std::cout << "\nPeople going up: ";
		printy(step.upPeople, tower.peopleWeights);
		std::cout << "\nStones going up: ";
		printy(step.upStones, tower.stoneWeights);
		std::cout << "\n";
		fflush(stdout);
	};

	Tower::Situation sit = startSituation;
	PrintSituation(startSituation);
	std::cout << "\n\n";
	for (int i = 0; i < solution.size(); i++)
	{
		std::cout << "Step " << i + 1 << ":\n\n";
		PrintStep(solution[i]);
		std::cout << "\nSituation after step " << i + 1 << ":\n\n";
		sit = sit + solution[i];
		PrintSituation(sit);
		std::cout << "\n\n";
	}
	fflush(stdout);
}


int main()
{
	char* stdobuf = new char[4096];
	setvbuf(stdout, stdobuf, _IOFBF, 4096);

	std::cout << "Seilschaften - by Kevin Schier\n\n";

	string filename;
	Tower* tower;
	Tower::Situation* startSituation;
	std::vector<Tower::SolutionStep>* solution;

begin:
	std::cout << "Type in a file to load: ";
	std::cin >> filename;

	if (GetTowerFromFile(filename.data(), &tower, &startSituation))
	{
		std::cout << "Couldn't construct a tower from this file.";
		goto cleanUp;
	}

	std::cout << "\n\n";

	solution = tower->SolveForSituation(*startSituation);

	if (solution->size() == 0)
	{
		std::cout << "No solution found.\n";
		goto cleanUp;
	}

	PrintFancy(*tower, *startSituation, *solution);

cleanUp:
	SAFE_DELETE(tower);
	SAFE_DELETE(startSituation);
	std::cout << "\n\n";
	goto begin;

	return 0;
}

