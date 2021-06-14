#include<iostream>
#include<fstream>
#include<vector>
#include<math.h>
#include<string>
#include<limits>
#include<algorithm>
#include<utility>
#include<unordered_map>

using namespace std;
constexpr short missingValue = -2;

//Find the square of Euclidean distance between rowWithMissing and other
short findDistance(const vector<short>& rowWithMissing,const vector<short>& other,const vector<int>& missingPlaces)
{
	//Care only about the ones with the same class
	if (rowWithMissing[0] != other[0])
	{
		return numeric_limits<short>::max();
	}

	//Don't bother with others that have a missing place in the same spot
	for (int i = 0; i < missingPlaces.size(); i++)
	{
		if (other[missingPlaces[i]] == missingValue)
		{
			return numeric_limits<short>::max();
		}
	}

	short dist = 0;
	int skipMissingLocations = 0;
	for (int i = 1; i < rowWithMissing.size(); i++)
	{
		if (skipMissingLocations < missingPlaces.size() && i == missingPlaces[skipMissingLocations])
		{
			++skipMissingLocations;
			continue;
		}
		dist += (other[i] - rowWithMissing[i])*(other[i] - rowWithMissing[i]);
	}
	return dist;
}

void kClosest(vector<vector<short>>& data, const int& rowWithMissing, const vector<int>& missingPlaces, const vector<vector<short>>& featureTypes, const int& k)
{
	//Holds the distance to the i-th row
	vector<pair<short,int>> distances(data.size(), make_pair(0, 0));
	
	for (int i = 0; i < distances.size(); i++)
	{
		distances[i] = make_pair(findDistance(data[rowWithMissing], data[i], missingPlaces), i);
	}
	sort(distances.begin(),distances.end());

	// vector<int> numberOfY(missingPlaces.size(), 0);
	vector<unordered_map<short, int>> occurencesOfDifferentTypes;
	for (int i = 0; i < missingPlaces.size(); i++)
	{
		unordered_map<short, int> temp;
		for (int t = 0; t < featureTypes[missingPlaces[i] - 1].size(); t++)
		{
			temp[featureTypes[missingPlaces[i] - 1][t]] = 0;
		}
		occurencesOfDifferentTypes.push_back(temp);
	}
	for (int i = 0; i < k; i++)
	{
		for (int j = 0; j < missingPlaces.size(); j++)
		{
			//i-th closest data row's value for missingPlaces[j]
			//if (data[distances[i].second][missingPlaces[j]]) ++numberOfY[j];

			++occurencesOfDifferentTypes[j][data[distances[i].second][missingPlaces[j]]];
		}
	}

	int maxOcc;
	short maxOccType;
	for (int i = 0; i < missingPlaces.size(); i++)
	{
		//If more than k/2 are 1(y) then we must assing 1 to the missing place
		//if (numberOfY[i] > k / 2) data[rowWithMissing][missingPlaces[i]] = 1;
		//else data[rowWithMissing][missingPlaces[i]] = 0;

		maxOcc = -1;
		for (int t = 0; t < featureTypes[missingPlaces[i] - 1].size(); t++)
		{
			if (occurencesOfDifferentTypes[i][featureTypes[missingPlaces[i] - 1][t]] > maxOcc)
			{
				maxOcc = occurencesOfDifferentTypes[i][featureTypes[missingPlaces[i] - 1][t]];
				maxOccType = featureTypes[missingPlaces[i] - 1][t];
			}
		}
		data[rowWithMissing][missingPlaces[i]] = maxOccType;
	}
}

void fillMissing(vector<vector<short>>& data, vector<int>& missingData, vector<vector<int>>& whatsMissing, const vector<vector<short>>& featureTypes, const int& maxMissing)
{
	int k = (int) sqrt(data.size());
	if (k % 2 == 0) k++;
	for (int i = 0;; i++)
	{
		/*
		for (int j = 0; j < data[missingData[i]].size(); j++)
		{
			if (data[missingData[i]][j] == -1) kClosest(data, missingData[i], j, k);
		}

		*/
		if (i == missingData.size()) break;
		if (whatsMissing[i].size() >= maxMissing)
		{
			data.erase(data.begin() + missingData[i]);
			missingData.erase(missingData.begin() + i);
			whatsMissing.erase(whatsMissing.begin() + i);
			for (int j = i; j < missingData.size(); j++) --missingData[j];
			--i;
		}
	}

	for (int i = 0; i < missingData.size(); i++)
	{
		kClosest(data, missingData[i], whatsMissing[i], featureTypes, k);
	}
}

