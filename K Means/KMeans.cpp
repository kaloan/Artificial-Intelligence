#include "KMeans.h"
#include<fstream>
#include<iostream>
#include<random>
#include<chrono>
#include<limits>

KMeans::KMeans()
{
	setDimensions(2);
	setK(3);
	setStiffness(0.0);
	setRestartProbability(0.0);
	setSoftLaw('e');
}

KMeans::KMeans(const unsigned int &dims, const unsigned int &k, const double &stiff, const char&law) 
{
	setDimensions(dims);
	setK(k);
	setStiffness(stiff);
	setSoftLaw(law);
}

KMeans::KMeans(const KMeans &otherKMeans)
{
	setDimensions(otherKMeans.dimensions);
	setK(otherKMeans.K);
	setStiffness(otherKMeans.stiffness);
	setData(otherKMeans.data);
	setSoftLaw(otherKMeans.softLaw);
}

KMeans::KMeans(KMeans &&otherKMeans)
{
	dimensions = otherKMeans.dimensions;
	K = otherKMeans.K; 
	stiffness = otherKMeans.stiffness;
	data = move(otherKMeans.data);
	dataToCentersSoft = move(otherKMeans.dataToCentersSoft);
	dataToCentersHard = move(otherKMeans.dataToCentersHard);
	clustersHard = move(otherKMeans.clustersHard);
	centers = move(otherKMeans.centers);
}


KMeans::~KMeans()
{

}

KMeans & KMeans::operator=(KMeans &&otherKMeans)
{
	// TODO: insert return statement here
	if (this != &otherKMeans)
	{
		dimensions = otherKMeans.dimensions;
		K = otherKMeans.K;
		stiffness = otherKMeans.stiffness;
		data = move(otherKMeans.data);
		dataToCentersSoft = move(otherKMeans.dataToCentersSoft);
		dataToCentersHard = move(otherKMeans.dataToCentersHard);
		clustersHard = move(otherKMeans.clustersHard);
		centers = move(otherKMeans.centers);
	}
	return *this;
}

void KMeans::setDimensions(const unsigned int &dims)
{
	dimensions = dims;
}

void KMeans::setK(const unsigned int &k)
{
	K = k;
}

void KMeans::setStiffness(const double &stiff)
{
	stiffness = stiff;
}

void KMeans::setRestartProbability(const double &restartProb)
{
	restartProbability = restartProb;
}

void KMeans::setData(const vector<vector<double>>&otherData)
{
	data.clear();
	data.assign(otherData.begin(), otherData.end());
}

void KMeans::setSoftLaw(const char &law)
{
	softLaw = law;
}

void KMeans::readDataFromFile(const string &filePath)
{
	//Open file
	fstream inputFile(filePath, fstream::in);
	if (!inputFile)
	{
		cout << "Error opening Kmeans input file!" << endl;
		return;
	}

	//Read file
	vector<double> specificCoords(dimensions,0.0);
	while (!inputFile.eof())
	{
		for (int i = 0; i < dimensions; i++) inputFile >> specificCoords[i];
		data.push_back(specificCoords);
	}
	data.shrink_to_fit();
}

