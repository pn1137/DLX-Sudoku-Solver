#include <deque>
#include <fstream>
#include <iostream>
#include <string>
#include <cstring>

// Row = 9*9*9
#define ROW_SIZE 729
// Col = 9*9*4
#define COL_SIZE 324

/*
 *	A node structure to hold directional links, a columnar link and positional information
 */
struct Node 
{
	Node *left = nullptr;
	Node *right = nullptr;
	Node *up = nullptr;
	Node *down = nullptr;
	Node *column = nullptr;
	
	int row;	// Row number
	int col;	// Column number
	
	// Used for column header nodes
	int count = 0;	// Tracks the number of active nodes in a column
};

/*
 *	Class to utilize Dancing Links and Algorithm X by Don Knuth
 *		in order to solve sudoku puzzles
 */
class DLX
{
	private:
		Node *headHeader;	// The header used to find column headers
		
		// [0][0] is main headHeader, 0th row is for headers, else 1-based
		Node nodeCoverMatrix[ROW_SIZE+1][COL_SIZE+1];	
		bool coverMatrix[ROW_SIZE][COL_SIZE];	// 0-based
		int sudokuBoard[9][9];	// A 2-D array representing a given sudoku puzzle
		
		std::deque<Node*> solution;	// The solution to the given sudoku puzzle
	
	public:
		// Constructor for DLX
		DLX(std::string sudokuString)
		{
			headHeader = &nodeCoverMatrix[0][0];	// Assign headHeader
			
			populatecoverMatrix();	// Create an exact cover matrix for sudoku puzzles
			sudokuStringToMatrix(sudokuString);	// Convert a sudoku board from a string into a 2-D array
			coverRows();	// Cover impossible rows in the cover matrix based on the given sudoku puzzle
			linkNodeCoverMatrix();	// Link nodes in the nodeCoverMatrix
		}
		
		// Eliminates rows in the cover matrix that are impossible in a given sudoku puzzle
		void coverRows()
		{
			for (int i = 0; i < 9; i++)
			{
				for (int j = 0; j < 9; j++)
				{
					int p = sudokuBoard[i][j];	// Value given in partial sudoku board
					
					if (p != 0)	
					{
						for (int k = 0; k < 9; k++)
						{
							if (k != p-1)	// 0-based cover matrix, subtract 1 from values 1-9
							{
								// Zero-out rows that can't be in the solution
								for (int l = 0; l < COL_SIZE; l++)
								{
									coverMatrix[i*81+(j+1)*9-(9-k)][l] = 0;
								}
							}
						}
					}
				}
			}
		}
		
		// Convert a sudoku board contained in a string into a 2-D array
		void sudokuStringToMatrix(std::string s)
		{
			for (int i = 0; i < s.size(); i++)
			{
				if (s.at(i) != '-')
				{
					sudokuBoard[(i/9)][i%9] = s.at(i)-'0';
				}
				else
				{
					sudokuBoard[i/9][i%9] = 0;
				}
			}
		}
	
		// Creates an exact cover matrix for sudoku puzzles
		void populatecoverMatrix()
		{
			// Zero-out the matrix
			memset(coverMatrix, 0, sizeof(coverMatrix[0][0]) * ROW_SIZE * COL_SIZE);
			
			// Populate the matrix
			for (int i = 0; i < ROW_SIZE; i++)
			{
				// i/9 = cell constraints
				coverMatrix[i][i/9] = 1;
				// 81 + i%9 + (i/81)*9 = row constraints
				coverMatrix[i][81 + i%9 + (i/81)*9] = 1;
				// 162 + i%81 = col constraints
				coverMatrix[i][162 + i%81] = 1;
				// 243 + (i/27)*9 + i%9 - (i/81)*27 + (i/243)*27 = block constraints
				coverMatrix[i][243 + (i/27)*9 + i%9 - (i/81)*27 + (i/243)*27] = 1;
			}
		}
		
		// Link the nodes in the nodeCoverMatrix based on the values in the coverMatrix
		void linkNodeCoverMatrix()
		{
			for (int i = 0; i < ROW_SIZE; i++)
			{
				for (int j = 0; j < COL_SIZE; j++)
				{
					if (coverMatrix[i][j] == 1)
					{
						// Column header
						nodeCoverMatrix[0][j+1].count++;
						
						// Current Node
						nodeCoverMatrix[i+1][j+1].column = &nodeCoverMatrix[0][j+1];
						nodeCoverMatrix[i+1][j+1].row = i+1;
						nodeCoverMatrix[i+1][j+1].col = j+1;		
						
						int k; 
						
						// Right link
						k = j+1;
						while (k != j)
						{
							if (k >= COL_SIZE) { k = 0; }	// Wrap around
							
							if (coverMatrix[i][k] == 1)
							{
								nodeCoverMatrix[i+1][j+1].right = &nodeCoverMatrix[i+1][k+1];
								break;
							}
							
							k++;
						}
						
						// Left link
						k = j-1;
						while (k != j)
						{
							if (k < 0) { k = COL_SIZE-1; }	// Wrap around
							
							if (coverMatrix[i][k] == 1)
							{
								nodeCoverMatrix[i+1][j+1].left = &nodeCoverMatrix[i+1][k+1];
								break;
							}
							
							k--;
						}
						
						// Down link
						k = i+1;
						while (k != i)
						{
							if (k >= ROW_SIZE)	// Link to column header
							{ 
								nodeCoverMatrix[i+1][j+1].down = &nodeCoverMatrix[0][j+1];
								nodeCoverMatrix[0][j+1].up = &nodeCoverMatrix[i+1][j+1];
								break;
							}
							
							if (coverMatrix[k][j] == 1)
							{
								nodeCoverMatrix[i+1][j+1].down = &nodeCoverMatrix[k+1][j+1];
								break;
							}
							
							k++;
						}
						
						// Up link
						k = i-1;
						while (k != i)
						{
							if (k < 0)	// Link to column header
							{ 
								nodeCoverMatrix[i+1][j+1].up = &nodeCoverMatrix[0][j+1];
								nodeCoverMatrix[0][j+1].down = &nodeCoverMatrix[i+1][j+1];
								break;
							}
							
							if (coverMatrix[k][j] == 1)
							{
								nodeCoverMatrix[i+1][j+1].up = &nodeCoverMatrix[k+1][j+1];
								break;
							}
							
							k--;
						}
					}
				}
			}
			
			linkColumnHeaders();
		}
		
