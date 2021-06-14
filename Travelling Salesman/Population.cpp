#include "Population.h"

void Population::printPoints() const
{
	for (int i = 0; i < numberOfPoints; i++)
	{
		// pi(xi, yi)
		cout << "p" << i << "(" << points[i].first << " , " << points[i].second << ")" << endl;
	}
	cout << endl;
}

void Population::randomizePoints()
{
	//Start the rng
	//uniform_real_distribution<double> UDist(-maxDist, maxDist);
	uniform_real_distribution<double> UDist(0, maxDist);
	uniform_real_distribution<double> UAngle(0, 8 * atan(1.0));
	default_random_engine prodRandVar(chrono::system_clock::now().time_since_epoch().count());

	points.reserve(numberOfPoints);

	for (int i = 0; i < numberOfPoints; i++)
	{
		//points.push_back(make_pair<double, double>(UDist(prodRandVar), UDist(prodRandVar)));
		points.push_back(make_pair<double, double>(UDist(prodRandVar)*cos(UAngle(prodRandVar)), UDist(prodRandVar)*sin(UAngle(prodRandVar))));
	}
}

double Population::pathLength(const vector<int>& genoType) const
{
	//Guard for case of a single point
	if (numberOfPoints == 1) return 0.0;

	double overallLength = 0.0;
	double diffX;
	double diffY;

	//Sum the length of the individual segments
	for (int i = 0; i < numberOfPoints-1; i++)
	{
		diffX = points[genoType[i + 1]].first - points[genoType[i]].first;
		diffY = points[genoType[i + 1]].second - points[genoType[i]].second;
		overallLength += sqrt(diffX * diffX + diffY * diffY);
	}
	return overallLength;
}

vector<int> Population::permutePoints(const int& n)
{
	//Create an array with elements 0 - n-1 in increasing order
	vector<int> permutation;
	permutation.reserve(n);
	for (int i = 0; i < n; i++)
	{
		permutation.push_back(i);
	}

	uniform_int_distribution<int> UPlaceChoice(0, n-1);
	default_random_engine prodRandVar(chrono::system_clock::now().time_since_epoch().count());
	int transpositionOther;

	//Get a permutation by many transpositions
	int temp;
	for (int i = 0; i < n; i++)
	{
		transpositionOther = UPlaceChoice(prodRandVar);
		temp = permutation[i];
		permutation[i] = permutation[transpositionOther];
		permutation[transpositionOther] = temp;
	}

	return permutation;
}

void Population::printGenotype(const vector<int>& genoType,const vector<pair<double, double>>& givenPoints)
{
	for (int i = 0; i < genoType.size(); i++)
	{
		// pi(xi, yi)
		cout << "p" << genoType[i] << "(" << givenPoints[genoType[i]].first << " , " << givenPoints[genoType[i]].second << ")" << endl;
	}
	cout << endl;
}

vector<pair<double, double>> Population::getPoints()
{
	return points;
}

void Population::getPopulationFitness(const vector<vector<int>>& pop, vector<pair<double, int>>& fit) const
{
	//Renew the fitness
	//fit.clear();
	//fit.reserve(pop.size());

	//Insert the pairs of <fitness,index> for the gen types in population
	pair<double, int> tPair;
	for (int i = 0; i < pop.size(); i++)
	{
		tPair.first = pathLength(pop[i]);
		tPair.second = i;
		fit[i] = tPair;
		//fit.push_back(tPair);
	}

	//Sort the fitness
	sort(fit.begin(), fit.end());
}

