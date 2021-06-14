#pragma once

#include<iostream>
#include<math.h>
#include<stdlib.h>
#include<random>
#include<time.h>
#include<vector>
#include<utility>
#include<chrono>
#include <limits>

using namespace std;

class Population
{
private:
	double maxDist;
	int populationSize;
	int numberOfPoints;
	int numberOfGenerations;

	vector<pair<double, double>> points;
	vector<char> crossoverType;
	vector<char> mutationType;

public:
	void printPoints() const;
	void randomizePoints();
	double pathLength(const vector<int>&) const;
	static vector<int> permutePoints(const int&);
	static void printGenotype(const vector<int>&,const vector<pair<double,double>>&);

	vector<pair<double, double>> getPoints();
	void getPopulationFitness(const vector<vector<int>>&, vector<pair<double, int>>&) const;
	void mutate(vector<int>&, const int&, const int&, const int&) const;
	void produceOffspring(const vector<int>&, const vector<int>&, vector<int>&, vector<int>&, const int&, const int&, const int&) const;

	void swapMutation(vector<int>&, const int&, const int&) const;
	void insertionMutation(vector<int>&,const int&,const int&) const;
	void reverseMutation(vector<int>&, const int&, const int&) const;

	void onePointCrossover(const vector<int>&, const vector<int>&, vector<int>&, vector<int>&, const int&) const;
	void twoPointCrossover(const vector<int>&, const vector<int>&, vector<int>&, vector<int>&, const int&, const int&) const;
	void partiallyMappedCrossover(const vector<int>&, const vector<int>&, vector<int>&, vector<int>&, const int&, const int&) const;
	void cycleCrossover(const vector<int>&, const vector<int>&, vector<int>&, vector<int>&) const;

	void produceNextGeneration(const vector<vector<int>>&, vector<vector<int>>&) const;
	void evolution() const;


	Population();
	Population(const double&, const int&, const int&, const int&, const vector<char>&, const vector<char>&);
	~Population();
};