void KMeans::writeClusteringToFile(const string &givenDirectory)
{
	//Open files
	const string directory = givenDirectory.back() == '/' ? givenDirectory : givenDirectory + "/";

	const string clusteringPath = directory + "clusters.txt";
	fstream clusteringFile(clusteringPath, fstream::out);
	if (!clusteringFile)
	{
		cout << "Error creating clustering file!" << endl;
		return;
	}
	const string centersPath = directory + "centers.txt";
	fstream centersFile(centersPath, fstream::out);
	if (!centersFile)
	{
		cout << "Error creating centers file!" << endl;
		return;
	}
	const string kPath = directory + "K.txt";
	fstream kFile(kPath, fstream::out);
	if (!kFile)
	{
		cout << "Error creating K file!" << endl;
		return;
	}


	//Transform hard to soft clustering if necessary
	if (dataToCentersHard.size() > 0)
	{
		dataToCentersSoft.clear();
		dataToCentersSoft.resize(dataToCentersHard.size(), vector<double>(K, 0.0));

		for (int i = 0; i < dataToCentersHard.size(); i++) dataToCentersSoft[i][dataToCentersHard[i]] = 1.0;
		/*for (int i = 0; i < dataToCentersHard.size(); i++)
		{
			dataToCentersSoft[i][dataToCentersHard[i]] = 0.0;
		}*/
	}

	//Write clustering
	for (const auto& softAssignment : dataToCentersSoft)
	{
		for (const auto& assignmentToCenter : softAssignment)
		{
			clusteringFile << assignmentToCenter << " ";
		}
		clusteringFile << endl;
	}
	clusteringFile.close();


	//Write centers
	for (const auto& center : centers)
	{
		for (const auto& coords : center)
		{
			centersFile << coords << " ";
		}
		centersFile << endl;
	}
	centersFile.close();

	//Write K
	kFile << K << endl;
	kFile.close();
}

void KMeans::initCenters()
{
	default_random_engine rng(chrono::system_clock::now().time_since_epoch().count());
	uniform_int_distribution<int> firstPick(0, data.size());
	centers.push_back(data[firstPick(rng)]);
	//vector<double> minDistsToCenter(data.size());
	vector<double> minDistsToCenter(data.size(),numeric_limits<double>::max());
	double accumulatedDistances;
	//vector<double> accumulatedDistances();
	vector<int> chosenCenters;

	for (int i = 1; i < K; i++)
	{
		accumulatedDistances = 0.0;
		//Find the squares of distances and accumulate them
		for (int s=0; s<data.size(); s++)
		{
			double& minDistToCenter = minDistsToCenter[s];
			//Check if the center has already been chosen
			/*for (const auto& chosen : chosenCenters)
			{
				if (s == chosen)
				{
					minDistToCenter = 0.0;
					goto afterMaxSearch;
				}
			}
			*/
			for (const auto& center : centers)
			{
				double dToACenter = distSQ(center, data[s]);
				//if (dToACenter > maxDistToCenter) maxDistToCenter = dToACenter;
				if (dToACenter < minDistToCenter) minDistToCenter = dToACenter;
			}

			afterMaxSearch:
			accumulatedDistances += minDistToCenter;
		}

		//Choose the new center
		//uniform_real_distribution<double> newCenterPick(numeric_limits<double>::min(), 1.0);
		uniform_real_distribution<double> newCenterPick(numeric_limits<double>::min(), accumulatedDistances);
		double randomNewCenterPick = newCenterPick(rng);
		double currentAccumulation = 0.0;
		for (int s = 0; s < minDistsToCenter.size(); s++)
		{
			currentAccumulation += minDistsToCenter[s];
			//if (randomNewCenterPick <= currentAccumulation / accumulatedDistances)
			if (randomNewCenterPick <= currentAccumulation)
			{
				centers.push_back(data[s]);
				break;
			}
		}
	}
}

void KMeans::recalculateCentersHard()
{
	for (int clusterIndex=0; clusterIndex<K; clusterIndex++)
	{
		auto& cluster = clustersHard[clusterIndex];
		vector<double> newCenterCoords(dimensions, 0.0);
		for (const auto& pointIndex : cluster)
		{
			for (int i = 0; i < dimensions; i++) newCenterCoords[i] += data[pointIndex][i];
		}
		for (auto& coordinate : newCenterCoords) coordinate /= cluster.size();
		centers[clusterIndex] = move(newCenterCoords);
	}
}

