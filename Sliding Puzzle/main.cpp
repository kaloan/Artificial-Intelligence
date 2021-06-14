#include<iostream>
#include<math.h>
#include<stdlib.h>
#include<stack>
#include<utility>
using namespace std;

//correctCoords[i] stores the correct final coordinates of the number i in (row,column) form
pair<int, int>* correctCoords = NULL;

//Structures used to track board states
class state
{
public:
	short** table; //Board
	int cZeroRow; //Current row of empty cell
	int cZeroColumn; //Current column of empty cell
	int evalFunction; //Evaluation function
	int distance; //Heuristic
	

	state()
	{
		table = NULL;
		cZeroRow = -1;
		cZeroColumn = -1;
		evalFunction = -1;
	}

	state(short**& table, const int& cZeroRow, const int& cZeroColumn, const int& evalFunction, const int& distance)
	{
		//No need to recopy the info
		this->table = table;
		this->cZeroRow = cZeroRow;
		this->cZeroColumn = cZeroColumn;
		this->evalFunction = evalFunction;
		this->distance = distance;
	}

	state(const state& otherState)
	{
		cZeroRow = otherState.cZeroRow;
		cZeroColumn = otherState.cZeroColumn;
		//No need to recopy the info
		table = otherState.table;
		evalFunction = otherState.evalFunction;
		distance = otherState.distance;
	}
	
	~state()
	{
		delete table[0];
		delete table;
	}
};


//Correct coordinates
void getCorrectCoordinates(const short& num, const short& size, const int& zeroRow, const int& zeroColumn, int& correctRow, int& correctColumn)
{
	if (num == 0)
	{
		correctRow = zeroRow;
		correctColumn = zeroColumn;
	}
	else 
	{
		//If correct place is after the empty's final linearly
		//Num takes standard place
		correctRow = num / size;
		correctColumn = num % size;

		//If correct place is before the empty's final linearly
		if (correctRow < zeroRow || correctRow == zeroRow && correctColumn <= zeroColumn)
		{
			//Num takes the standard place of num-1
			correctRow = (num - 1) / size;
			correctColumn = (num - 1) % size;
		}
	}
}

//Manhattan distance - coords are (row,col)
int manhattanDistance(const short& num, const short& size, const int& placeRow, const int& placeColumn, const int& zeroRow, const int& zeroColumn)
{
	return abs(placeRow - correctCoords[num].first) + abs(placeColumn - correctCoords[num].second);
}

//Hamming distance - false if not in proper place
int hammingDistance(const short& num, const short& size, const int& placeRow, const int& placeColumn, const int& zeroRow, const int& zeroColumn)
{
	if (placeRow == correctCoords[num].first && placeColumn == correctCoords[num].second) return 0;
	return 1;
}

//Heuristic function
int heuristicFull(short**& table, const short& size,const int& zeroRow, const int& zeroColumn, const char& heuristicType)
{
	int res = 0;
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			if (table[i][j] != 0)
			{
				if(heuristicType=='m') res += manhattanDistance(table[i][j], size, i, j, zeroRow, zeroColumn);
				else res+=hammingDistance(table[i][j], size, i, j,zeroRow,zeroColumn);
			}
		}
	}
	return res;
}



//Find square side from input
short findSize(short& input)
{
	input++;
	short res = -1;
	for (short j = 0; j < input; j++)
	{
		if (j*j == input)
		{
			res = j;
			return res;
		}
	}
	if (res == -1)
	{
		cout << "IMPOSSIBLE SIZE GIVEN!" << endl;
		exit(1);
	}
	return res;
}

//Check necessary condition for solvability of the starting permutation
bool checkPermutation(short**& table, short&size, int& zeroRow)
{
	//Find the starting row of 0
	int initialZeroRow;
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			if (table[i][j] == 0)
			{
				initialZeroRow = i;
				break;
			}
		}
	}

	//Calculate the number of inversions
	int transpositionCount = 0;
	for (int i = 0; i < size*size-1; i++)
	{
		for (int j = i+1; j < size*size; j++)
		{
			if (table[0][i]!=0 && table[0][j]!=0 && table[0][i] > table[0][j])
			{
				transpositionCount++;
			}
		}
	}

	//Check if permutation is solvable
	//Switching to different row changes number of transpositions by a number with the same parity as size-1 and the final permutation has even parity
	if (size % 2 == 0)
	{
		if (transpositionCount % 2 == abs(initialZeroRow-zeroRow) % 2) return true;
		else return false;
	}
	else 
	{
		if (transpositionCount % 2 == 0) return true;
		else return false;
	}

	return false;
}

