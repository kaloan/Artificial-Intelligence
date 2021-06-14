#pragma once
#include<vector>
#include<string>

using namespace std;

class KMeans
{
private:
	unsigned int dimensions; //Dimensionality
	unsigned int K; //Number of centers
	double stiffness; //Parameter used in soft Kmeans
	vector<vector<double>> data; //The data
	vector<vector<double>> dataToCentersSoft; //Maps each data point to the probability vector of it belonging to the vector of  centers (with hard K-means a point has only 1 nonzero probability and it is 1.0, and this vector is used only for writing to file)
	vector<int> dataToCentersHard; //Used for hard K-means computation
	//vector<vector<vector<double>>> clusters; //
	vector<vector<int>> clustersHard; //Used to get each center's cluster points' indexes
	vector<vector<double>> centers; //The centers

	//unsigned int restartCount; //The number of random restarts in randomRestartClusterise
	double restartProbability; //The probability that a clusterisation will be restarted on next step
	char softLaw; //Determines the law of fuzzy clustering

public:
	void setDimensions(const unsigned int&);
	void setK(const unsigned int&);
	void setStiffness(const double&);
	void setRestartProbability(const double&);
	void setData(const vector<vector<double>>&);
	void setSoftLaw(const char&);


	void readDataFromFile(const string&);
	void writeClusteringToFile(const string&);


	void initCenters();
	void recalculateCentersHard();
	void recalculateCentersSoft();
	//void recalculateCenters();
	void clusterisationHard();
	void clusterisationSoft();

	double distSQ(const vector<double>&, const vector<double>&);
	double calculateFitnessHard();
	double calculateFitnessSoft();
	double calculateFitness();
	double AIC();
	double BIC();

	void clusterise();
	void randomRestartClusterise(const unsigned int&, const double&);
	void XMeansClusterise(const string&);

	KMeans();
	KMeans(const unsigned int&, const unsigned int&, const double&, const char&);
	KMeans(const KMeans&);
	KMeans(KMeans&&);
	~KMeans();
	KMeans& operator=(KMeans&&);
};

