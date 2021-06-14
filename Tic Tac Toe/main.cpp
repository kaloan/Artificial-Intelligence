#include<iostream>
#include<vector>
#include<utility>
#include<limits>
#include<algorithm>
using namespace std;

//Number of cells per side
constexpr unsigned int boardSize = 3;

//Signs of player and computer
char playerChar;
char computerChar;

int maxValue(vector<vector<char>>& board, int alpha, int beta, int depth, int boardFill);
int minValue(vector<vector<char>>& board, int alpha, int beta, int depth, int boardFill);

void printBoard(const vector<vector<char>>& board)
{
	for (int i = 0; i < board.size(); i++)
	{
		for (int j = 0; j < board[i].size(); j++)
		{
			cout << board[i][j] << " ";
		}
		cout << endl;
	}
}

void playerTurnInput(vector<vector<char>>& board)
{
	int row, col;

	cout << endl << "YOUR TURN!" << endl << "Choose place (2 numbers): " << endl;
	cin >> row >> col;
	cout << endl;
	--row;
	--col;
	if (row >= board.size() || col >= board.size() || row < 0 || col < 0)
	{
		cout << "Impossible coordinates!" << endl;
		playerTurnInput(board);
	}
	else if (board[row][col] != '_')
	{
		cout << "Impossible move! Spot is taken!" << endl;
		playerTurnInput(board);
	}
	else board[row][col] = playerChar;
}

int assessBoard(vector<vector<char>>& board)
{	
	//Check if either has won
	bool completeAny;

	//Row check
	for (const auto& row : board)
	{
		//bool completeRow = true;
		completeAny = true;
		for (const auto& cell : row)
		{
			if (cell != computerChar)
			{
				completeAny = false;
				break;
			}
		}
		if (completeAny) return 1;
		else
		{
			//bool completeRowPlayer = true;
			completeAny = true;
			for (const auto& cell : row)
			{
				if (cell != playerChar)
				{
					completeAny = false;
					break;
				}
			}
			if (completeAny) return -1;
		}
	}

	//Col check
	for (int colNum = 0; colNum < board.size(); colNum++)
	{
		completeAny = true;
		for (int rowNum = 0; rowNum < board.size(); rowNum++)
		{
			if (board[rowNum][colNum] != computerChar)
			{
				completeAny = false;
				break;
			}
		}
		if (completeAny) return 1;
		else
		{
			completeAny = true;
			for (int rowNum = 0; rowNum < board.size(); rowNum++)
			{
				if (board[rowNum][colNum] != playerChar)
				{
					completeAny = false;
					break;
				}
			}
			if (completeAny) return -1;
		}
	}

	//Main diagonal check
	completeAny = true;
	for (int diagPosNum = 0; diagPosNum < board.size(); diagPosNum++)
	{
		if (board[diagPosNum][diagPosNum] != computerChar)
		{
			completeAny = false;
			break;
		}
	}
	if (completeAny) return 1;
	else
	{
		completeAny = true;
		for (int diagPosNum = 0; diagPosNum < board.size(); diagPosNum++)
		{
			if (board[diagPosNum][diagPosNum] != playerChar)
			{
				completeAny = false;
				break;
			}
		}
		if (completeAny) return -1;
	}
	
	//Reverse diagonal check
	completeAny = true;
	int revDiagMaxCol = board.size() - 1;
	for (int diagPosNum = 0; diagPosNum < board.size(); diagPosNum++)
	{
		if (board[diagPosNum][revDiagMaxCol - diagPosNum] != computerChar)
		{
			completeAny = false;
			break;
		}
	}
	if (completeAny) return 1;
	else
	{
		completeAny = true;
		for (int diagPosNum = 0; diagPosNum < board.size(); diagPosNum++)
		{
			if (board[diagPosNum][revDiagMaxCol - diagPosNum] != playerChar)
			{
				completeAny = false;
				break;
			}
		}
		if (completeAny) return -1;
	}


	return 0;
}

vector<pair<int, int>> getNewFreeSpots(const vector<pair<int, int>>& freeSpots, const int& pos)
{
	vector<pair<int, int>> res;
	res.reserve(freeSpots.size() - 1);
	int i;
	for (i = 0; i < pos; i++)
	{
		res.push_back(freeSpots[i]);
	}
	for (i = pos + 1; i < freeSpots.size(); i++)
	{
		res.push_back(freeSpots[i]);
	}

	return res;
}


int alphaBetaSearch(vector<vector<char>>& board,const int& boardFill)
{
	pair<int, int> bestMove(boardSize, boardSize);
	int alphaStart = numeric_limits<int>::min();
	int betaStart = numeric_limits<int>::max();
	int beginFill = boardFill;
	int maxVal = numeric_limits<int>::min();

	for (int i = 0; i < board.size(); i++)
	{
		for (int j = 0; j < board.size(); j++)
		{
			if (board[i][j] == '_')
			{
				board[i][j] = computerChar;
				//int alphaStart = numeric_limits<int>::min();
				//int betaStart = numeric_limits<int>::max();
				int moveVal = minValue(board, alphaStart, betaStart, 1, beginFill + 1);
				board[i][j] = '_';
				if (moveVal > maxVal)
				{
					maxVal = moveVal;
					//alphaStart = maxVal > alphaStart ? maxVal : alphaStart;
					bestMove = make_pair(i, j);
				}
			}
		}
	}
	//int moveVal = maxValue(board, bestMove, alphaStart, betaStart, 0, beginFill);
	board[bestMove.first][bestMove.second] = computerChar;

	//return moveVal;
	return maxVal;
}