void Population::mutate(vector<int>& genoType, const int &mut, const int &gene1, const int& gene2) const
{
	//Allows us to manipulate the percentage of mutations by changing the max value for the distribution of mut
	//Allows us to manipulate what percentage of mutations are of what type by changing the number of their occurences in the mutationType
	if (mut < mutationType.size())
	{
		if (gene1 <= gene2)
		{
			if (mutationType[mut] == 's') swapMutation(genoType, gene1, gene2);
			else if (mutationType[mut] == 'i') insertionMutation(genoType, gene1, gene2);
			else reverseMutation(genoType, gene1, gene2);
		}
		else
		{
			if (mutationType[mut] == 's') swapMutation(genoType, gene2, gene1);
			else if (mutationType[mut] == 'i') insertionMutation(genoType, gene2, gene1);
			else reverseMutation(genoType, gene2, gene1);
		}
	}
}

void Population::produceOffspring(const vector<int>& genoType1, const vector<int>& genoType2, vector<int>& child1, vector<int>& child2, const int& crossType, const int& pos1, const int&pos2) const
{
	if (crossoverType[crossType] == 'o') onePointCrossover(genoType1, genoType2, child1, child2, pos1);
	else if (crossoverType[crossType] == 't')
	{
		if(pos1<=pos2) twoPointCrossover(genoType1, genoType2, child1, child2, pos1, pos2);
		else twoPointCrossover(genoType1, genoType2, child1, child2, pos2, pos1);
	}
	else if (crossoverType[crossType] == 'p')
	{
		if (pos1 <= pos2) partiallyMappedCrossover(genoType1, genoType2, child1, child2, pos1, pos2);
		else partiallyMappedCrossover(genoType1, genoType2, child1, child2, pos2, pos1);
	}
	else cycleCrossover(genoType1, genoType2, child1, child2);
}

void Population::swapMutation(vector<int>& genoType, const int &gene1, const int &gene2) const
{
	//Just swap the values at indexes gene1 and gene2
	int temp = genoType[gene1];
	genoType[gene1] = genoType[gene2];
	genoType[gene2] = temp;
}

void Population::insertionMutation(vector<int>& genoType, const int &gene1, const int &gene2)  const
{
	//Move all of the points in between, and including the last, one time to the left and place the original at gene1 to index gene2
	int curr = genoType[gene1];
	for (int i = gene1; i < gene2; i++)
	{
		genoType[i] = genoType[i+1];
	}
	genoType[gene2] = curr;
}

void Population::reverseMutation(vector<int>& genoType, const int &gene1, const int &gene2)  const
{
	//Reverse order from gene1 to gene2, both inclusive
	int i = gene1;
	int j = gene2;
	int temp;

	while (i < j)
	{
		temp = genoType[i];
		genoType[i++] = genoType[j];
		genoType[j--] = temp;
	}
}

void Population::onePointCrossover(const vector<int>& genoType1, const vector<int>& genoType2, vector<int>& child1, vector<int>& child2, const int &newPartStart)  const
{
	//Allow for copying the parents 

	for (int i = 0; i < newPartStart; i++)
	{
		child1[i] = genoType1[i];
		child2[i] = genoType2[i];
	}
	int child1Fill = newPartStart;
	int child2Fill = newPartStart;
	
	bool alreadyIn;

	//Fill first child from second parent
	for (int i = 0; i < numberOfPoints; i++)
	{
		alreadyIn = false;
		if (child1Fill == numberOfPoints) break;
		for (int j = 0; j < newPartStart; j++)
		{
			if (genoType2[i] == genoType1[j])
			{
				alreadyIn = true;
				break;
			}
		}
		if (!alreadyIn) child1[child1Fill++] = genoType2[i];
	}

	//Fill second child from first parent
	for (int i = 0; i < numberOfPoints; i++)
	{
		alreadyIn = false;
		if (child2Fill == numberOfPoints) break;
		for (int j = 0; j < newPartStart; j++)
		{
			if (genoType1[i] == genoType2[j])
			{
				alreadyIn = true;
				break;
			}
		}
		if (!alreadyIn) child2[child2Fill++] = genoType1[i];
	}
}

