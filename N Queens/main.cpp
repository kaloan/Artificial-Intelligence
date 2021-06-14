#include<iostream>
#include<time.h>
#include<random>
#include<chrono>
#include<math.h>
#include<thread>

using namespace std;

int sameStart;
int sameLen;

//Find the conflicts of a queen
inline void findConflicts(int& n, int*& c, int*& d1, int*& d2, const int& queenColumn, const int& queenRow, int& conflicts) noexcept
{
	//return c[queenColumn] + d1[n - 1 + queenColumn - queenRow] + d2[2 * n - 2 - queenColumn - queenRow] - 3;
	conflicts = c[queenColumn] + d1[n - 1 + queenColumn - queenRow] + d2[2 * n - 2 - queenColumn - queenRow];
}


//Find the row of the queen with max conflicts
inline void findMaxConflictsRow(int& n, int*& q, int*& c, int*& d1, int*& d2, int& cMaxConflicts, int*& multiplaces) noexcept
{
	int currentConflicts;
	sameStart = 0;
	sameLen = 1;

	for (int i=0; i<n; i++)
	{
		findConflicts(n, c, d1, d2, q[i], i, currentConflicts);
		if (currentConflicts > cMaxConflicts)
		{
			cMaxConflicts = currentConflicts;
			sameStart = sameStart + sameLen;
			multiplaces[sameStart] = i;
			sameLen = 1;
		}
		else if (currentConflicts == cMaxConflicts)
		{
			multiplaces[sameStart + sameLen] = i;
			++sameLen;
		}
	}

}

//Find the column of the queen in maxRow with min conflicts
inline void findMinConflicsCol(int& n, int*& q, int*& c, int*& d1, int*& d2, int& maxRow, int*& multiplaces) noexcept
{
	sameStart = 0;
	sameLen = 1;

	int currentMinConflicts;
	findConflicts(n, c, d1, d2, q[maxRow], maxRow, currentMinConflicts);
	//multiplaces[0] = maxRow;
	multiplaces[0] = q[maxRow];
	int currentConflicts;

	for (int i = 0; i < n; i++)
	{
		//currentConflicts = c[i] + d1[n - 1 + i - maxRow] + d2[2 * n - 2 - i - maxRow];
		//findConflicts(n, c, d1, d2, q[i], i, currentConflicts);
		findConflicts(n, c, d1, d2, i, maxRow, currentConflicts);
		if (currentConflicts < currentMinConflicts)
		{
			currentMinConflicts = currentConflicts;
			sameStart += sameLen;
			//multiplaces[sameStart] = q[i];
			multiplaces[sameStart] = i;
			sameLen = 1;
		}
		else if (currentConflicts == currentMinConflicts)
		{
			//multiplaces[sameStart] = q[i];
			multiplaces[sameStart + sameLen] = i;
			++sameLen;
		}
	}

}



//Initialization
void init(int& n, int*& q, int*& c, int*& d1, int*& d2) noexcept
{
	//Create the board
	q = new int[n];

	//Create and fill the columns array
	c = new int[n];
	for (int i = 0; i < n; i++)
	{
		c[i] = 0;
	}

	//Create and fill the diagonals arrays
	d1 = new int[2 * n - 1];
	d2 = new int[2 * n - 1];
	for (int i = 0; i < 2 * n - 1; i++)
	{
		d1[i] = d2[i] = 0;
	}

	//Fill the board randomly with a single queen on each row
	/*
	char** board = new char*[n];
	board[0] = new char[n*n];
	for (int i = 1; i < n; i++)
	{
		board[i] = board[i - 1] + n;
	}

	srand(time(NULL));
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			board[i][j] = '_';
		}
	}

	for (int i = 0; i < n; i++)
	{
		board[i][rand() % n] = '*';
	}

	return board;
	*/

	srand(time(NULL));
	for (int i = 0; i < n; i++)
	{
		q[i] = rand() % n;
		++c[q[i]];
		++d1[n - 1 + q[i] - i];
		++d2[2 * n - 2 - q[i] - i];
	}
}

