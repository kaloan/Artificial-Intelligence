#pragma once
#include<unordered_map>
#include<string>
#include<math.h>
using namespace std;

class DecisionTree
{
private:
	string missingSignifier; //The string for a missing value - with C++17 : constexpr string_view
	vector<vector<short>> data; //The data
	vector<string> featureNames; //Names of class and features
	unordered_map<string, unordered_map<string, short>> strToNumDict; //Maps each feature name to the mapping of its category value names to shorts
	vector<vector<short>> possibleFeatureValues; //At i holds the possible values of feature at position i
	int pruningConstant;

public:
	void setMissingSignifier(const string&);
	void setPruningConstant(const int&);

	vector<vector<short>> getData();
	vector<string> getFeatureNames();

	void freqToProbs(const vector<int>&, vector<double>&);
	double probsToEntropy(const vector<double>&);
	vector<vector<short>> filterData(const vector<vector<short>>&, const int&, const short&);
	vector<vector<vector<short>>> fullFilterForFeature(const vector<vector<short>>&, const int&, vector<vector<double>>&);
	void readData(const string&,const string&);



	DecisionTree();
	~DecisionTree();
};