//Directly gives the log of probs
void naiveBayesClassifier(const vector<vector<short>>& data, vector<vector<unordered_map<short, double>>>& probs, vector<double>& classProbs, const vector<vector<short>>& featureTypes)
{
	int rowSize = data[0].size();
	int numFeatures = featureTypes.size();
	int classNum = probs.size();

	//Same as for probs, but counting with ints to save time doing addition
	vector<unordered_map<short, int>> frequencyOneClass;
	for (int f = 0; f < featureTypes.size(); f++)
	{
		unordered_map<short, int> temp;
		for (int t = 0; t < featureTypes[f].size(); t++)
		{
			//To evade situations of zero probability we just start by assigning 1 as default value
			temp[featureTypes[f][t]] = 1;
		}
		frequencyOneClass.push_back(temp);
	}
	vector<vector<unordered_map<short, int>>> frequencies(classNum, frequencyOneClass);
	//vector<vector<int>> totalOfFeatureInClass(classNum, vector<int>(numFeatures, 0));
	vector<int> numberOfSamplesOfClass(classNum);
	/*
	for (short c = 0; c < probs.size(); c++)
	{
		//Get number of 
		for (int i = 0; i < data.size(); i++)
		{
			if (data[i][0] == c)
			{
				for (int j = 1; j < rowSize; j++)
				{

				}
			}
		}
	}
	*/

	for (int i = 0; i < data.size(); i++)
	{
		++numberOfSamplesOfClass[data[i][0]];
		//short cClass = data[i][0];
		for (int f = 0; f < numFeatures; f++)
		{
			++frequencies[data[i][0]][f][data[i][f + 1]];
			//++totalOfFeatureInClass[data[i][0]][f];
		}
	}

	for (int c = 0; c < classNum; c++)
	{
		classProbs[c] = log(((double)numberOfSamplesOfClass[c]) / ((double)data.size()));
		for (int f = 0; f < numFeatures; f++)
		{
			for (int t = 0; t < featureTypes[f].size(); t++)
			{
				//The P(Feature f=Cathegory/Type t | Class c) = Number of samples with Feature f=Cathegory/Type t of Class c / Number of samples of Class c
				//probs[c][f][t] = ((double)frequencies[c][f][t]) / ((double)numberOfSamplesOfClass[c]);
				probs[c][f][featureTypes[f][t]] = log(((double)frequencies[c][f][featureTypes[f][t]]) / ((double)numberOfSamplesOfClass[c]));
			}
		}
	}
}

void separateData(const vector<vector<short>>& data, vector<vector<short>>& trainingData, vector<vector<short>>& testingData, const int& folds, const int& foldRepetition)
{
	int testingDataSize = data.size() / folds;
	int startingShift = foldRepetition * testingDataSize;

	copy(data.begin(), data.begin() + startingShift, trainingData.begin());
	copy(data.begin() + startingShift, data.begin() + startingShift + testingDataSize, testingData.begin());
	copy(data.begin() + startingShift + testingDataSize, data.end(), trainingData.begin());
}

double validate(vector<vector<unordered_map<short, double>>>& naiveProbs, const vector<double>& classProbs, const vector<vector<short>>& testingData)
{
	int classNum = classProbs.size();
	//int rowSize = testingData[0].size();
	int numFeatures = testingData[0].size() - 1;

	double maxProb;
	double cProb;
	int maxProbClass;
	int guessed = 0;

	for (int i = 0; i < testingData.size(); i++)
	{
		maxProb = -numeric_limits<double>::max();
		for (int c = 0; c < classNum; c++)
		{
			//Utilize that the log values of the probabilities are already calculated
			cProb = classProbs[c];
			for (int f = 0; f < numFeatures; f++)
			{
				//short appp = testingData[i][f + 1];
				cProb += naiveProbs[c][f].at(testingData[i][f + 1]);
			}
			if (cProb > maxProb)
			{
				maxProb = cProb;
				maxProbClass = c;
			}
		}
		if (maxProbClass == testingData[i][0]) ++guessed;
	}
	return (double)guessed / (double)testingData.size();
}

