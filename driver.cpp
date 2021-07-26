#include "dlx.cpp"
#include "naive.cpp"
#include <chrono>
#include <fstream>
#include <string>

int main(int argc, char** argv)
{	
	int dlxTotalTime = 0;
	int naiveTotalTime = 0;
  
  std::string filename = argv[1];
	
	std::ifstream file;
  
  // DLX
	file.open(filename);
	if (file.is_open())
	{
		std::string line;
		
		std::cout << "dlx start\n";
		
		while (std::getline(file, line))
		{
			
			// Begin time
			auto start = std::chrono::high_resolution_clock::now();
			
			// Use Dancing Links and Algorithm X to find the solution to the sudoku puzzle
			DLX* d = new DLX(line);
			d->search();
			
			dlxTotalTime += std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();
			
			std::cout << "Puzzle time to solve: " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count() << '\t';
			std::cout << "Total time elapsed: " << dlxTotalTime << std::endl;
			
			delete(d);
			d = nullptr;
		}
		file.close();
	}
	
  // Naive
	file.open(filename);
	if (file.is_open())
	{
		std::string line;
		
		std::cout << "naive start";
		
		while (std::getline(file, line))
		{
			// Begin time
			auto start = std::chrono::high_resolution_clock::now();
			
			Naive *n = new Naive();
			try
			{
				n->runSolver(line);
			}
			catch (int n)
			{
				// Catch exception to end the recursion 
			}		
		
			naiveTotalTime += std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();
			std::cout << "Puzzle time to solve: " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count() << '\t';
			std::cout << "Total time elapsed: " << naiveTotalTime << std::endl;;
			
			delete(n);
			n = nullptr;
		}
		file.close();
	}
	
	std::cout << std::endl << "The total time taken for DLX: " << dlxTotalTime << "ms" << std::endl;
	std::cout << std::endl << "The total time taken for naive: " << naiveTotalTime << "ms" << std::endl;
	
	return 0;
}