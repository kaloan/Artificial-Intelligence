#include "DecisionTree.h"
#include<fstream>
#include<sstream>
#include<iostream>

void DecisionTree::setMissingSignifier(const string &missingSig)
{
	missingSignifier = missingSig;
}

void DecisionTree::setPruningConstant(const int &givenConstant)
{
	pruningConstant = givenConstant;
}

vector<vector<short>> DecisionTree::getData()
{
	return data;
}

vector<string> DecisionTree::getFeatureNames()
{
	return featureNames;
}

void DecisionTree::freqToProbs(const vector<int>& frequencies, vector<double>& probs)
{
	int freqSize = frequencies.size();
	//vector<double> probs(freqSize);

	int sumOfFreqs = 0;
	for (int i = 0; i < freqSize; i++)
	{
		sumOfFreqs += frequencies[i];
	}

	for (int i = 0; i < freqSize; i++)
	{
		probs[i] = (double)frequencies[i] / (double)sumOfFreqs;
	}
	//return probs;
}

double DecisionTree::probsToEntropy(const vector<double>& probabilities)
{
	double entropySum = 0.0;
	for (const auto& probability : probabilities)
	{
		entropySum += -probability * log2(probability);
	}
	return entropySum;
}

vector<vector<short>> DecisionTree::filterData(const vector<vector<short>>& dataToFilter, const int &featurePosition, const short &featureValue)
{
	vector<vector<short>> filteredData;
	for (auto& sample : data)
	{
		if (sample[featurePosition] == featureValue) filteredData.push_back(sample);
	}
	return filteredData;
}

vector<vector<vector<short>>> DecisionTree::fullFilterForFeature(const vector<vector<short>>& dataToFilter, const int &featurePosition, vector<vector<double>>& classProbsForFeatureValues)
{

	//Each feature value has its frequencies for class values
	vector<vector<int>> classFreqsForFeatureValues(possibleFeatureValues[featurePosition].size(), vector<int>(possibleFeatureValues[0].size(), 0));

	vector<vector<vector<short>>> dataByFeatureValue(possibleFeatureValues[featurePosition].size());

	//Separate data and count occurences of class values 
	for (const auto& sample : dataToFilter)
	{
		dataByFeatureValue[sample[featurePosition]].push_back(sample);
		++classFreqsForFeatureValues[sample[featurePosition]][sample[0]];
	}

	//Calculate the class probs for each of the feature values
	for (int i = 0; i < classFreqsForFeatureValues.size(); i++)
	{
		//classProbsForFeatureValues[i] = freqToProbs(classFreqsForFeatureValues[i]);
		freqToProbs(classFreqsForFeatureValues[i], classProbsForFeatureValues[i]);
	}
	

	/*
	classProbsForFeatureValues.clear();
	for (const auto& classFreqsForSpecificFeatureValue : classFreqsForFeatureValues)
	{
		classProbsForFeatureValues.push_back(freqToProbs(classFreqsForSpecificFeatureValue));
	}
	*/

	return dataByFeatureValue;
}

void DecisionTree::readData(const string &dataFile, const string &featureInfoFile)
{
	//Open file with information about features
	ifstream featuresInput(featureInfoFile, fstream::in);
	if (!featuresInput)
	{
		cout << "ERROR OPENING FEATURE INFO FILE: " << featureInfoFile;
		return;
	}

	//Read the info for the features
	string cFeatureLine;
	string cFeature;
	string cCategory;
	short categoryNumber;
	while (getline(featuresInput, cFeatureLine))
	{
		categoryNumber = 0;
		vector<short> cPossibleFeatureValues;
		unordered_map<string, short> cFeatureCategories;
		cFeatureCategories[missingSignifier] = -1;

		stringstream lineStream(cFeatureLine);
		getline(lineStream, cFeature, ',');
		featureNames.push_back(cFeature);

		while (getline(lineStream, cCategory, ','))
		{
			cFeatureCategories[cCategory] = categoryNumber;
			cPossibleFeatureValues.push_back(categoryNumber);
			++categoryNumber;
		}
		strToNumDict[cFeature] = cFeatureCategories;
		possibleFeatureValues.push_back(cPossibleFeatureValues);
	}
	featureNames.shrink_to_fit();


	//Open file with data
	ifstream dataInput(dataFile, fstream::in);
	if (!dataInput)
	{
		cout << "ERROR OPENING FEATURE INFO FILE: " << dataFile;
		return;
	}

	//Read the data
	string cDataLine;
	string cDataFeatureValue;
	vector<short> cSample(strToNumDict.size());
	int cPlace;
	bool cSampleMissingValues;

	while (getline(dataInput, cDataLine))
	{
		cPlace = 0;
		cSampleMissingValues = false;

		stringstream lineStream(cDataLine);
		while (getline(lineStream, cDataFeatureValue, ','))
		{
			if (cDataFeatureValue == "?") cSampleMissingValues = true;
			cSample[cPlace] = strToNumDict[featureNames[cPlace]][cDataFeatureValue];
			++cPlace;
		}

		//Skip samples with missing info
		if (!cSampleMissingValues) data.push_back(cSample);
	}
}

DecisionTree::DecisionTree()
{
}


DecisionTree::~DecisionTree()
{
}
