// necessary header files for input/output, vectors, integer limits, arrays, and string stream.
#include <stdio.h>
#include <iostream>
#include <vector>
#include <limits.h>
#include <array>
#include <sstream>

//defining a function-like macro 
#define min(a, b) (((a) < (b)) ? (a) : (b))
#define max(a, b) (((a) > (b)) ? (a) : (b))

using namespace std;

void printBoard(vector<vector<int>> &);
int userMove();
void makeMove(vector<vector<int>> &, int, unsigned int);
void errorMessage(int);
int aiMove();
vector<vector<int>> copyBoard(vector<vector<int>>);
bool winningMove(vector<vector<int>> &, unsigned int);
int scoreSet(vector<unsigned int>, unsigned int);
int tabScore(vector<vector<int>>, unsigned int);
array<int, 2> miniMax(vector<vector<int>> &, unsigned int, int, int, unsigned int);
int heurFunction(unsigned int, unsigned int, unsigned int);

unsigned int NUM_COL = 7;
unsigned int NUM_ROW = 6;
unsigned int PLAYER = 1;
unsigned int COMPUTER = 2;
unsigned int MAX_DEPTH = 7;

bool gameOver = false;
unsigned int turns = 0;
unsigned int currentPlayer = PLAYER;

vector<vector<int>> board(NUM_ROW, vector<int>(NUM_COL));

void playGame()
{
	printBoard(board);
	while (!gameOver)     // while game is not over
	{
		// initially current plaer is human

		if (currentPlayer == COMPUTER)
		{
			makeMove(board, aiMove(), COMPUTER);      // aiMove tells col no for move 
		}
		else if (currentPlayer == PLAYER)
		{
			makeMove(board, userMove(), PLAYER);
		}
		else if (turns == NUM_ROW * NUM_COL)     // if turns over
		{
			gameOver = true;
		}
		gameOver = winningMove(board, currentPlayer);
		currentPlayer = (currentPlayer == 1) ? 2 : 1;   // change the turn of the player 
		turns++;
		cout << endl;
		printBoard(board);
	}
	if (turns == NUM_ROW * NUM_COL)
	{
		cout << "Draw!" << endl;
	}
	else
	{
		cout << ((currentPlayer == PLAYER) ? "AI Wins!" : "Player Wins!") << endl;

	}
}

void makeMove(vector<vector<int>> &b, int c, unsigned int p)  // passing board , col , player 
{
	for (unsigned int r = 0; r < NUM_ROW; r++)
	{
		// If the position is empty, update the board with the player's piece (1 or 2)
		if (b[r][c] == 0)
		{
			b[r][c] = p;
			break;
		}
	}
}

int userMove()
{
	int move = -1;
	while (true)
	{
		cout << "Enter a column: ";
		cin >> move;

		//  If the input is not an integer
		if (!cin)
		{
			cin.clear();
			cin.ignore(INT_MAX, '\n');   //// Discard invalid input
			errorMessage(1);   // Display an error message for invalid input
		}
		else if (!((unsigned int)move < NUM_COL && move >= 0))    // If the entered column is out of bounds
		{
			errorMessage(2);
		}
		else if (board[NUM_ROW - 1][move] != 0)   // If the selected column is already full
		{
			errorMessage(3);
		}
		else
		{
			break;
		}
		cout << endl
			 << endl;
	}
	return move;
}

int aiMove()
{
	cout << "AI is thinking about a move..." << endl;

	//The function returns an array with the best move's score and column index accessed through `[1]`.
	return miniMax(board, MAX_DEPTH, 0 - INT_MAX, INT_MAX, COMPUTER)[1];
}

// The function returns an array with the best move's score and column index accessed through `[1]`.
array<int, 2> miniMax(vector<vector<int>> &b, unsigned int d, int alf, int bet, unsigned int p)
{

	// Base case: Check if the depth limit is reached or the game is in a terminal state
	if (d == 0  /* || d >= (NUM_COL * NUM_ROW) - turns */)
	{
		// Return the evaluation of the current board state and an invalid move
		return array<int, 2>{tabScore(b, COMPUTER), -1};
	}
	// If the current player is the computer (maximizing player)
	if (p == COMPUTER)
	{
		//keep track of the best move and its score
		array<int, 2> moveSoFar = {INT_MIN, -1};

		//// Check if the opponent has already won
		if (winningMove(b, PLAYER))
		{
			return moveSoFar;
		}

		// Iterate through each column to find the best move
		for (unsigned int c = 0; c < NUM_COL; c++)
		{
					// Check if the column is not full
			if (b[NUM_ROW - 1][c] == 0)
			{
				// Create a new board after making a potential move
				vector<vector<int>> newBoard = copyBoard(b);
				makeMove(newBoard, c, p);

				// Recursively call miniMax for the opponent (minimizing player)
				int score = miniMax(newBoard, d - 1, alf, bet, PLAYER)[0];

			// Update the best move if the current move has a higher score
				if (score > moveSoFar[0])
				{
					moveSoFar = {score, (int)c};
				}

				// Update the alpha value
				alf = max(alf, moveSoFar[0]);

				// Perform alpha-beta pruning if beta is less than or equal to alpha
				if (alf >= bet)
				{
					break;
				}
			}
		}

		// Return the best move for the computer

		return moveSoFar;
	}
			// If the current player is the opponent (minimizing player)

	else
	{
		array<int, 2> moveSoFar = {INT_MAX, -1};

		if (winningMove(b, COMPUTER))
		{
			return moveSoFar;
		}
		for (unsigned int c = 0; c < NUM_COL; c++)
		{
			if (b[NUM_ROW - 1][c] == 0)
			{
				vector<vector<int>> newBoard = copyBoard(b);
				makeMove(newBoard, c, p);

				// Recursively call miniMax for the computer (maximizing player)

				int score = miniMax(newBoard, d - 1, alf, bet, COMPUTER)[0];
				if (score < moveSoFar[0])
				{
					moveSoFar = {score, (int)c};
				}
				bet = min(bet, moveSoFar[0]);
				if (alf >= bet)
				{
					break;
				}
			}
		}

		// Return the best move for the opponent
		return moveSoFar;
	}
}