void Population::twoPointCrossover(const vector<int>& genoType1, const vector<int>& genoType2, vector<int>& child1, vector<int>& child2, const int& smallerPos, const int &largerPos)  const
{

	//Middle part is indexes in [smallerPos,largerPos]
	
	//Copy in between
	for (int i = smallerPos; i <= largerPos; i++)
	{
		child1[i] = genoType1[i];
		child2[i] = genoType2[i];
	}

	int cChild1Pos = largerPos + 1;
	int cChild2Pos = cChild1Pos;
	bool alreadyIn;

	//cout << "1SP: " << smallerPos << " LP: " << largerPos << endl ;

	//Fill second child
	for (int i = largerPos + 1;; i++)
	{
		alreadyIn=false;
		if (cChild2Pos == numberOfPoints) cChild2Pos = 0;
		if (cChild2Pos == smallerPos) break;

		if (i == numberOfPoints) i = 0;

		for (int j = smallerPos; j <= largerPos; j++)
		{
			if (genoType1[i] == genoType2[j])
			{
				alreadyIn = true;
				break;
			}
		}
		if (!alreadyIn)
		{
			child2[cChild2Pos++] = genoType1[i];
		}
	}

	//cout << "2SP: " << smallerPos << " LP: " << largerPos;

	//Fill first child
	for (int i = largerPos + 1;; i++)
	{
		alreadyIn = false;
		if (cChild1Pos == numberOfPoints) cChild1Pos = 0;
		if (cChild1Pos == smallerPos) break;
		if (i == numberOfPoints) i = 0;

		for (int j = smallerPos; j <= largerPos; j++)
		{
			if (genoType2[i] == genoType1[j]) 
			{
				alreadyIn = true;
				break;
			}
		}
		if (!alreadyIn)
		{
			child1[cChild1Pos++] = genoType2[i];
		}
	}
}

void Population::partiallyMappedCrossover(const vector<int>& genoType1, const vector<int>& genoType2, vector<int>& child1, vector<int>& child2, const int& smallerPos, const int &largerPos)  const
{

	//Swap the parts
	for (int i = 0; i < numberOfPoints; i++)
	{
		if (i >= smallerPos && i <= largerPos)
		{
			child1[i] = genoType1[i];
			child2[i] = genoType2[i];
		}
		else
		{
			child1[i] = genoType2[i];
			child2[i] = genoType1[i];
		}
	}

	bool alreadyIn;
	int cyclePos;
	
	//Fill first child
	for (int i = smallerPos; i<=largerPos; i++)
	{
		alreadyIn = false;

		for (int j = smallerPos; j <= largerPos; j++)
		{
			if (genoType2[i] == genoType1[j])
			{
				alreadyIn = true;
				break;
			}
		}

		if (!alreadyIn)
		{
			cyclePos = i;

			while (cyclePos >= smallerPos && cyclePos <= largerPos)
			{
				for (int j = 0; j < numberOfPoints; j++)
				{
					if (genoType2[j] == genoType1[cyclePos])
					{
						cyclePos = j;
						break;
					}
				}
			}

			child1[cyclePos] = genoType2[i];
		}
	}
	
	//Fill second child
	for (int i = smallerPos; i <= largerPos; i++)
	{
		alreadyIn = false;

		for (int j = smallerPos; j <= largerPos; j++)
		{
			if (genoType1[i] == genoType2[j])
			{
				alreadyIn = true;
				break;
			}
		}

		if (!alreadyIn)
		{
			cyclePos = i;

			while (cyclePos >= smallerPos && cyclePos <= largerPos)
			{
				for (int j = 0; j < numberOfPoints; j++)
				{
					if (genoType1[j] == genoType2[cyclePos])
					{
						cyclePos = j;
						break;
					}
				}
			}

			child2[cyclePos] = genoType1[i];
		}
	}
}