inline void init2(int& n, int*& q, int*& c, int*& d1, int*& d2) noexcept
{
	//Zero the columns array
	for (int i = 0; i < n; i++)
	{
		c[i] = 0;
	}

	//Zero the diagonals arrays
	for (int i = 0; i < 2 * n - 1; i++)
	{
		d1[i] = d2[i] = 0;
	}
	
	srand(time(NULL));
	for (int i = 0; i < n; i++)
	{
		q[i] = rand() % n;
		++c[q[i]];
		++d1[n - 1 + q[i] - i];
		++d2[2 * n - 2 - q[i] - i];
	}
}

inline void printBoard(int& n, int*& q) noexcept
{
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < q[i]; j++)
		{
			cout << " _ ";
		}
		cout << " * ";
		for (int j = q[i] + 1; j < n; j++)
		{
			cout << " _ ";
		}
		cout << endl;
	}
	cout << endl;
}

//Main playing for a nxn board
bool solve(int& n, int*& q, int*& c, int*& d1, int*& d2, int*& multiplaces) noexcept
{
	//cout << "RESTARTING!" << endl;
	init2(n, q, c, d1, d2);


	//printBoard(n, q);

	int cMaxConflicts;
	int currQueenToMove;

	//int maxIter = pow(n, 3);
	//int maxIter = n < 1200 ? pow(n, 3) : pow(n,3.5);
	unsigned long long maxIter = n < 20 ? pow(n, 3) : pow(n, 2.3)*log(log(n));

	//int maxIter = pow(n, 2.8);
	//int maxIter = 1000;
	unsigned long long iter = 0;
	int oldQueenColumn;
	while (++iter <= maxIter)
	{
		cMaxConflicts = -1;

		findMaxConflictsRow(n, q, c, d1, d2, cMaxConflicts, multiplaces);
		currQueenToMove = multiplaces[sameStart + rand() % sameLen];
		/*if (iter > log(log(n)))
		{
			findMaxConflictsRow(n, q, c, d1, d2, cMaxConflicts, multiplaces);
			currQueenToMove = multiplaces[sameStart + rand() % sameLen];
		}
		else currQueenToMove = rand() % n;*/

		//if (cMaxConflicts == 0) break;
		if (cMaxConflicts == 3) break;

		oldQueenColumn = q[currQueenToMove];
		//q[currQueenToMove] = findMinConflicsCol(n, q, c, d1, d2, currQueenToMove);
		findMinConflicsCol(n, q, c, d1, d2, currQueenToMove, multiplaces);
		q[currQueenToMove] = multiplaces[sameStart + rand() % sameLen];

		//Decrement the amount of queens at the cols/diagonals taking the old queen position
		--c[oldQueenColumn];
		--d1[n - 1 + oldQueenColumn - currQueenToMove];
		--d2[2 * n - 2 - oldQueenColumn - currQueenToMove];

		//Increment the amount of queens at the cols/diagonals taking the new queen position
		++c[q[currQueenToMove]];
		++d1[n - 1 + q[currQueenToMove] - currQueenToMove];
		++d2[2 * n - 2 - q[currQueenToMove] - currQueenToMove];
	}


	//printBoard(n, q);
	if (cMaxConflicts != 3)
	{
		//solve(n, q, c, d1, d2, multiplaces);
		return false;
	}
	else if (n < 20) printBoard(n, q);
	return true;
	//cin.get();
}


//Main
int main()
{
	/*
	int n;
	char** board = input(n);

	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			cout << board[i][j] << " ";
		}
		cout << endl;
	}
	*/

	//Input size
	int n;
	cin >> n;
	auto start = chrono::high_resolution_clock::now();
	int* multiplaces = new int[n + 2];
	int* q = new int[n]; // Columns of queens
	int* c = new int[n]; // Number for queens per column
	int* d1 = new int[2 * n - 1]; // Main diagonals starting from bottom left
	int* d2 = new int[2 * n - 1]; // Reverse diagonals starting from bottom right

	//auto start = chrono::high_resolution_clock::now();

	//Call the solve
	while(!solve(n, q, c, d1, d2, multiplaces));
	//cout << 5 % 1;
	auto stop = chrono::high_resolution_clock::now();
	auto duration = chrono::duration_cast<chrono::milliseconds>(stop - start);
	cout << "TIME TAKEN: " << (double)duration.count() / 1000.0 << "sec." << endl;

	//cin.get();
	int aaa;
	cin >> aaa;
	return 0;
}