int tabScore(vector<vector<int>> b, unsigned int p)
{
	// Initialize the total score
	int score = 0;

	// Vectors to store rows, columns, and sets of 4 elements
	vector<unsigned int> rs(NUM_COL);
	vector<unsigned int> cs(NUM_ROW);
	vector<unsigned int> set(4);

	// Calculate scores for each row
	for (unsigned int r = 0; r < NUM_ROW; r++)
	{
		// Copy row elements to the 'rs' vector
		for (unsigned int c = 0; c < NUM_COL; c++)
		{
			rs[c] = b[r][c];
		}
		// Iterate through each set of 4 elements in the row and calculate score
		for (unsigned int c = 0; c < NUM_COL - 3; c++)
		{
			for (int i = 0; i < 4; i++)
			{
				set[i] = rs[c + i];
			}
			// Add the score of the set to the total score
			score += scoreSet(set, p);
		}
	}

	// Calculate scores for each column
	for (unsigned int c = 0; c < NUM_COL; c++)
	{
		// Copy column elements to the 'cs' vector
		for (unsigned int r = 0; r < NUM_ROW; r++)
		{
			cs[r] = b[r][c];
		}
		// Iterate through each set of 4 elements in the column and calculate score
		for (unsigned int r = 0; r < NUM_ROW - 3; r++)
		{
			for (int i = 0; i < 4; i++)
			{
				set[i] = cs[r + i];
			}
			// Add the score of the set to the total score
			score += scoreSet(set, p);
		}
	}

	// Calculate scores for each diagonal (bottom-left to top-right)
	for (unsigned int r = 0; r < NUM_ROW - 3; r++)
	{
		for (unsigned int c = 0; c < NUM_COL; c++)
		{
			rs[c] = b[r][c];
		}
		for (unsigned int c = 0; c < NUM_COL - 3; c++)
		{
			for (int i = 0; i < 4; i++)
			{
				set[i] = b[r + i][c + i];
			}
			// Add the score of the set to the total score
			score += scoreSet(set, p);
		}
	}

	// Calculate scores for each diagonal (top-left to bottom-right)
	for (unsigned int r = 0; r < NUM_ROW - 3; r++)
	{
		for (unsigned int c = 0; c < NUM_COL; c++)
		{
			rs[c] = b[r][c];
		}
		for (unsigned int c = 0; c < NUM_COL - 3; c++)
		{
			for (int i = 0; i < 4; i++)
			{
				set[i] = b[r + 3 - i][c + i];
			}
			// Add the score of the set to the total score
			score += scoreSet(set, p);
		}
	}

	// Return the total score
	return score;
}


// Calculates the score of a set of four elements (row, column, or diagonal)
int scoreSet(vector<unsigned int> v, unsigned int p)
{
	// Count of elements matching the player's piece, opponent's piece, and empty spaces
	unsigned int good = 0;
	unsigned int bad = 0;
	unsigned int empty = 0;

	// Iterate through the set and count the occurrences of player's piece, opponent's piece, and empty spaces
	for (unsigned int i = 0; i < v.size(); i++)
	{
		good += (v[i] == p) ? 1 : 0;
		bad += (v[i] == PLAYER || v[i] == COMPUTER) ? 1 : 0;
		empty += (v[i] == 0) ? 1 : 0;
	}

	// Adjust the count of opponent's pieces (subtract the count of player's pieces)
	bad -= good;

	// Call the heuristic function to determine the score of the set
	return heurFunction(good, bad, empty);
}

// Determines the score based on the count of player's pieces (g), opponent's pieces (b), and empty spaces (z)
int heurFunction(unsigned int g, unsigned int b, unsigned int z)
{
	int score = 0;

	// Evaluate different scenarios and assign scores accordingly
	if (g == 4)
	{
		score += 500001;  // Winning move for the player
	}
	else if (g == 3 && z == 1)
	{
		score += 5000;  // Potential winning move for the player with one empty space
	}
	else if (g == 2 && z == 2)
	{
		score += 500;  // Two player's pieces with two empty spaces
	}
	else if (b == 2 && z == 2)
	{
		score -= 501;  // Two opponent's pieces with two empty spaces
	}
	else if (b == 3 && z == 1)
	{
		score -= 5001;  // Potential winning move for the opponent with one empty space
	}
	else if (b == 4)
	{
		score -= 500000;  // Winning move for the opponent
	}

	// Return the calculated score
	return score;
}


