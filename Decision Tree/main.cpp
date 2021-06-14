#include<iostream>
#include<fstream>
#include<sstream>
#include<math.h>
#include<vector>
#include<algorithm>
#include<unordered_map>
#include<string>


using namespace std;

//The string for a missing value - with C++17 : constexpr string_view
const string missingSignifier = "?";

//For simplicity most of the functions consider the class as a "feature"


void filterData(const vector<vector<short>>& data, vector<vector<short>>& filteredData, const int& featurePosition, const short& featureValue)
{
	for (auto& sample : data)
	{
		if (sample[featurePosition] == featureValue) filteredData.push_back(sample);
	}
}

double findEntropy(const vector<vector<short>>& data, const int& featurePosition, const short& featureValue)
{
	return 0.0;
}

void createDecisionTree(const vector<vector<short>>& data, const vector<string>& featureNames)
{

}


void readData(const string& dataFilename, const string& featureInfoFile, vector<string>& featureNames, vector<vector<short>>& data)
{
	//Maps each feature name to the mapping of its category value names to shorts
	unordered_map<string, unordered_map<string, short>> strToNumDict;


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
		unordered_map<string, short> cFeatureCategories;
		cFeatureCategories[missingSignifier] = -1;

		stringstream lineStream(cFeatureLine);
		getline(lineStream, cFeature, ',');
		featureNames.push_back(cFeature);

		while (getline(lineStream, cCategory, ','))
		{
			cFeatureCategories[cCategory] = categoryNumber;
			++categoryNumber;
		}
		strToNumDict[cFeature] = cFeatureCategories;

	}
	featureNames.shrink_to_fit();


	//Open file with data
	ifstream dataInput(dataFilename, fstream::in);
	if (!dataInput)
	{
		cout << "ERROR OPENING FEATURE INFO FILE: " << dataFilename;
		return;
	}

	//Read the data
	string cDataLine;
	string cDataFeatureValue;
	vector<short> cSample(strToNumDict.size());
	int cPlace;
	bool cSampleMissingValues;

	while (getline(dataInput,cDataLine))
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
		if(!cSampleMissingValues) data.push_back(cSample);
	}
}


int main()
{
	string dataFilename = "D:/Programming/AI/DecisionTree/Debug/breast-cancer.data";
	string featureInfoFile = "D:/Programming/AI/DecisionTree/Debug/breast-cancer-features.txt";
	vector<string> featureNames;
	vector<vector<short>> data;


	readData(dataFilename, featureInfoFile, featureNames, data);
	
	createDecisionTree(data, featureNames);
	//Test reading
	/*
	for (int i = 0; i < featureNames.size(); i++)
	{
		cout << "Feature/class" << i << ": " << featureNames[i] << endl;
	}
	for (auto& feat : featureNames)
	{
		cout << feat << endl;
	}

	for (auto s : data)
	{
		for (auto f : s)
		{
			cout << f << " ";
		}
		cout << endl;
	}
	*/
	for (int i = 0; i < featureNames.size(); i++)
	{
		cout << "Feature/class" << i << ": " << featureNames[i] << endl;
	}
	for (const auto& feat : featureNames)
	{
		cout << feat << endl;
	}

	for (const auto& s : data)
	{
		for (const auto& f : s)
		{
			cout << f << " ";
		}
		cout << endl;
	}

	//cout << "Standard: " << __cplusplus;
	
	cin.get();
	return 0;
}