#include <iostream>

#define GRID_SIZE 8

void	printBits(char *grid)
{
	for (int y = 0; y < GRID_SIZE; y++)
	{
		if (y > 0)
			std::cout << std::endl;

		for (int x = 0; x < GRID_SIZE; x++)
		{
			if (grid[y] & 1 << (GRID_SIZE - x - 1))
				std::cout << "1 ";
			else
				std::cout << "0 ";
		}
	}
	std::cout << std::endl;
	std::cout << std::endl;
}


void	setBits(char *grid, int x, int y, bool value)
{
	char	check;
	int		shift;

	shift = GRID_SIZE - x - 1;
	check = 1 << shift;

	if (value && !(grid[y] & check))
		grid[y] += check;
	else if (!value && (grid[y] & check))
		grid[y] -= check;
}


bool	getBits(char *grid, int x, int y)
{
	return (grid[y] & 1 << (GRID_SIZE - x - 1));
}


int	main(void)
{
	char	grid[8] = {2, 0, 0, 0, 0, 0, 1 ,0};

	printBits(grid);

	setBits(grid, 6, 0, false);
	setBits(grid, 7, 0, true);
	setBits(grid, 1, 0, true);
	setBits(grid, 4, 2, true);

	printBits(grid);

	int	x, y;

	x = 4;
	y = 2;

	printf("x %i y %i = %i\n", x, y, getBits(grid, x, y));

	return (0);
}