bool winningMove(vector<vector<int>> &b, unsigned int p)
{
	// Initialize a variable to count the consecutive pieces in a sequence
	unsigned int winSequence = 0;

    // Check for a winning sequence horizontally
	for (unsigned int c = 0; c < NUM_COL - 3; c++)
	{
		for (unsigned int r = 0; r < NUM_ROW; r++)
		{
			for (int i = 0; i < 4; i++)
			{
				if ((unsigned int)b[r][c + i] == p)
				{
					winSequence++;
				}
				if (winSequence == 4)
				{
					return true;
				}
			}
			winSequence = 0;
		}
	}

    // Check for a winning sequence vertically

	for (unsigned int c = 0; c < NUM_COL; c++)
	{
		for (unsigned int r = 0; r < NUM_ROW - 3; r++)
		{
			for (int i = 0; i < 4; i++)
			{
				if ((unsigned int)b[r + i][c] == p)
				{
					winSequence++;
				}
				if (winSequence == 4)
				{
					return true;
				}
			}
			winSequence = 0;
		}
	}

    // Check for a winning sequence diagonally (bottom-left to top-right)
	for (unsigned int c = 0; c < NUM_COL - 3; c++)
	{
		for (unsigned int r = 3; r < NUM_ROW; r++)
		{
			for (int i = 0; i < 4; i++)
			{
				if ((unsigned int)b[r - i][c + i] == p)
				{
					winSequence++;
				}
				if (winSequence == 4)
				{
					return true;
				}
			}
			winSequence = 0;
		}
	}

	// Check for a winning sequence diagonally (top-left to bottom-right)
	for (unsigned int c = 0; c < NUM_COL - 3; c++)
	{
		for (unsigned int r = 0; r < NUM_ROW - 3; r++)
		{
			for (int i = 0; i < 4; i++)
			{
				if ((unsigned int)b[r + i][c + i] == p)
				{
					winSequence++;
				}
				if (winSequence == 4)
				{
					return true;
				}
			}
			winSequence = 0;
		}
	}
	return false;
}

void initBoard()
{
	/* num_rows=6 & cols=7 */
	for (unsigned int r = 0; r < NUM_ROW; r++)
	{
		for (unsigned int c = 0; c < NUM_COL; c++)
		{
			board[r][c] = 0;
		}
	}
}

vector<vector<int>> copyBoard(vector<vector<int>> b)
{
	vector<vector<int>> newBoard(NUM_ROW, vector<int>(NUM_COL));
	for (unsigned int r = 0; r < NUM_ROW; r++)
	{
		for (unsigned int c = 0; c < NUM_COL; c++)
		{
			newBoard[r][c] = b[r][c];
		}
	}
	return newBoard;
}

void printBoard(vector<vector<int>> &b)
{
	// for printing cols name 0 1 2 3 4 5 6 
	for (unsigned int i = 0; i < NUM_COL; i++)
	{
		cout << " " << i;
	}
	cout << endl
		 << "---------------" << endl;
	for (unsigned int r = 0; r < NUM_ROW; r++)
	{
		for (unsigned int c = 0; c < NUM_COL; c++)
		{
			            cout << "|";

			// Switch statement to determine the content of the cell
            switch (b[NUM_ROW - r - 1][c])
            {
            case 0:
                // If the cell is empty, print a space
                cout << " ";
                break;
            case 1:
                // If the cell is filled by the player, print "O"  (player)
                cout << "O";
                break;
            case 2:
                // If the cell is filled by the computer, print "X" (machine)
                cout << "X";
                break;
            }

            // If it's the last column, print a closing vertical bar
            if (c + 1 == NUM_COL)
            {
                cout << "|";
            }
		}
		cout << endl;
	}
	cout << "---------------" << endl;
	cout << endl;
}

void errorMessage(int t)
{
	if (t == 1)
	{
		cout << "Use a value 0.." << NUM_COL - 1 << endl;
	}
	else if (t == 2)
	{
		cout << "That is not a valid column." << endl;
	}
	else if (t == 3)
	{
		cout << "That column is full." << endl;
	}
	cout << endl;
}

int main()
{

	initBoard();
	playGame();
	return 0;

}



/*
- `tabScore` function returns an overall score for the game board based on patterns of player pieces,
 opponent pieces, and empty spaces.

- `scoreSet` function returns a score for a set of four elements (e.g., a row, column, or diagonal) based 
on the count of player pieces, opponent pieces, and empty spaces.

- `heurFunction` function returns a heuristic score based on the count of player pieces (`g`),
 opponent pieces (`b`), and empty spaces (`z`) in a set.

- `miniMax` function returns an array where the first element is the score of the best move, and `[1]` retrieves 
the column index of that move. This function is used for decision-making in the game, considering different
 possible moves and their scores.
*/