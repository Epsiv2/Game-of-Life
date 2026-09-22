// Libraries:
#include <iostream>
#include <chrono>
#include <thread>
#include <limits>
#include <string>
#include <vector>
#include <random>
#define NOMINMAX // Prevents windows.h from defining min and max macros that can conflict with std::min and std::max
#include <windows.h>
#include <fstream>

using namespace std;



// Count the number of alive neighbors for a given cell
int countAliveNeighbors(const vector<vector<char>>& board, int row, int col)
{
    int aliveNeighbors = 0;

    // Check all possible neighboring positions
    for (int rowOffset = -1; rowOffset <= 1; rowOffset++)
    {
        for (int colOffset = -1; colOffset <= 1; colOffset++)
        {
            // Skip the current cell
            if (rowOffset == 0 && colOffset == 0)
            {
                continue;
            }

            // Calculate the neighbor's position
            int neighborRow = row + rowOffset;
            int neighborCol = col + colOffset;

            // Make sure the neighbor is inside the board
            if (neighborRow >= 0 && neighborRow < board.size() &&
                neighborCol >= 0 && neighborCol < board[neighborRow].size())
            {
                // Check if the neighbor is alive
                if (board[neighborRow][neighborCol] == 'X')
                {
                    aliveNeighbors++;
                }
            }
        }
    }

    return aliveNeighbors;
}

// Display the current board
void printBoard(const vector<vector<char>>& board, 
				const vector<vector<int>>& cellAge)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    for (int row = 0; row < board.size(); row++)
    {
        for (int col = 0; col < board[row].size(); col++)
        {
			if (board[row][col] == 'X')
			{
				// Set the text color based on the age of the cell
				if (cellAge[row][col] == 1)
				{
					SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY); // Young cells: Green
				}
				else if (cellAge[row][col] == 2)
				{
					SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_INTENSITY); // Middle-aged cells: Blue
				}
				else
				{
					SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY); // Old cells: Red
				}
			}
			else
			{
				// Reset to default color for dead cells
				SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
			}
			// Display the current cell state (alive or dead)
            cout << board[row][col] << ' ';
			SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); // Reset to default color after each cell
        }

        cout << endl;
    }
}

// Generate the next generation of the board based on the current state
void generateNextGeneration(const vector<vector<char>>& board, vector<vector<char>>& nextBoard,	
							const vector<vector<int>>& cellAge, vector<vector<int>>& nextCellAge)
{
    // Calculate the state of every cell for the next generation
	for (int row = 0; row < board.size(); row++)
	{
		for (int col = 0; col < board[row].size(); col++)
		{
			int aliveNeighbors = countAliveNeighbors(board, row, col);
			
            // Apply the rules of the Game of Life
			if (board[row][col] == 'X')
			{
				// A live cell survives with 2 or 3 live neighbors, otherwise it dies
				if (aliveNeighbors == 2 || aliveNeighbors == 3)
				{
					nextBoard[row][col] = 'X';
					nextCellAge[row][col] = cellAge[row][col] + 1; // Increment the age of the cell
				}
				else
				{
					nextBoard[row][col] = '.';
					nextCellAge[row][col] = 0; // Reset the age of the dead cell
				}
			}
			else
			{
				// Dead cell becomes alive if it has exactly 3 live neighbors
				if (aliveNeighbors == 3)
				{
					nextBoard[row][col] = 'X';
					nextCellAge[row][col] = 1; // Set the age of the newly born cell to 1
				}
				else
				{
					nextBoard[row][col] = '.';
					nextCellAge[row][col] = 0; // Reset the age of the dead cell
				}
			}
		}
	}
}

// Clear the input buffer to handle invalid input
void clearInputBuffer()
{
	cin.clear(); // Clear the error flag
	cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Discard invalid input
}