//Initialization
void input(short& size, short**& table, int& zeroRow, int& zeroColumn, int& cZeroRow, int& cZeroColumn)
{
	//Get the square side
	cin >> size;
	size = findSize(size);

	//Transform linear coordinate of 0 to square coordinate
	int zeroProper;
	cin >> zeroProper;
	zeroProper = zeroProper % (size*size);

	if (zeroProper < 0)
	{
		zeroProper = size * size + zeroProper;
	}

	zeroRow = zeroProper / size;
	zeroColumn = zeroProper % size;


	//Make the board
	table = new short*[size];
	table[0] = new short[size*size];
	for (int i = 1; i < size; i++)
	{
		table[i] = table[i - 1] + size;
	}

	//Fill the board from input
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			cin >> table[i][j];
		}
	}

	//Check if the board is solvable
	if (!checkPermutation(table, size, zeroRow))
	{
		cout << "UNSOLVABLE PUZZLE!" << endl;
		exit(1);
	}
	
	//Find the starting coordinates of the empty cell
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			if (table[i][j] == 0)
			{
				cZeroRow = i;
				cZeroColumn = j;
				break;
				break;
			}
		}
	}

	//Find the correct end coordinates of all numbers
	correctCoords = new pair<int, int>[size*size];
	for (short i = 0; i < size*size; i++)
	{
		int correctRow;
		int correctColumn;
		getCorrectCoordinates(i, size, zeroRow, zeroColumn, correctRow, correctColumn);
		correctCoords[i] = make_pair(correctRow, correctColumn);
	}
}


//Creates a new table for the next possible state
short** move(const short& size, short**& currentTable,int& cZeroRow,int& cZeroColumn, char move)
{
	//Create the next table and copy previous values
	short** nextTable;
	nextTable = new short*[size];
	nextTable[0] = new short[size*size];

	for (int i = 1; i < size; i++)
	{
		nextTable[i] = nextTable[i - 1] + size;
	}
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			nextTable[i][j] = currentTable[i][j];
		}
	}


	//Swap the required cells
	short temp = nextTable[cZeroRow][cZeroColumn];
	if (move == 'u')
	{
		nextTable[cZeroRow][cZeroColumn] = nextTable[cZeroRow - 1][cZeroColumn];
		nextTable[cZeroRow - 1][cZeroColumn] = temp;
	}
	else if (move == 'd')
	{
		nextTable[cZeroRow][cZeroColumn] = nextTable[cZeroRow + 1][cZeroColumn];
		nextTable[cZeroRow + 1][cZeroColumn] = temp;
	}
	else if (move == 'l')
	{
		nextTable[cZeroRow][cZeroColumn] = nextTable[cZeroRow][cZeroColumn - 1];
		nextTable[cZeroRow][cZeroColumn - 1] = temp;
	}
	else if (move == 'r')
	{
		nextTable[cZeroRow][cZeroColumn] = nextTable[cZeroRow][cZeroColumn + 1];
		nextTable[cZeroRow][cZeroColumn + 1] = temp;
	}


	return nextTable;
}