void Population::cycleCrossover(const vector<int>& genoType1, const vector<int>& genoType2, vector<int>& child1, vector<int>& child2)  const
{
	int cStart = 0;
	bool placeFirstInFirst = true;
	int currentGenoIndex;
	vector<bool> alreadyChecked(numberOfPoints, false);
	
	while (cStart != numberOfPoints)
	{
		if (!alreadyChecked[cStart]) 
		{
			currentGenoIndex = cStart;
			alreadyChecked[cStart] = true;
			
			while (genoType1[cStart] != genoType2[currentGenoIndex])
			{
				//Take account of how we are to fill the new genotypes
				if (placeFirstInFirst)
				{
					child1[currentGenoIndex] = genoType1[currentGenoIndex];
					child2[currentGenoIndex] = genoType2[currentGenoIndex];
				}
				else
				{
					child1[currentGenoIndex] = genoType2[currentGenoIndex];
					child2[currentGenoIndex] = genoType1[currentGenoIndex];
				}
				
				for (int i = 0; i < numberOfPoints; i++)
				{
					if (genoType1[i] == genoType2[currentGenoIndex])
					{
						currentGenoIndex = i;
						alreadyChecked[i] = true;
						break;
					}
				}
			}

			//Doesn't place the last elements of the cycle in the loop
			if (placeFirstInFirst)
			{
				child1[currentGenoIndex] = genoType1[currentGenoIndex];
				child2[currentGenoIndex] = genoType2[currentGenoIndex];
			}
			else
			{
				child1[currentGenoIndex] = genoType2[currentGenoIndex];
				child2[currentGenoIndex] = genoType1[currentGenoIndex];
			}
			placeFirstInFirst = !placeFirstInFirst;
		}
		cStart++;
	}
}

void Population::produceNextGeneration(const vector<vector<int>>& population, vector<vector<int>>& newGeneration) const
{
	uniform_int_distribution<int> UIndex(0, populationSize - 1);
	uniform_int_distribution<int> UPermutationLocation(0, numberOfPoints - 1);
	uniform_int_distribution<int> UBestIndex(0, populationSize / 10);
	uniform_int_distribution<int> UCrossover(0, crossoverType.size() - 1);
	uniform_int_distribution<int> UMutation(0, 15 * mutationType.size() - 1);
	default_random_engine rng(chrono::system_clock::now().time_since_epoch().count());
	//Gives a geometric distribution with P(X>=populationSize) <= 1-sqrt(1 - migrationPerc) so that the migration percentage is migrationPerc
	//double migrationPerc = 0.05;
	//geometric_distribution<int> GeoIndex(1.0 - pow(1.0 - sqrt(1.0 - migrationPerc), 1.0 / (populationSize - 1)));

	int randIndex;
	int randIndex2;
	int newGenerationIndex;
	
	//Ensure that the current best will remain
	newGeneration[0] = population[0];
	newGeneration[1] = population[1];
	newGenerationIndex = 2;


	//Make sure the best ones produce children
	for (int i = 0; i < populationSize / 10; i++)
	{
		do
		{
			randIndex = UBestIndex(rng);
		} while (randIndex == i);
		produceOffspring(population[i], population[randIndex], newGeneration[newGenerationIndex], newGeneration[newGenerationIndex + 1], UCrossover(rng), UPermutationLocation(rng), UPermutationLocation(rng));
		newGenerationIndex+=2;

		if (i < populationSize / 20)
		{
			produceOffspring(population[0], population[i], newGeneration[newGenerationIndex], newGeneration[newGenerationIndex + 1], UCrossover(rng), UPermutationLocation(rng), UPermutationLocation(rng));
			newGenerationIndex += 2;
		}
	}

	//Fill the rest of the new generation
	while (newGenerationIndex < 2 * populationSize - populationSize/5)
	{
		//Ensure atleast one of the genotypes isn't with very low fitness
		do
		{
			randIndex = UIndex(rng);
		} while (randIndex > 8 * populationSize / 10);
		randIndex2 = UBestIndex(rng);

		produceOffspring(population[randIndex], population[randIndex2], newGeneration[newGenerationIndex], newGeneration[newGenerationIndex + 1], UCrossover(rng), UPermutationLocation(rng), UPermutationLocation(rng));
		newGenerationIndex += 2;
	}

	while (newGenerationIndex < 2 * populationSize)
	{
		//Add a bit of immigration
		newGeneration[newGenerationIndex++] = Population::permutePoints(numberOfPoints);
	}
	

	/*
	//Using a geometric distribution gives an edge to ones with more fitness
	while (newGenerationIndex < 2 * populationSize)
	{
		randIndex = GeoIndex(rng);
		randIndex2 = GeoIndex(rng);
		if(randIndex < populationSize && randIndex2 < populationSize) produceOffspring(population[randIndex], population[randIndex2], newGeneration[newGenerationIndex], newGeneration[newGenerationIndex + 1], UCrossover(rng), UPermutationLocation(rng), UPermutationLocation(rng));
		else
		{
			newGeneration[newGenerationIndex] = Population::permutePoints(numberOfPoints);
			newGeneration[newGenerationIndex + 1] = Population::permutePoints(numberOfPoints);
		}
		newGenerationIndex+=2;
	}
	*/


	//Mutate but make sure the old best are left
	for (int i = 2; i < 2 * populationSize; i++)
	{
		mutate(newGeneration[i], UMutation(rng), UPermutationLocation(rng), UPermutationLocation(rng));
	}
}