void KMeans::recalculateCentersSoft()
{
	vector<vector<double>> newCenters(K, vector<double>(dimensions, 0.0));
	vector<double> denoms(K, 0.0);
	for (int i=0; i<data.size(); i++)
	{
		auto& pointFuzzyAssignment = dataToCentersSoft[i];
		for (int multiplierIndex = 0; multiplierIndex < K; multiplierIndex++)
		{
			auto& multiplier = pointFuzzyAssignment[multiplierIndex];
			for (int coordIndex = 0; coordIndex < dimensions; coordIndex++)
			{
				newCenters[multiplierIndex][coordIndex] += multiplier * data[i][coordIndex];
			}
			denoms[multiplierIndex] += multiplier;
		}
	}
	
	for (int i = 0; i < K; i++)
	{
		for (int j = 0; j < dimensions; j++)
		{
			newCenters[i][j] /= denoms[i];
		}
	}
	centers = move(newCenters);
}

void KMeans::clusterisationHard()
{
	dataToCentersHard.resize(data.size(), 0);
	clustersHard.resize(K, vector<int>());
	bool change = true;
	double defaultMin = numeric_limits<double>::max();
	uniform_real_distribution<double> testRestart(0.0, 1.0);
	default_random_engine rng(chrono::system_clock::now().time_since_epoch().count());

	do
	{
		change = false;
		vector<int> newAssignments(dataToCentersHard.size(), 0);
		for (auto& cluster : clustersHard) cluster.clear();

		for (int i=0; i < dataToCentersHard.size(); i++)
		{
			double minDist = defaultMin;
			int& centerAssignment = newAssignments[i];
			for (int c = 0; c < K; c++)
			{
				double cDist = distSQ(data[i], centers[c]);
				if (cDist < minDist)
				{
					minDist = cDist;
					centerAssignment = c;
				}
			}
			//Add to the new center's corresponing cluster
			clustersHard[centerAssignment].push_back(i);
		}
		
		for (int i = 0; i < dataToCentersHard.size(); i++)
		{
			if (dataToCentersHard[i] != newAssignments[i])
			{
				change = true;
				break;
			}
		}

		recalculateCentersHard();
		dataToCentersHard = move(newAssignments);
	} while (change && (testRestart(rng) >= restartProbability));
}

void KMeans::clusterisationSoft()
{
	dataToCentersSoft.resize(data.size(), vector<double>(K, 0.0));
	bool change = true;
	double defaultMin = numeric_limits<double>::max();
	constexpr double accuracy = 0.00001;
	uniform_real_distribution<double> testRestart(0.0, 1.0);
	default_random_engine rng(chrono::system_clock::now().time_since_epoch().count());

	do
	{
		change = false;
		vector<vector<double>> newAssignments(dataToCentersSoft.size(), vector<double>(K, 0.0));

		for (int p = 0; p < dataToCentersSoft.size(); p++)
		{
			double sumOfProbs=0.0;
			for (int c = 0; c < K; c++)
			{
				switch(softLaw)
				{
				case 'e':
					newAssignments[p][c] = exp(-stiffness * sqrt(distSQ(data[p], centers[c])));
					break;
				default:
					double distToC = distSQ(data[p], centers[c]);
					newAssignments[p][c] = distToC == 0 ? 1 : 1 / (stiffness*distToC);
					break;
				}
				sumOfProbs += newAssignments[p][c];
			}
			if (sumOfProbs != 0.0)
			{
				for (int c = 0; c < K; c++)
				{
					newAssignments[p][c] /= sumOfProbs;
				}
			}
			else
			{
				int closestC;
				double minDist = numeric_limits<double>::max();
				for (int c = 0; c < K; c++)
				{
					double distToC = distSQ(data[p], centers[c]);
					if (distToC < minDist)
					{
						minDist = distToC;
						closestC = c;
					}
				}
				newAssignments[p][closestC] = 1;
			}
		}

		for (int p = 0; p < dataToCentersSoft.size(); p++)
		{
			for (int c = 0; c < K; c++)
			{
				if (abs(dataToCentersSoft[p][c] - newAssignments[p][c]) > accuracy)
				{
					change = true;
					goto Recalculation;
				}
			}
		}

		Recalculation:
		dataToCentersSoft = move(newAssignments);
		recalculateCentersSoft();	
	} while (change && (testRestart(rng) >= restartProbability));
}