//Check for solution of "depth" <= maxEval
bool IDAStarDepth(const short& size,const state& currentState, const int& zeroRow, const int& zeroColumn, const int& maxEval,const char& heuristicType, stack<char>& path, char lastMove)
{
	/*
	//For debugging
	short** currentTable = currentState.table;
	cout << endl;
	cout << lastMove << endl;
	cout << currentState.evalFunction << endl << currentState.distance << endl;
	
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			cout << currentTable[i][j] << " ";
		}
		cout << endl;
	}
	*/

	//Check if state has evaluation function more than the current maximum
	if (currentState.evalFunction > maxEval)
	{
		//cout << "END OF CURRENT PATH!" << endl;
		return false;
	}
	if (currentState.distance == 0)
	{
		//cout << "SOLUTION!" << endl;
		path.push(lastMove);
		return true;
	}

	//Used for further moves
	int cZeroRow = currentState.cZeroRow;
	int cZeroColumn = currentState.cZeroColumn;
	short** currentTable = currentState.table;
	short** nextTable = NULL;
	int cOtherDist;
	int nOtherDist;
	int newEval;
	int newDistance;

	//Try to move up
	if (cZeroRow > 0 && lastMove != 'd')
	{
		nextTable = move(size, currentTable, cZeroRow, cZeroColumn,'u');

		if (heuristicType == 'm')
		{
			cOtherDist = manhattanDistance(currentTable[cZeroRow - 1][cZeroColumn], size, cZeroRow - 1, cZeroColumn, zeroRow, zeroColumn);
			nOtherDist = manhattanDistance(currentTable[cZeroRow - 1][cZeroColumn], size, cZeroRow, cZeroColumn, zeroRow, zeroColumn);
		}
		else
		{
			cOtherDist = hammingDistance(currentTable[cZeroRow - 1][cZeroColumn], size, cZeroRow - 1, cZeroColumn, zeroRow, zeroColumn);
			nOtherDist = hammingDistance(currentTable[cZeroRow - 1][cZeroColumn], size, cZeroRow, cZeroColumn, zeroRow, zeroColumn);
		}

		newDistance = currentState.distance + nOtherDist - cOtherDist;
		newEval = 1 + currentState.evalFunction + newDistance - currentState.distance;
		state upState(nextTable, cZeroRow-1, cZeroColumn, newEval, newDistance);
		//cout << "GOING UP" << endl;
		if (IDAStarDepth(size, upState, zeroRow, zeroColumn, maxEval, heuristicType, path, 'u'))
		{
			path.push(lastMove);
			return true;
		}
	}

	//Try to move down
	if (cZeroRow < size-1 && lastMove != 'u')
	{
		nextTable = move(size, currentTable, cZeroRow, cZeroColumn, 'd');
		
		if (heuristicType == 'm')
		{
			cOtherDist = manhattanDistance(currentTable[cZeroRow + 1][cZeroColumn], size, cZeroRow + 1, cZeroColumn, zeroRow, zeroColumn);
			nOtherDist = manhattanDistance(currentTable[cZeroRow + 1][cZeroColumn], size, cZeroRow, cZeroColumn, zeroRow, zeroColumn);
		}
		else
		{
			cOtherDist = hammingDistance(currentTable[cZeroRow + 1][cZeroColumn], size, cZeroRow + 1, cZeroColumn, zeroRow, zeroColumn);
			nOtherDist = hammingDistance(currentTable[cZeroRow + 1][cZeroColumn], size, cZeroRow, cZeroColumn, zeroRow, zeroColumn);
		}

		newDistance = currentState.distance + nOtherDist - cOtherDist;
		newEval = 1 + currentState.evalFunction + newDistance - currentState.distance;
		state downState(nextTable, cZeroRow + 1, cZeroColumn, newEval, newDistance);
		//cout << "GOING DOWN" << endl;
		if (IDAStarDepth(size, downState, zeroRow, zeroColumn, maxEval, heuristicType, path, 'd'))
		{
			path.push(lastMove);
			return true;
		}

	}

	//Try to move left
	if (cZeroColumn > 0 && lastMove != 'r')
	{
		nextTable = move(size, currentTable, cZeroRow, cZeroColumn, 'l');
		
		if (heuristicType == 'm')
		{
			cOtherDist = manhattanDistance(currentTable[cZeroRow][cZeroColumn - 1], size, cZeroRow, cZeroColumn - 1, zeroRow, zeroColumn);
			nOtherDist = manhattanDistance(currentTable[cZeroRow][cZeroColumn - 1], size, cZeroRow, cZeroColumn, zeroRow, zeroColumn);
		}
		else
		{
			cOtherDist = hammingDistance(currentTable[cZeroRow][cZeroColumn - 1], size, cZeroRow, cZeroColumn -1, zeroRow, zeroColumn);
			nOtherDist = hammingDistance(currentTable[cZeroRow][cZeroColumn - 1], size, cZeroRow, cZeroColumn, zeroRow, zeroColumn);
		}

		newDistance = currentState.distance + nOtherDist - cOtherDist;
		newEval = 1 + currentState.evalFunction + newDistance - currentState.distance;
		state leftState(nextTable, cZeroRow, cZeroColumn - 1, newEval, newDistance);
		//cout << "GOING LEFT" << endl;
		if (IDAStarDepth(size, leftState, zeroRow, zeroColumn, maxEval, heuristicType, path, 'l'))
		{
			path.push(lastMove);
			return true;
		}
	}

	//Try to move right
	if (cZeroColumn < size - 1 && lastMove != 'l')
	{
		nextTable = move(size, currentTable, cZeroRow, cZeroColumn, 'r');

		if (heuristicType == 'm')
		{
			cOtherDist = manhattanDistance(currentTable[cZeroRow][cZeroColumn + 1], size, cZeroRow, cZeroColumn + 1, zeroRow, zeroColumn);
			nOtherDist = manhattanDistance(currentTable[cZeroRow][cZeroColumn + 1], size, cZeroRow, cZeroColumn, zeroRow, zeroColumn);
		}
		else
		{
			cOtherDist = hammingDistance(currentTable[cZeroRow][cZeroColumn + 1], size, cZeroRow, cZeroColumn + 1, zeroRow, zeroColumn);
			nOtherDist = hammingDistance(currentTable[cZeroRow][cZeroColumn + 1], size, cZeroRow, cZeroColumn, zeroRow, zeroColumn);
		}

		newDistance = currentState.distance + nOtherDist - cOtherDist;
		newEval = 1 + currentState.evalFunction + newDistance - currentState.distance;
		state rightState(nextTable, cZeroRow, cZeroColumn + 1, newEval, newDistance);
		//cout << "GOING RIGHT" << endl;
		if (IDAStarDepth(size, rightState, zeroRow, zeroColumn, maxEval, heuristicType, path, 'r'))
		{
			path.push(lastMove);
			return true;
		}
	}

	//cout << "END OF CURRENT PATH!" << endl;
	return false;
}


