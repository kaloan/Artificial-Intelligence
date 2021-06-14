#include<iostream>
#include<fstream>
#include<math.h>
#include<vector>
#include<string>
#include<iomanip>
#include<filesystem>
#include "Kmeans.h"

using namespace std;



int main()
{
	cout.precision(numeric_limits<double>::digits10);
	/*string a;
	cin >> a;
	cout << endl << a;
	bool check = a == string("normal");
	cout << check;
	double n, m;
	cin >> n >> m;
	*/
	//Choose a file to read
	string dataFilePathNormal = "D:/Programming/AI/KMeans/Release/normal.txt";
	string dataFilePathUnbalance = "D:/Programming/AI/KMeans/Release/unbalance.txt";
	//string dataFilePathNormal = current_path() + "D:/Programming/AI/KMeans/Release/normal.txt";

	cout << "Choose a file - normal or unbalance: ";
	string dataFileChoice;
	bool rightChoice = false;
	do
	{
		cin >> dataFileChoice;
		if (dataFileChoice == "normal")
		{
			dataFileChoice = dataFilePathNormal;
			rightChoice = true;
		}
		else if (dataFileChoice == "unbalance")
		{
			dataFileChoice = dataFilePathUnbalance;
			rightChoice = true;
		}
		else cout << "Not option!" << endl;
	} while (!rightChoice);

	//Input centers
	cout << "Choose number of centers - 0 is XMeans: ";
	unsigned int centerNumber;
	cin >> centerNumber;

	//Information criterion - only for XMeans
	string criterion;
	if (centerNumber == 0)
	{
		cout << "Choose information criterion - AIC or BIC: ";
		cin >> criterion;
	}

	//Input stiffness
	cout << "Choose stiffness - 0 is hard Kmeans: ";
	double stiff = 0.0;
	char law = 'e';
	cin >> stiff;
	if (stiff != 0.0)
	{
		cout << "Choose law - negative exponent (e) or inverse square (i): ";
		cin >> law;
	}


	//Input restarts - only for non Xmeans
	char restarts;
	double restartChance;
	unsigned int restartNum;
	if (centerNumber != 0)
	{
		cout << "Choose to have restarts - y or n: ";
		cin >> restarts;


		if (restarts == 'y')
		{
			cout << "Choose maximum restarts: ";
			cin >> restartNum;
			cout << "Choose restart probability at each new iteration: ";
			cin >> restartChance;
		}
	}


	constexpr unsigned int dim = 2;

	//Init and work
	KMeans clusteriser(dim, centerNumber, stiff, law);
	clusteriser.readDataFromFile(dataFileChoice);
	clusteriser.setSoftLaw(law);
	if (centerNumber == 0) clusteriser.XMeansClusterise(criterion);
	else
	{
		if (restarts == 'y') clusteriser.randomRestartClusterise(restartNum, restartChance);
		else clusteriser.clusterise();
	}


	//Write data to files
	//string clusteringFilePath = "D:/Programming/AI/KMeans/Debug/clusters.txt";
	//string centerFilePath = "D:/Programming/AI/KMeans/Debug/centers.txt";
	//clusteriser.writeClusteringToFile(clusteringFilePath, centerFilePath);

	string outputDir = "D:/Programming/AI/KMeans/Release/";
	clusteriser.writeClusteringToFile(outputDir);
	fstream dataC(outputDir + "datachoice.txt", fstream::out);
	cout << dataFileChoice << endl;
	dataC << dataFileChoice << endl;

	return 0;
}