#include <iostream>

#define GRID_SIZE 8

typedef struct	s_bitboard
{
	char	bbH[GRID_SIZE];
	char	bbV[GRID_SIZE];
	char	bbD[GRID_SIZE];
	char	bbA[GRID_SIZE];
}	t_bitboard;


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
}


void	printGrid(t_bitboard *grid)
{
	std::cout << "  horizontal          "
				<< "vertical           "
				<< "diagonal             "
				<< "anti" << std::endl;
	for (int y = 0; y < GRID_SIZE; y++)
	{
		if (y > 0)
			std::cout << std::endl;

		for (int x = 0; x < GRID_SIZE; x++)
		{
			if (grid->bbH[y] & 1 << (GRID_SIZE - x - 1))
				std::cout << "1 ";
			else
				std::cout << "0 ";
		}

		std::cout << "   ";

		for (int x = 0; x < GRID_SIZE; x++)
		{
			if (grid->bbV[y] & 1 << (GRID_SIZE - x - 1))
				std::cout << "1 ";
			else
				std::cout << "0 ";
		}

		std::cout << "   ";

		for (int x = 0; x < GRID_SIZE; x++)
		{
			if (grid->bbD[y] & 1 << (GRID_SIZE - x - 1))
				std::cout << "1 ";
			else
				std::cout << "0 ";
		}

		std::cout << "   ";

		for (int x = 0; x < GRID_SIZE; x++)
		{
			if (grid->bbA[y] & 1 << (GRID_SIZE - x - 1))
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


void	putStone(t_bitboard *grid, int x, int y, bool value)
{
	char	check, checkV;
	int		yD, yA;

	check = 1 << (GRID_SIZE - x - 1);
	checkV = 1 << y;
	yD = (y + x) % GRID_SIZE;
	yA = (y - x) % GRID_SIZE;
	if (yA < 0)
		yA = GRID_SIZE + yA;

	// Set stone at true if it's not
	if (value && !(grid->bbH[y] & check))
	{
		grid->bbH[y] += check;
		grid->bbV[x] += checkV;
		grid->bbD[yD] += check;
		grid->bbA[yA] += check;
	}
	// Set stone at false if it's not
	else if (!value && (grid->bbH[y] & check))
	{
		grid->bbH[y] -= check;
		grid->bbV[x] -= checkV;
		grid->bbD[yD] -= check;
		grid->bbA[yA] -= check;
	}
}


int	main(void)
{
	t_bitboard	grid = {0};

	printGrid(&grid);

	putStone(&grid, 3, 0, true);
	putStone(&grid, 4, 0, true);
	putStone(&grid, 5, 0, true);
	putStone(&grid, 6, 0, true);
	putStone(&grid, 7, 0, true);

	printGrid(&grid);

	int	x, y;

	x = 4;
	y = 2;

	char verify = 0b11111;
	char tmp = 0b01011111 & 0b00111110;

	if (tmp == verify)
		printf("tkt %b\n", tmp);
	else
		printf("nop %b\n", tmp);

	return (0);
}
