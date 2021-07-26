/**
    The MIT License (MIT)

    Copyright (c) 2014 Rafal Szymanski <rafalio>

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
    
    https://github.com/rafalio/dancing-links-java/blob/master/src/dlx/NaiveSudokuSolver.java/
**/

#include <iostream>
#include <string>
#include <vector>
 
class Naive
{
	private:
		
	
		void solve(std::vector<std::vector<int>> board, int cell)
		{			
			if(cell == 81){
				printSolution(board);
				throw 0;
			}
			else{
				int row = cell / 9;
				int col = cell % 9;
				
				// Advance forward on cells that are prefilled
				if (board[row][col] != 0) solve(board, cell+1);
				else
				{
					// we are positioned on something we need to fill in. Try all possibilities
					for (int i = 1; i <= 9; i++)
					{
						if(consistent(board, row, col, i)){
							board[row][col] = i;
							solve(board,cell+1);
							board[row][col] = 0; // unmake move
						}   
					}
				}
				// no solution
			}
    }
      
    // Check whether putting "c" into index "ind" leaves the board in a consistent state
    bool consistent(std::vector<std::vector<int>> board, int row, int col, int c) 
		{        
			// check columns/rows
			for (int i = 0; i < 9; i++)
			{
				if (board[row][i] == c) return false;
				if (board[i][col] == c) return false;
			}
			
			// Check subsquare
			
			int rowStart = row - row % 3; 
			int colStart = col - col % 3;
			
			for (int m = 0; m < 3; m++)
			{
				for (int k = 0; k < 3; k++)
				{
					if (board[rowStart + k][colStart + m] == c) return false;
				}
			}
			return true;
    }
		
		std::vector<std::vector<int>> sudokuStringToMatrix(std::string s)
		{
			std::vector<std::vector<int>> board (9, std::vector<int>(9, 0));
			for (int i = 0; i < s.size(); i++)
			{
				if (s.at(i) != '-')
				{
					board[(i/9)][i%9] = s.at(i)-'0';
				}
				else
				{
					board[i/9][i%9] = 0;
				}
			}
			std::cout << std::endl;
			return board;
		}    
   
    public:
			void runSolver(std::string sudokuString){
				solve(sudokuStringToMatrix(sudokuString),0);
			} 
			
			void printSolution(std::vector<std::vector<int>> board)
			{
        for (int i = 0; i < 9; i++)
			{
				for (int j = 0; j < 9; j++)
				{
					std::cout << board[i][j] << " | ";
					if (j == 8)
					{
						std::cout << std::endl;
					}
				}
			}
			}
};