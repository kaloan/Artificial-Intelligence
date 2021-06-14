#include <iostream>
#include "NeuralNetwork.h"
using namespace std;

int main()
{
	unsigned int middleLayerSize = 4;
	vector<unsigned int> layerSizes({2,middleLayerSize,1});
	NeuralNetwork network(layerSizes);
	//network.printNetwork();
	
	//Generate the truth table
	vector<vector<double>> examples;
	vector<vector<double>> expected;
	for (int i = 0; i <= 1; i++)
	{
		for(int j = 0; j <= 1; j++)
		{
			examples.push_back({(double)i,(double)j});
			expected.push_back({(double)((i && !j) || (!i && j))});
		}
	}
	examples.shrink_to_fit();
	expected.shrink_to_fit();
	
	//Print truth table
	cout << "The truth table is: " << endl;
	for (int i = 0; i < examples.size(); i++)
	{
		for (int j = 0; j < examples[i].size(); j++)
		{
			cout << examples[i][j] << " ";
		}
		cout << expected[i][0];
		cout << endl;
	}

	//Training
	int K = 1000000;
	cout << "Training " << K << " times through the truth table" << endl;
	for (int k = 0; k < K; k++)
	{
		for (int i = 0; i < examples.size(); i++)
		{
			network.backpropagate(examples[i], expected[i]);
		}
	}

	//Testing
	cout << "Testing the network: " << endl;
	for (int i = 0; i < examples.size(); i++)
	{
		vector<double> testTheNetwork = network.classify(examples[i]);
		cout << "INPUT IS: ";
		for (int j = 0; j < examples[i].size(); j++)
		{
			cout << examples[i][j] << " ";
		}
		cout << endl << "OUTPUT IS: " << testTheNetwork[0] << endl << endl;
	}

	cin.get();

	return 0;
}