// Get a valid integer from the user
int getValidInteger(const string& prompt, int minValue, int maxValue = numeric_limits<int>::max())
{
	int value;
	cout << prompt;
	cin >> value;

	while (cin.fail() || value < minValue || value > maxValue)
	{
		clearInputBuffer();
		cout << "Invalid input. Please enter an integer between " << minValue << " and " << maxValue << "." << endl;
		cin >> value;
	}
	return value;
}

// Prompt the user to choose the setup mode for the board
int chooseSetupMode()
{
	cout << "How do you want to setup the board?" << endl;
	cout << "1. Manual setup (enter coordinates of alive cells)" << endl;
	cout << "2. Random setup (place alive cells randomly)" << endl;
	cout << "3. Load from file (load board configuration from a file)" << endl;
	return getValidInteger("Choose an option: ", 1, 3);
}

// Wait for a specified number of seconds
void waitForSeconds(int seconds)
{
	this_thread::sleep_for(chrono::seconds(seconds));
}

// Set up the initial board with user-defined alive cells
void setupBoard(vector<vector<char>>& board,
				vector<vector<int>>& cellAge)
{

	// Validate the input for the number of alive cells
	int maxCells = board.size() * board[0].size();

	int numberOfCells = getValidInteger("How many cells do you want to set as alive? ", 0, maxCells);

	// Prompt the user to enter the coordinates of the alive cells
	for (int i = 0; i < numberOfCells; i++)
	{
		int row, col;

		while (true)
		{
			cout << "Enter the coordinates of alive cell " << (i + 1) << " (row and column): ";
			cin >> row >> col;

			// Validate the input for row and column indices
			if (cin.fail() || row < 0 || row >= board.size() || col < 0 || col >= board[0].size())
			{
				clearInputBuffer();
				cout << "Invalid input. Please enter valid row and column indices (0-indexed): ";
				continue;
			}
			
			// Check if the cell is already alive
			if (board[row][col] == 'X')
			{
				cout << "This cell is already alive. Please choose a different cell." << endl;
				continue;
			}
			break;
		}
		
		board[row][col] = 'X';
		cellAge[row][col] = 1;
	}
}

// Set up the initial board with randomly placed alive cells
void setupRandomBoard(vector<vector<char>>& board,
					  vector<vector<int>>& cellAge)
{

	int maxCells = board.size() * board[0].size();

	int numberOfCells = getValidInteger("How many cells do you want to set as alive randomly? ", 0, maxCells);

	// Use random number generation to place alive cells randomly on the board
	random_device rd;
	mt19937 gen(rd()); // Mersenne Twister random number generator (pseudo-random)
	uniform_int_distribution<> disRow(0, board.size() - 1);
	uniform_int_distribution<> disCol(0, board[0].size() - 1);

	int cellsPlaced = 0;

	while (cellsPlaced < numberOfCells)
	{
		int row = disRow(gen);
		int col = disCol(gen);

		// Check if the cell is already alive
		if (board[row][col] == '.')
		{
			board[row][col] = 'X';
			cellsPlaced++;
			cellAge[row][col] = 1;
		}
	}
}

// Load the initial board configuration from a file
bool loadBoardFromFile(const string& filename,
					   vector<vector<char>>& board,
					   vector<vector<int>>& cellAge)
{
	ifstream inputFile(filename);
	// Check if the file was opened successfully
	if (!inputFile)
	{
		cout << "Error: Could not open file " << filename << endl;
		return false;
	}
	int rows, cols;
	inputFile >> rows >> cols;
	// Validate the dimensions of the board
	if (rows <= 0 || cols <= 0)
	{
		cout << "Error: Invalid board dimensions in file." << endl;
		return false;
	}

	// Resize the board and cellAge vectors to match the dimensions specified in the file
	board.resize(rows, vector<char>(cols, '.'));
	cellAge.resize(rows, vector<int>(cols, 0));

	// Read the board configuration from the file
	string line;
	for (int row = 0; row < rows; row++)
	{
		inputFile >> line;

		// Check if the row has the correct number of cells
		if (line.length() != cols)
		{
			cout << "Error: Invalid row length in file." << endl;
			return false;
		}

		// Check if the characters in the row are valid (either 'X' for alive or '.' for dead)
		for (int col = 0; col < cols; col++)
		{
			if (line[col] != 'X' && line[col] != '.')
			{
				cout << "Error: Invalid character in file." << endl;
				return false;
			}

			board[row][col] = line[col];

		if(line[col] == 'X')
		{
			cellAge[row][col] = 1; // Set age to 1 for alive cells
			}
			else
			{
				cellAge[row][col] = 0; // Set age to 0 for dead cells
			}
		}
	}
	inputFile.close();
	return true;
}

