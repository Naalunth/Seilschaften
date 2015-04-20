#include "Tower.h"
#include <queue>
#include <stack>
#include <set>
#include <algorithm>
#include <utility>


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
	vector<Tower::SolutionStep>* result = new vector<Tower::SolutionStep>();
	queue<SearchStruct*> workBuffer;
	set<Situation> alreadyChecked;
	alreadyChecked.insert(startingSituation);
	SearchStruct* currentNode = new SearchStruct{ startingSituation, SolutionStep(), 0 };

	workBuffer.push(currentNode);


	auto DifferenceBetweenSituations = [&](const Situation& a, const Situation& b) -> int {
		int res = 0;
		for (size_t p = 0; p < peopleWeights.size(); p++)
		{
			res += a.peoplePositions[p] ? peopleWeights[p] : -(int) peopleWeights[p];
			res -= b.peoplePositions[p] ? peopleWeights[p] : -(int) peopleWeights[p];
		}
		for (size_t s = 0; s < stoneWeights.size(); s++)
		{
			res += a.stonePositions[s] ? stoneWeights[s] : -(int) stoneWeights[s];
			res -= b.stonePositions[s] ? stoneWeights[s] : -(int) stoneWeights[s];
		}
		res /= 2;
		return res;
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


	auto BuildAllAllowedCombinations = [&](SearchStruct* const originalSearchStruct) -> vector < SearchStruct* > {
		vector<SearchStruct*> res;
		SearchStruct* ssc = new SearchStruct(*originalSearchStruct);
		ssc->lastStep = originalSearchStruct;

		bool canStonesGoUp =
			std::find(originalSearchStruct->situation.peoplePositions.begin(), originalSearchStruct->situation.peoplePositions.end(), TOWER_BOTTOM)
			!=
			originalSearchStruct->situation.peoplePositions.end();


		for (;;)
		{
			for (;;)
			{
				if (!canStonesGoUp)
				{
					for (size_t s = 0; s < stoneWeights.size(); s++)
					{
						//Any situation where a stone can't be moved
						if ((ssc->situation.stonePositions[s] > originalSearchStruct->situation.stonePositions[s] && !originalSearchStruct->situation.stoneIsInBottomBasket[s])
							|| (ssc->situation.stonePositions[s] == TOWER_BOTTOM && originalSearchStruct->situation.stonePositions[s] == TOWER_BOTTOM && originalSearchStruct->situation.stoneIsInBottomBasket[s]))
							goto doNotInsert;
					}
				}

				bool onlyStonesMove = true;
				for (size_t p = 0; p < peopleWeights.size(); p++)
				{
					if (originalSearchStruct->situation.peoplePositions[p] != ssc->situation.peoplePositions[p])
					{
						onlyStonesMove = false;
						break;
					}
				}

				int d = DifferenceBetweenSituations(originalSearchStruct->situation, ssc->situation);
				if (d < 0) goto doNotInsert;
				if (!onlyStonesMove)
				{
					if (d > (int) limit) goto doNotInsert;
				}

				if (alreadyChecked.find(ssc->situation) != alreadyChecked.end()) goto doNotInsert;

				ssc->solutionStep = CreateSolutionStep(originalSearchStruct->situation, ssc->situation);
				SearchStruct* s = new SearchStruct(*ssc);
				for (int i = 0; i < s->situation.stoneIsInBottomBasket.size(); i++)
					s->situation.stoneIsInBottomBasket[i] = std::find(s->solutionStep.downStones.begin(), s->solutionStep.downStones.end(), i) != s->solutionStep.downStones.end();
				res.push_back(s);
				alreadyChecked.insert(s->situation);

			doNotInsert:
				if (ssc->situation.stonePositions.size() <= 0)
				{
					break;
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

		return res;
	};

	while (!workBuffer.empty())
	{
		currentNode = workBuffer.front();
		workBuffer.pop();

		if (currentNode->situation.IsSolution())
		{
			goto foundSolution;
		}

		vector<SearchStruct*> com = BuildAllAllowedCombinations(currentNode);
		sort(com.begin(), com.end(), [&](SearchStruct* a, SearchStruct* b){return a->situation < b->situation; });
		for (SearchStruct* s : com)
		{
			workBuffer.push(s);
		}
	}
	return result;

foundSolution:
	result->push_back(currentNode->solutionStep);
	while (currentNode->lastStep)
	{
		currentNode = currentNode->lastStep;
		result->push_back(currentNode->solutionStep);
	}
	result->pop_back();
	std::reverse(result->begin(), result->end());

	return result;
}


bool Tower::Situation::IsSolution()
{
	for (auto a : peoplePositions)
	{
		if (a == TOWER_TOP) return false;
	}
	return true;
}

bool Tower::Situation::operator==(const Situation& other) const
{
	return this->peoplePositions == other.peoplePositions && this->stonePositions == other.stonePositions && this->stoneIsInBottomBasket == other.stoneIsInBottomBasket;
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
	if (stonePositions.size() > 0)
	{
		while (*(it1++) == *(it2++) && it1 != peoplePositions.end() && it2 != other.peoplePositions.end());
		if (it1 == peoplePositions.end() && !(it2 == other.peoplePositions.end())) return true;
		if (it2 == other.peoplePositions.end() && !(it1 == peoplePositions.end())) return false;
		if (*(--it1) != *(--it2)) return *it1 < *it2;
	}

	it1 = stonePositions.begin();
	it2 = other.stonePositions.begin();
	if (stonePositions.size() > 0)
	{
		while (*(it1++) == *(it2++) && it1 != stonePositions.end() && it2 != other.stonePositions.end());
		if (it1 == stonePositions.end() && !(it2 == other.stonePositions.end())) return true;
		if (it2 == other.stonePositions.end() && !(it1 == stonePositions.end())) return false;
		if (*(--it1) != *(--it2)) return *it1 < *it2;
	}

	return false;
}

Tower::Situation Tower::Situation::operator+(const Tower::SolutionStep& step) const
{
	Situation res = *this;
	auto x = [&](vector<size_t> a, vector<bool>& b, bool c){for (int i : a)b[i] = c; };
	x(step.downPeople, res.peoplePositions, TOWER_BOTTOM);
	x(step.upPeople, res.peoplePositions, TOWER_TOP);
	x(step.downStones, res.stonePositions, TOWER_BOTTOM);
	x(step.upStones, res.stonePositions, TOWER_TOP);
	return res;
}

