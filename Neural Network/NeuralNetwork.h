#pragma once
#include <vector>

using namespace std;

class NeuralNetwork
{
private:
	vector<vector<vector<double>>> network; //A vector of matrices of real numbers which are used to transition between layers.
	vector<vector<double>> activities;
public:
	void printNetwork();

	void applyActivation(vector<double>&); //Could be done with std::transform instead
	double scalarProduct(const vector<double>&, const vector<double>&);
	vector<double> matVecMult(const vector<vector<double>>&, const vector<double>&);
	vector<double> classify(const vector<double>&);
	
	void backpropagate(const vector<double>&,const vector<double>&);

	NeuralNetwork();
	NeuralNetwork(const vector<unsigned int>&);
	~NeuralNetwork();
};