// Run The Game of Life simulation
void runGameOfLife(vector<vector<char>>& board, vector<vector<char>>& nextBoard, int& generation,
					vector<vector<int>>& cellAge, vector<vector<int>>& nextCellAge)
{
	string input;
	// Loop to generate the next generation or quit
	while (true)
	{
		cout << endl;
		cout << "Press 'n' for next generation, 'a' for auto-play, 'q' to quit: ";
		cin >> input;
		// Quit the program
		if (input == "q" || input == "Q")
		{
			break;
		}
		// Generate the next generation
		else if (input == "n" || input == "N")
		{
			generateNextGeneration(board, nextBoard, cellAge, nextCellAge);
			board = nextBoard; // Update the current board to the next generation
			cellAge = nextCellAge; // Update the cell ages
			generation++;

			cout << endl;
			cout << "Generation: " << generation << endl;
			printBoard(board, cellAge);
		}
		// Auto-play mode
		else if (input == "a" || input == "A")
		{

			int numberOfGenerations = getValidInteger("Enter the number of generations to simulate: ", 1);


			for (int i = 0; i < numberOfGenerations; i++)
			{
				generateNextGeneration(board, nextBoard, cellAge, nextCellAge);
				board = nextBoard; // Update the current board to the next generation
				cellAge = nextCellAge; // Update the cell ages
				generation++;

				cout << endl;
				cout << "Generation: " << generation << endl;
				printBoard(board, cellAge);

				waitForSeconds(1); // Wait for 1 second before displaying the next generation
			}
		}
		else
		{
			cout << "Invalid input. Please try again." << endl;
		}
	}
}



// Main function
int main()
{

	// Create the initial generation
	int generation = 0;


	int setupMode = chooseSetupMode();

	vector<vector<char>> board;
	vector<vector<char>> nextBoard;
	vector<vector<int>> cellAge;
	vector<vector<int>> nextCellAge;

	int rows, cols = 0;

	if (setupMode == 1 || setupMode == 2)
	{
		rows = getValidInteger("Enter the number of rows for the board: ", 1);
		cols = getValidInteger("Enter the number of columns for the board: ", 1);

		board = vector<vector<char>>(rows, vector<char>(cols, '.')); // Initialize the board with dead cells
		cellAge = vector<vector<int>>(rows, vector<int>(cols, 0)); // Initialize cell ages

		if (setupMode == 1)
		{
			setupBoard(board, cellAge);
		}
		else
		{
			setupRandomBoard(board, cellAge);
		}
	}
	else if (setupMode == 3)
	{
		if (!loadBoardFromFile("board.txt", board, cellAge))
		{
			return 1;
		}
		rows = board.size();
		cols = board[0].size();
	}

	nextBoard = vector<vector<char>>(rows, vector<char>(cols, '.')); // Initialize the next generation board with dead cells
	nextCellAge = vector<vector<int>>(rows, vector<int>(cols, 0)); // Initialize next generation cell ages

    // Display the current board
    printBoard(board, cellAge);


	cout << "Generation: " << generation << endl;

	runGameOfLife(board, nextBoard, generation, cellAge, nextCellAge); // Run the Game of Life simulation



    return 0;
}