double KMeans::distSQ(const vector<double>& point1, const vector<double>& point2)
{
	double res = 0.0;
	double cCoord;

	for (int i = 0; i < dimensions; i++)
	{
		cCoord = point1[i] - point2[i];
		res += cCoord * cCoord;
	}

	return res;
}

double KMeans::calculateFitnessHard()
{
	double res = 0.0;
	for (int c = 0; c < K; c++)
	{
		for (const auto& clusterPointIndex : clustersHard[c])
		{
			res += distSQ(centers[c], data[clusterPointIndex]);
		}
	}
	return res;
}

double KMeans::calculateFitnessSoft()
{
	double res=0.0;
	for (int i = 0; i < data.size(); i++)
	{
		const auto& point = data[i];
		const auto& pointFuzzyAssignment = dataToCentersSoft[i];
		for (int multiplierIndex = 0; multiplierIndex < K; multiplierIndex++)
		{
			const auto& multiplier = pointFuzzyAssignment[multiplierIndex];
			res += multiplier * distSQ(centers[multiplierIndex], point);
		}
	}
	return res;
}

double KMeans::calculateFitness()
{
	if (stiffness == 0.0) return calculateFitnessHard();
	return calculateFitnessSoft();
}

double KMeans::AIC()
{
	if (stiffness == 0.0) return data.size()*log(calculateFitnessHard() / data.size()) + 2 * K + (2*K*K+2*K) / (data.size() - K - 1);
	return data.size()*log(calculateFitnessSoft() / data.size()) + 2 * K + (2 * K*K + 2 * K) / (data.size() - K - 1);
}

double KMeans::BIC()
{
	if (stiffness == 0.0) return data.size()*log(calculateFitnessHard() / data.size()) + log(data.size()) *  K;
	return data.size()*log(calculateFitnessSoft() / data.size()) + log(data.size())  *  K;
}

void KMeans::clusterise()
{
	if (K == 0) XMeansClusterise("AIC");
	else 
	{
		initCenters();
		if (stiffness != 0.0) clusterisationSoft();
		else clusterisationHard();
	}
}

void KMeans::randomRestartClusterise(const unsigned int &restartCount, const double &restartProbability)
{
	double minFitness = numeric_limits<double>::max();

	for (int i = 0; i < restartCount; i++)
	{
		//KMeans attempt(*this);
		//KMeans attempt(move(*this));
		KMeans attempt(this->dimensions, this->K, this->stiffness, this->softLaw);
		attempt.data = move(this->data);
		attempt.setRestartProbability(restartProbability);
		attempt.clusterise();
		double attemptFitness = attempt.calculateFitness();
		cout << "Fitness #" << i << ": " << attemptFitness << endl;
		if (attemptFitness < minFitness)
		{
			minFitness = attemptFitness;
			*this = move(attempt);
		}
		else this->data = move(attempt.data);
	}
}

void KMeans::XMeansClusterise(const string &criterion)
{
	double minFitness = numeric_limits<double>::max();
	int maxAttempt = (int)(ceil(log(data.size())*log(data.size())));

	for (int i = 1; ; i++)
	{
		//KMeans attempt(*this);
		//KMeans attempt(move(*this));
		KMeans attempt(this->dimensions, this->K, this->stiffness, this->softLaw);
		attempt.data = move(this->data);
		attempt.setRestartProbability(restartProbability);
		attempt.setK(i);
		attempt.randomRestartClusterise(5,0.0);
		double attemptFitness = criterion=="AIC"?attempt.AIC():attempt.BIC();
		cout << "Criterion value for " << i << " centers: " << attemptFitness << endl;
		//if (attemptFitness < minFitness)
		if(abs(minFitness/attemptFitness) > (1/0.98))
		{
			minFitness = attemptFitness;
			*this = move(attempt);
		}
		else
		{
			this->data = move(attempt.data);
			break;
		}
	}
}