//Iterate over the maximum value of the evaluation
void IDAStarMain(const short& size, const state& initialState,const int& zeroRow, const int& zeroColumn,const char& heuristicType)
{
	//cout << "IN IDASTARMAIN FIRSTLINE" << endl;
	//cout << "IN IDASTARMAIN" << endl;
	bool solutionFound=false;
	for (int maxEval = initialState.evalFunction; maxEval <= (size  * size - 1) * 2 * (size - 1); maxEval++)
	{
		//cout << "CURRENT MAX EVAL: " << maxEval << endl;
		stack<char> path;
		if (IDAStarDepth(size, initialState, zeroRow, zeroColumn, maxEval, heuristicType, path, '0'))
		{
			solutionFound = true;
			//cout << "Solution at depth: ";
			//cout << maxEval << endl;
			cout << maxEval;
			if (path.empty())
			{
				cout << "ERROR HAS OCCURED! PATH STACK NOT FILLED!";
				exit(1);
			}
			while(!path.empty())
			{
				//cout << "EMPTYING STACK!" << endl;
				//cout << path.top() << " -- ";
				/*
				if (path.top() == 'l') cout << "left";
				else if (path.top() == 'u') cout << "up";
				else if (path.top() == 'r') cout << "right";
				else if (path.top() == 'd') cout << "down";
				else if (path.top() != '0')
				{
					cout << "ERROR HAS OCCURED! PATH STACK FILLED ERRONEUSLY!";
					exit(1);
				}
				*/
				//Take into account that they are saved according to the movement of the empty cell
				if (path.top() == 'l') cout << "right";
				else if (path.top() == 'u') cout << "down";
				else if (path.top() == 'r') cout << "left";
				else if (path.top() == 'd') cout << "up";
				else if (path.top() != '0')
				{
					cout << "ERROR HAS OCCURED! PATH STACK FILLED ERRONEUSLY!";
					exit(1);
				}
				cout << endl;
				path.pop();
			}
			break;
		}
	}
	if (!solutionFound)	cout << "NO SOLUTION HAS BEEN FOUND!" << endl;

}




//Main
int main()
{
	//Initialization
	short size;
	short** initialTable = NULL;
	int zeroRow;
	int zeroColumn;
	int cZeroRow;
	int cZeroColumn;
	input(size, initialTable, zeroRow, zeroColumn, cZeroRow, cZeroColumn);

	int initialEval = heuristicFull(initialTable, size, zeroRow, zeroColumn,'m');
	state initialState(initialTable, cZeroRow, cZeroColumn, initialEval, initialEval);

	//Search for solution using the IDA* algorithm
	IDAStarMain(size, initialState, zeroRow, zeroColumn,'m');

	int a;
	cin >> a;
	return 0;
}