int maxValue(vector<vector<char>>& board, int alpha, int beta, int depth, int boardFill)
{
	//int numMoves = freeSpots.size();
	//cout << "AT DEPTH:" << depth << endl;
	//printBoard(board);
	int testEnd = assessBoard(board);
	if (boardFill == board.size()*board.size() || testEnd != 0) return testEnd * (board.size()*board.size() + 1 - depth);

	int value = numeric_limits<int>::min();
	//int value = depth - (board.size() * board.size() + 1);
	int cMin;
	//pair<int, int> cMove;
	for (int i = 0; i < board.size(); i++)
	{
		for (int j = 0; j < board.size(); j++)
		{
			if (board[i][j] == '_')
			{
				board[i][j] = computerChar;
				cMin = minValue(board, alpha, beta, depth + 1, boardFill + 1);
				board[i][j] = '_';
				if (cMin > value)
				{
					value = cMin;
					/*if (depth == 0)
					{
						bestMove.first = i;
						bestMove.second = j;
					}*/
					if (value >= beta) return value;
					alpha = value > alpha ? value : alpha;
					//if (value >= alpha) alpha = value;
				}
			}
		}
	}

	return value;
}

int minValue(vector<vector<char>>& board, int alpha, int beta, int depth, int boardFill)
{
	//int numMoves = freeSpots.size();
	//cout << "AT DEPTH:" << depth << endl;
	//printBoard(board);
	int testEnd = assessBoard(board);
	if (boardFill == board.size()*board.size() || testEnd != 0) return testEnd * (board.size()*board.size() + 1 - depth);

	int value = numeric_limits<int>::max();
	//int value = (board.size() * board.size() + 1) - depth;
	int cMax;
	for (int i = 0; i < board.size(); i++)
	{
		for (int j = 0; j < board.size(); j++)
		{
			if (board[i][j] == '_')
			{
				board[i][j] = playerChar;
				cMax = maxValue(board, alpha, beta, depth + 1, boardFill + 1);
				board[i][j] = '_';
				if (cMax < value)
				{
					value = cMax;
					if (value <= alpha) return value;
					beta = value < beta ? value : beta;
					//if (value < beta) beta = value;
				}
			}
		}
	}

	return value;
}

void playGame(vector<vector<char>>& board,const short playerTurn, const int boardFill)
{
	cout << "After turn #" << boardFill << endl;
	printBoard(board);
	cout << endl;
	
	int boardState = assessBoard(board);
	if (boardState > 0)
	{
		cout << "I WIN!" << endl;
		return;
	}
	else if (boardState < 0)
	{
		cout << "YOU WIN!" << endl;
		return;
	}
	else
	{
		if (boardFill == boardSize * boardSize)
		{
			//if (assessBoard(board) == 0) cout << "DRAW!" << endl;
			//else if (assessBoard(board) > 0) cout << "I WIN!" << endl;
			//else cout << "YOU WIN!" << endl;
			cout << "DRAW!" << endl;
			return;
		}
		if (playerTurn == 1)
		{
			playerTurnInput(board);
			playGame(board, 0, boardFill + 1);
		}
		else
		{
			//if (boardFill == 0) board[1][1] = computerChar;
			//else alphaBetaSearch(board, boardFill);
			alphaBetaSearch(board, boardFill);
			playGame(board, 1, boardFill + 1);
		}
	}
}

void startGame()
{
	//The board
	vector<vector<char>> board(boardSize, vector<char>(boardSize, '_'));
	//printBoard(board);

	//0 - PC, 1 - Player
	short playerTurn;
	cout << "Choose who starts: Computer(0) or Player(1)" << endl;
	cin >> playerTurn;
	//cout << endl << playerTurn << endl;

	//Let player choose sign
	cout << endl << "Choose player sign: O or X" << endl;
	cin >> playerChar;
	if (playerChar == 'O') computerChar = 'X';
	else computerChar = 'O';

	//Print the starting board
	//cout << "Initial board state: " << endl;
	//printBoard(board);

	//Call the main playing function
	playGame(board, playerTurn, 0);
	cin.get();
	return;
}

int main()
{
	/*
	vector<int> a({ 0, 0 ,2 ,3 ,4 ,5 });
	int pl = 0;
	cout << pl << " " << pl++ << " " << (++pl) ++ << endl;
	cout << pl;
	*/
	/*
	
	//Number of cells per side
	constexpr unsigned int boardSize = 3;
	//The board
	vector<vector<char>> board(boardSize,vector<char>(boardSize,'_'));

	//0 - PC, 1 - Player
	short playerTurn;
	cin >> playerTurn;
	cout << endl << playerTurn << endl;
	*/
	//Number of cells per side
	//constexpr unsigned int boardSize = 3;
	
	//Start the game
	//startGame(boardSize);
	startGame();

	cin.get();
	return 0;
}