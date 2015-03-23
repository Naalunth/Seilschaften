#pragma once
#include <vector>

using namespace std;

typedef unsigned long uint32;
typedef unsigned long long uint64;

class Tower
{
public:

	struct Situation
	{
	public:
		//1 := top, 0 := bottom
		vector<bool> peoplePositions;
		//1 := top, 0 := bottom
		vector<bool> stonePositions;

		bool IsSolution();
		bool operator<(const Situation& other) const;
	};

	struct SolutionStep
	{
	public:
		vector<size_t> upPeople;
		vector<size_t> downPeople;
		vector<size_t> upStones;
		vector<size_t> downStones;
	};


	vector<uint32> peopleWeights;
	vector<uint32> stoneWeights;

	uint32 limit;

	Tower();
	~Tower();

	vector<SolutionStep>* SolveForSituation(Situation&);

};