double kFold(const vector<vector<short>>& data, const int& folds, const int& classNum, const vector<vector<short>>& featureTypes)
{
	//pair<short, double> the pair.second is probability that a feature is of type pair.first
	//unordered_map<short, double> for a features
	//vector<unordered_map<short, double>> for all features
	//vector<vector<unordered_map<short, double>>> for all classes
	//vector<unordered_map<short, double>> probsOneClass(classNum, unordered_map<short, double>(featureTypes.size(), make_pair(0,0.0)));
	vector<unordered_map<short, double>> probsOneClass;
	for (int f = 0; f < featureTypes.size(); f++)
	{
		unordered_map<short, double> temp;
		for (int t = 0; t < featureTypes[f].size(); t++)
		{
			temp[featureTypes[f][t]] = 0.0;
		}
		probsOneClass.push_back(temp);
	}
	vector<vector<unordered_map<short, double>>> probs(classNum, probsOneClass);

	vector<double> classProbs(probs.size());

	vector<vector<short>> trainingData(data.size() - data.size() / folds);
	vector<vector<short>> testingData(data.size() / folds);
	double sumOfAccuracies = 0.0;
	double cFoldAccuracy;
	for (int i = 0; i < folds; i++)
	{
		separateData(data, trainingData, testingData, folds, i);
		naiveBayesClassifier(trainingData, probs, classProbs, featureTypes);
		cFoldAccuracy = validate(probs, classProbs, testingData);
		cout << "Accuracy of " << i << " fold is: " << cFoldAccuracy << endl;
		sumOfAccuracies += cFoldAccuracy;
	}

	return sumOfAccuracies / folds;
}


void readInput(const string& filename, vector<vector<short>>& data, vector<int>& missingData, vector<vector<int>>& whatsMissing, const int& dataInLine)
{
	fstream fs;
	fs.open(filename, ios::in);
	
	if (fs)
	{
		int k = 0;
		char waste[256];
		bool hasMissing;
		char c;
		vector<short> current;
		current.resize(dataInLine);
		vector<int> cMissing;

		while (!fs.eof())
		{
			//cout << (char)fs.peek();
			hasMissing = false;
			fs >> c;
			//cout << "G:" << fs.tellg() << " ";
			//cout << (char)fs.peek();
			if (c == 'd')
			{
				//Get democrat
				current[0] = 0;

				//if(k>=424) fs.seekg(6, fs.cur);
				//else if (k > 0) fs.seekg(7, fs.cur);
				//else fs.seekg(8, fs.cur);
			}
			else if(c == 'r')
			{
				//Get republican
				current[0] = 1;

				//if(k>0) fs.seekg(9, fs.cur);
				//else fs.seekg(10, fs.cur);
			}
			//Skip empty lines
			else continue;
			fs.getline(waste, 256, ',');
			//cout << "G:" << fs.tellg();

			//Get yes or no for other
			for (int i = 1; i < dataInLine; i++)
			{
				//cout << (char) fs.peek();
				fs >> c;
				if (c == '?')
				{
					current[i] = missingValue;
					hasMissing = true;
					cMissing.push_back(i);
				}
				else if (c == 'y') current[i] = 1;
				else if (c == 'n') current[i] = 0;
				else
				{
					cout << "ERROR FILE NOT PROPERLY FORMATTED!";
					exit(1);
				}
				fs.get();
			}
			//cout << endl;
			data.push_back(current);
			if (hasMissing)
			{
				missingData.push_back(k);
				whatsMissing.push_back(cMissing);
				cMissing.clear();
			}
			k++;
		}
		data.shrink_to_fit();
		missingData.shrink_to_fit();
		fs.close();
		//data.pop_back();
	}
	else
	{
		cout << "ERROR LOADING INPUT FILE!";
		exit(1);
	}
}

int main()
{
	//char* filename = new char[strlen("house-votes-84.data")+1];
	//strcpy(filename, "house-votes-84.data");
	//char filename[] = "house-votes-84.data";
	string filename = "D:/Programming/AI/NaiveBayesClassifier/Debug/house-votes-84.data";
	cout << "Reading data from: " << filename << endl;

	constexpr int maxMissing = 5;
	constexpr int dataInLine = 17;
	constexpr int classNum = 2;
	vector<vector<short>> featureTypes(dataInLine - 1, vector<short>({ 0, 1 }));

	vector<vector<short>> data;
	vector<int> missingData;
	vector<vector<int>> whatsMissing;

	readInput(filename, data, missingData, whatsMissing, dataInLine);
	fillMissing(data, missingData, whatsMissing, featureTypes, maxMissing);

	/*for (int i = 0; i < data.size(); i++)
	{
		for (int j = 0; j < dataInLine; j++)
		{
			cout << data[i][j] << " ";
		}
		cout << endl;
	}*/

	constexpr int folds = 10;
	double resultAccuracy = kFold(data, folds, classNum, featureTypes);
	cout << "Average accuracy is: " << resultAccuracy << endl;


	cin.get();
	return 0;
}