void Population::evolution() const
{
	//Create and fill a population - considered as 1st generation
	vector<vector<int>> population;
	population.resize(populationSize, vector<int>(numberOfPoints, 0));
	vector<pair<double, int>> fitness(populationSize);

	for (int i = 0; i < populationSize; i++)
	{
		population[i] = Population::permutePoints(numberOfPoints);
	}
	getPopulationFitness(population, fitness);

	//Sort the population - in block so the original copy doesn't waste memory
	{
		vector<vector<int>> popForSwitch = population;
		for (int i = 0; i < populationSize; i++)
		{
			population[i] = popForSwitch[fitness[i].second];
		}
	}

	cout << "Best fitness at generation #1: " << fitness[0].first << endl;

	//Allocate space for new generations
	vector<vector<int>> newGeneration(2 * populationSize, vector<int>(numberOfPoints, 0));
	fitness.resize(2 * populationSize);

	//Standard generation loop
	for (int i = 1; i < numberOfGenerations; i++)
	{
		//cout << "Current generation is: " << i << endl;
		if(i % 10 == 0) cout << "Best fitness at generation #" << i <<": " << fitness[0].first << endl;

		//Create the next generation and store in in newGeneration
		produceNextGeneration(population, newGeneration);

		//Take the best and place them in a sorted order
		getPopulationFitness(newGeneration, fitness);
		for (int j = 0; j < populationSize; j++)
		{
			population[j] = newGeneration[fitness[j].second];
		}
	}

	cout << "Best final fitness: " << fitness[0].first << endl;
	//Population::printGenotype(population[0], points);
}

Population::Population()
{
	maxDist = 10000000.0;
	populationSize = 250;
	numberOfPoints = 35;
	numberOfGenerations = 50;

	randomizePoints();
	crossoverType.push_back('o');
	crossoverType.push_back('t');
	crossoverType.push_back('c');
	crossoverType.push_back('p');

	mutationType.push_back('s');
	mutationType.push_back('i');
	mutationType.push_back('r');
}

Population::Population(const double &maxDist, const int &populationSize, const int &numberOfPoints, const int &numberOfGenerations, const vector<char>& crossoverType, const vector<char>& mutationType)
{
	this->maxDist = maxDist;
	this->populationSize = populationSize;
	this->numberOfPoints = numberOfPoints;
	this->numberOfGenerations = numberOfGenerations;
	this->crossoverType = crossoverType;
	this->mutationType = mutationType;
	randomizePoints();
}


Population::~Population()
{

}