		// Links column headers
		void linkColumnHeaders()
		{
			// Link right
			for (int j = 0; j < COL_SIZE+1; j++)
			{
				// Set data for column header
				nodeCoverMatrix[0][j].row = 0;
				nodeCoverMatrix[0][j].col = j;
				nodeCoverMatrix[0][j].column = &nodeCoverMatrix[0][j];
				
				if (j == COL_SIZE)
				{
					nodeCoverMatrix[0][j].right = &nodeCoverMatrix[0][0];
				}
				else
				{
					nodeCoverMatrix[0][j].right = &nodeCoverMatrix[0][j+1];
				}
			}
			
			// Link left
			for (int j = COL_SIZE+1; j >= 0; j--)
			{
				if (j == 0)
				{
					nodeCoverMatrix[0][j].left = &nodeCoverMatrix[0][COL_SIZE];
				}
				else
				{
					nodeCoverMatrix[0][j].left = &nodeCoverMatrix[0][j-1];					
				}
			}
		}		
		
		// Covers columns and rows linked to a node
		void cover(Node* node)
		{
			Node* column = node->column;
			
			column->right->left = column->left;
			column->left->right = column->right;
			
			for (Node *row = column->down; row != column; row = row->down)
			{			
				for (Node *right = row->right; right != row; right = right->right)
				{
					right->up->down = right->down;
					right->down->up = right->up;
					right->column->count--;
				}
			}
		}
		
		// Uncovers columns and rows linked to a node
		void uncover(Node* node)
		{
			Node* column = node->column;
			
			for (Node *row = column->up; row != column; row = row->up)
			{
				for (Node *left = row->left; left != row; left = left->left)
				{
					left->up->down = left;
					left->down->up = left;
					left->column->count++;
				}
			}
			
			column->right->left = column;
			column->left->right = column;
		}
		
		// Searches the nodeCoverMatrix for a solution to the given sudoku puzzle
		// Algorithm X by Don Knuth
		void search()
		{
			// No columns remain, solution found
			if (headHeader->right == headHeader)
			{
				outputSolution();
				return;
			}
			else	// Search for solution
			{
				// Heuristic of column with minimum nodes
				Node *column = getMinCountColumn();
				cover(column);
				
				// Try column
				for (Node *row = column->down; row != column; row = row->down)
				{		
					// Try row
					solution.push_back(row);
					
					for (Node *right = row->right; right != row; right = right->right)
					{
						cover(right);
					}
					
					search();
					
					// Row doesn't lead to solution
					solution.pop_back();
					
					for (Node *left = row->left; left != row; left = left->left)
					{
						uncover(left);
					}
				}
				
				// Column doesn't lead to solution
				uncover(column);
			}			
		}
		
		// Returns the column with the minimum number of nodes
		Node* getMinCountColumn()
		{
			Node *min = headHeader->right;
			for (Node* node = min->right; node != headHeader; node = node->right)
			{
				if (node->count < min->count)
				{
					min = node;
				}
			}
			
			return min;
		}
		
		// Outputs the solution to the sudoku puzzle
		void outputSolution()
		{
			int array[9][9];
			
			int row;
			
			while (solution.size() > 0)
			{
				row = solution.front()->row-1;
				solution.pop_front();
				
				//  row  col    value
				// [0-8][0-8] = 1-9
				array[row/81][(row%81)/9] = (row%9) + 1;	
			}
			
			for (int i = 0; i < 9; i++)
			{
				for (int j = 0; j < 9; j++)
				{
					std::cout << array[i][j] << " | ";
					if (j == 8)
					{
						std::cout << std::endl;
					}
				}
			}
		}
		
		// Outputs the node cover matrix
		void outputNodeMatrix()
		{
			for (int i = 1; i < ROW_SIZE+1; i++)
			{
				std::cout << "Row " << i << ": ";
				for (int j = 1; j < COL_SIZE+1; j++)
				{
					if (nodeCoverMatrix[i][j].right != nullptr)
					{
						std::cout << j << ", ";
					}
				}
				std::cout << '\n';
			}			
		}
		
		// Outputs the column headers
		void outputHeaders()
		{
			Node* n;
			
			// Output headers to right
			n = headHeader->right;
			while (n != headHeader)
			{
				std::cout << n->col << " " << n->count << std::endl;
				n = n->right;
			}
			
			std::cout << std::endl;
			
			// Output headers to left
			n = headHeader->left;
			while (n != headHeader)
			{
				std::cout << n->col << " " << n->count << std::endl;
				n = n->left;
			}
		}
		
		// Outputs the cover matrix to a file
		void outputcoverMatrix(std::string filename)
		{
			std::ofstream outFile;
			outFile.open(filename);
			
			if (outFile.is_open())
			{
				for (int i = 0; i < ROW_SIZE; i++)
				{
					for (int j = 0; j < COL_SIZE; j++)
					{
						if (coverMatrix[i][j] == 1)
						outFile << '1';
						else
						outFile << ' ';
					}
					outFile << '\n';
				}
				
				outFile.close();
			}
		}
};