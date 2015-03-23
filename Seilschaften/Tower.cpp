#include "Tower.h"
#include <queue>
#include <stack>
#include <set>


Tower::Tower()
{
}


Tower::~Tower()
{
}



vector<Tower::SolutionStep>* Tower::SolveForSituation(Tower::Situation& startingSituation)
{
	struct SearchStruct
	{
		Tower::Situation situation;
		Tower::SolutionStep solutionStep;
		SearchStruct* lastStep;
		~SearchStruct(){ if (lastStep) delete lastStep; }
	};
	vector<Tower::SolutionStep>* result = 0;
	queue<SearchStruct*> workBuffer;
	set<Situation> alreadyChecked;
	SearchStruct* currentNode = new SearchStruct{ startingSituation, SolutionStep(), 0 };

	workBuffer.push(currentNode);


	auto DifferenceBetweenSituations = [&](const Situation& a, const Situation& b) -> uint32 {
		int res = 0;
		for (size_t p = 0; p < peopleWeights.size(); p++)
		{
			res += a.peoplePositions[p] ? peopleWeights[p] : -peopleWeights[p];
			res -= b.peoplePositions[p] ? peopleWeights[p] : -peopleWeights[p];
		}
		for (size_t s = 0; s < stoneWeights.size(); s++)
		{
			res += a.stonePositions[s] ? stoneWeights[s] : -stoneWeights[s];
			res -= b.stonePositions[s] ? stoneWeights[s] : -stoneWeights[s];
		}
		res /= 2;
		return res >= 0 ? res : -res;
	};


	auto CreateSolutionStep = [&](const Situation& from, const Situation& to) -> SolutionStep {
		SolutionStep res;
		for (size_t p = 0; p < peopleWeights.size(); p++)
		{
			if (from.peoplePositions[p] > to.peoplePositions[p])
				res.downPeople.push_back(p);
			else if (from.peoplePositions[p] < to.peoplePositions[p])
				res.upPeople.push_back(p);
		}
		for (size_t s = 0; s < stoneWeights.size(); s++)
		{
			if (from.stonePositions[s] > to.stonePositions[s])
				res.downStones.push_back(s);
			else if (from.stonePositions[s] < to.stonePositions[s])
				res.upStones.push_back(s);
		}
		return res;
	};


	auto BuildAllAllowedCombinations = [&](SearchStruct* originalSearchStruct) -> vector<SearchStruct*> {
		vector<SearchStruct*> res;
		SearchStruct* ssc = new SearchStruct;
		memcpy(ssc, originalSearchStruct, sizeof SearchStruct);
		ssc->lastStep = originalSearchStruct;
		uint64* personBitfields = new uint64[peopleWeights.size() / 64 + 1];
		uint64* stoneBitfields = new uint64[stoneWeights.size() / 64 + 1];
		for (;;)
		{
			for (;;)
			{

				if (alreadyChecked.find(ssc->situation) == alreadyChecked.end()
					&& DifferenceBetweenSituations(ssc->situation, originalSearchStruct->situation) <= limit)
				{
					ssc->solutionStep = CreateSolutionStep(originalSearchStruct->situation, ssc->situation);
					SearchStruct* s = new SearchStruct;
					memcpy(s, ssc, sizeof SearchStruct);
					res.push_back(s);
					alreadyChecked.insert(s->situation);
				}

				auto it = ssc->situation.stonePositions.begin();
				for (;;)
				{
					if (*it)
					{
						*it = false;
						if (++it == ssc->situation.stonePositions.end()) break;
					}
					else
					{
						*it = true;
						break;
					}
				}
				if (ssc->situation.stonePositions == originalSearchStruct->situation.stonePositions)
				{
					break;
				}
			}

			auto it = ssc->situation.peoplePositions.begin();
			for (;;)
			{
				if (*it)
				{
					*it = false;
					if (++it == ssc->situation.peoplePositions.end()) break;
				}
				else
				{
					*it = true;
					break;
				}
			}
			if (ssc->situation.peoplePositions == originalSearchStruct->situation.peoplePositions)
			{
				break;
			}
		}
	};


	while (!workBuffer.empty())
	{
		currentNode = workBuffer.front();
		workBuffer.pop();

		if (currentNode->situation.IsSolution())
		{
			break;
		}

		vector<SearchStruct*> com = BuildAllAllowedCombinations(currentNode);
		sort(com.begin(), com.end(), [&](SearchStruct* a, SearchStruct* b){return a->situation < b->situation; });
		for (SearchStruct* s : com)
		{
			workBuffer.push(s);
		}
	}

	return result;
}


bool Tower::Situation::IsSolution()
{
	for (auto it = peoplePositions.begin(); it != peoplePositions.end(); it++)
	{
		if (!*it) return false;
	}
	return true;
}

bool Tower::Situation::operator<(const Situation& other) const
{
	auto it1 = peoplePositions.begin();
	auto it2 = other.peoplePositions.begin();
	int a = 0, b = 0;
	while (it1 != peoplePositions.end())
	{
		a += *(it1++);
		b += *(it2++);
	}
	if (a != b) return a > b;

	it1 = peoplePositions.begin();
	it2 = other.peoplePositions.begin();
	while (*(it1++) == *(it2++));
	if (it1 == peoplePositions.end() && !(it2 == other.peoplePositions.end())) return true;
	if (it2 == other.peoplePositions.end() && !(it1 == peoplePositions.end())) return false;
	if (!(it1 == peoplePositions.end() && it2 == other.peoplePositions.end())) return *--it1 < *--it2;

	it1 = stonePositions.begin();
	it2 = other.stonePositions.begin();
	while (*(it1++) == *(it2++));
	if (it1 == stonePositions.end() && !(it2 == other.stonePositions.end())) return true;
	if (it2 == other.stonePositions.end() && !(it1 == stonePositions.end())) return false;
	if (!(it1 == stonePositions.end() && it2 == other.stonePositions.end())) return *--it1 < *--it2;

	return false;
}

