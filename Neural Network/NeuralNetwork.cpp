#include "NeuralNetwork.h"

#include <iostream>
#include <chrono>
#include <random>
#include <math.h>

void NeuralNetwork::printNetwork()
{
	for (int t=0; t<network.size(); t++)
	{
		cout << "Transition from layer " << t << " to layer " << t + 1 << ":" <<endl;
		const auto& transition = network[t];
		for (const auto& row : transition)
		{
			for (const auto& pos : row)
			{
				cout << pos << " ";
			}
			cout << endl;
		}
		cout << endl << endl;
	}
}

void NeuralNetwork::applyActivation(vector<double>& layerInputs)
{
	for (auto& nodeValue : layerInputs)
	{
		nodeValue = 1 / (1 + exp(-nodeValue));
	}
}

double NeuralNetwork::scalarProduct(const vector<double>&vec1, const vector<double>&vec2)
{
	double res=0.0;
	for (int i=0;i<vec1.size();i++)
	{
		res += vec1[i] * vec2[i];
	}
	return res;
}

vector<double> NeuralNetwork::matVecMult(const vector<vector<double>>& mat, const vector<double>& vec)
{
	vector<double> res(mat.size());

	for (int i=0; i<mat.size();i++)
	{
		res[i] = scalarProduct(mat[i], vec);
	}

	return res;
}

vector<double> NeuralNetwork::classify(const vector<double>& input)
{
	vector<double> currentLayerOutput = input;
	copy(currentLayerOutput.begin(), currentLayerOutput.end(), activities[0].begin());
	int i = 1;
	for (const auto& transitionMatrix : network)
	{
		//vector<double> inputsForLayer = matVecMult(transitionMatrix, currentLayerOutput);
		currentLayerOutput = matVecMult(transitionMatrix, currentLayerOutput);
		applyActivation(currentLayerOutput);
		copy(currentLayerOutput.begin(),currentLayerOutput.end(),activities[i++].begin());
	}
	return currentLayerOutput;
}

void NeuralNetwork::backpropagate(const vector<double>& input, const vector<double>& expectedOutput)
{
	double learingRate = 0.1;

	vector<double> returnedOutput = classify(input);

	//Get error of output layer
	vector<double> currentError(returnedOutput.size(), 0);
	for (int i = 0; i < returnedOutput.size(); i++)
	{
		currentError[i] = returnedOutput[i] * (1 - returnedOutput[i]) * (expectedOutput[i] - returnedOutput[i]);
	}
	//Do last transition matrix change
	for (int l = network.size() - 1; l >= 0; l--)
	{
		auto& transition = network[l];
		const auto& prevLayerOutputs = activities[l];
		for (int i = 0; i < transition.size(); i++)
		{
			for (int j = 0; j < transition[i].size(); j++)
			{
				transition[i][j] += learingRate * currentError[i] * prevLayerOutputs[j];
			}
		}

		if (l > 0)
		{
			vector<double> newError(prevLayerOutputs.size(), 0.0);

			//Backpropagation sum
			for (int i = 0; i < transition.size(); i++)
			{
				for (int j = 0; j < transition[i].size(); j++)
				{
					newError[j] += transition[i][j] * currentError[i];
				}
			}

			//Finalize the error
			for (int i = 0; i < newError.size(); i++)
			{
				newError[i] *= prevLayerOutputs[i] * (1 - prevLayerOutputs[i]);
			}

			currentError = move(newError);
		}
	}

	/*
	for (int i = network.size() - 2; i >= 0; i--)
	{
		vector<double> currentError = 
	}
	*/

}

NeuralNetwork::NeuralNetwork()
{
}

NeuralNetwork::NeuralNetwork(const vector<unsigned int>& layerSizes)
{
	//For RNG
	uniform_real_distribution<double> randomise(-0.05, 0.05);
	default_random_engine rng(chrono::system_clock::now().time_since_epoch().count());

	//There has to be a transition
	if (layerSizes.size() < 2)
	{
		cout << "NEED MORE THAN 1 LAYER!" << endl;
		return;
	}

	//Create and fill the transition matrices
	vector<vector<double>> acts;
	for (int i = 0; i < layerSizes.size(); i++)
	{
		acts.push_back(vector<double>(layerSizes[i], 0.0));
	}
	acts.shrink_to_fit();
	activities = move(acts);
	for (int i = 0; i < layerSizes.size() - 1; i++)
	{
		int prevSize = layerSizes[i];
		int nextSize = layerSizes[i + 1];
		vector<vector<double>> transition(nextSize,vector<double>(prevSize,0));
		for (auto& row : transition)
		{
			for (auto& position : row)
			{
				position = randomise(rng);
			}
		}
		network.push_back(transition);
	}

}


NeuralNetwork::~NeuralNetwork()
{
}
