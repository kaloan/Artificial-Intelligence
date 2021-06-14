#include "Population.h"


int main()
{
	cout.precision(numeric_limits<double>::digits10);

	//Test default constructor
	/*
	cout << "Default:" << endl;
	Population tester;
	tester.printPoints();
	cout << endl;
	*/

	//Test param constructor
	
	double maximum = 100000000.0;
	int population = 50000;
	int points;
	cin >> points;

	//Seems like a good approximation of the required generations to get close to the optimal solution
	int gens = (int) (1.5*points);
	//int gens = 100;

	vector<char> crossovers({ 'o' , 't' , 'p', 'c' });
	vector<char> mutations({ 's' , 'i' , 'r' });
	
	
	//cout << "Parametric:" << endl;
	Population tester2(maximum, population, points, gens, crossovers, mutations);
	//tester2.printPoints();
	//vector<pair<double, double>> planePoints = tester2.getPoints();

	//Test mutations and length
	/*
	//Test permutation and printing
	cout << "A permutation: " << endl;
	vector<int> perm = Population::permutePoints(points);
	vector<pair<double, double>> planePoints = tester2.getPoints();
	Population::printGenotype(perm,planePoints);
	cout << endl;
	
	//Test single mutation
	cout << "After mutation : " << endl;
	tester2.insertionMutation(perm, 0, 1);
	Population::printGenotype(perm, planePoints);

	//Test path length
	cout << "Path length for the last permutation is: " << tester2.pathLength(perm) << endl;

	//Test mutation choice
	cout << "After mutation : " << endl;
	tester2.mutate(perm,0, 0, 3);
	Population::printGenotype(perm, planePoints);
	*/

	//Test crossovers
	/*
	vector<int> child1;
	vector<int> child2;
	cout << "Parent1 : " << endl;
	vector<int> permT1=Population::permutePoints(points);
	Population::printGenotype(perm1, planePoints);
	cout << "Parent2 : " << endl;
	vector<int> permT2=Population::permutePoints(points);
	Population::printGenotype(perm2, planePoints);


	int myints1[] = { 0,1,2,3,4,5,6 };
	int myints2[] = { 6,5,1,3,2,4,0 };
	permT1.assign(myints1, myints1 + 7);
	Population::printGenotype(permT1, planePoints);
	permT2.assign(myints2, myints2 + 7);
	Population::printGenotype(permT2, planePoints);

	tester2.twoPointCrossover(permT1, permT2, child1,child2);
	cout << "Child permutaion 1: " << endl;
	Population::printGenotype(child1, planePoints);

	cout << "Child permutaion 2: " << endl;
	Population::printGenotype(child2, planePoints);
	*/

	/*
	vector<int> child1(10);
	vector<int> child2(10);
	tester2.cycleCrossover({8, 4, 7 , 3, 6, 2, 5, 1, 9, 0}, {0, 1 ,2, 3, 4, 5, 6, 7, 8, 9}, child1, child2);
	Population::printGenotype(child1, planePoints);
	Population::printGenotype(child2, planePoints);
	*/
	//tester.evolution();

	tester2.evolution();
	int pppp;
	cin >> pppp;
	return 0;
}