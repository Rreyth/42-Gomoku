#include <iostream>

#define GRID_W_INTER 8

typedef struct	s_bitboard
{
	char	bbH[GRID_W_INTER];
	char	bbV[GRID_W_INTER];
	char	bbD[GRID_W_INTER];
	char	bbA[GRID_W_INTER];
}	t_bitboard;

typedef enum e_bitboardAxis
{
	BBH = 0,
	BBV,
	BBD,
	BBA
}	bitboardAxis;


void	printBitLine(char line)
{
	for (int x = 0; x < GRID_W_INTER; x++)
	{
		if (line & 1 << (GRID_W_INTER - x - 1))
			std::cout << "1";
		else
			std::cout << "0";
	}
	std::cout << std::endl;
}


void	printBits(char *grid)
{
	for (int y = 0; y < GRID_W_INTER; y++)
	{
		if (y > 0)
			std::cout << std::endl;

		for (int x = 0; x < GRID_W_INTER; x++)
		{
			if (grid[y] & 1 << (GRID_W_INTER - x - 1))
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
	for (int y = 0; y < GRID_W_INTER; y++)
	{
		if (y > 0)
			std::cout << std::endl;

		for (int x = 0; x < GRID_W_INTER; x++)
		{
			if (grid->bbH[y] & 1 << (GRID_W_INTER - x - 1))
				std::cout << "1 ";
			else
				std::cout << "0 ";
		}

		std::cout << "   ";

		for (int x = 0; x < GRID_W_INTER; x++)
		{
			if (grid->bbV[y] & 1 << (GRID_W_INTER - x - 1))
				std::cout << "1 ";
			else
				std::cout << "0 ";
		}

		std::cout << "   ";

		for (int x = 0; x < GRID_W_INTER; x++)
		{
			if (grid->bbD[y] & 1 << (GRID_W_INTER - x - 1))
				std::cout << "1 ";
			else
				std::cout << "0 ";
		}

		std::cout << "   ";

		for (int x = 0; x < GRID_W_INTER; x++)
		{
			if (grid->bbA[y] & 1 << (GRID_W_INTER - x - 1))
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

	shift = GRID_W_INTER - x - 1;
	check = 1 << shift;

	if (value && !(grid[y] & check))
		grid[y] += check;
	else if (!value && (grid[y] & check))
		grid[y] -= check;
}


bool	getBits(char *grid, int x, int y)
{
	return (grid[y] & 1 << (GRID_W_INTER - x - 1));
}


void	putStone(t_bitboard *grid, int x, int y, bool value)
{
	char	check, checkV;
	int		yV, yD, yA;

	check = 1 << x;
	checkV = 1 << y;
	yV = x;
	yD = (y + x) % GRID_W_INTER;
	yA = (y - x) % GRID_W_INTER;
	if (yA < 0)
		yA = GRID_W_INTER + yA;


	// Set stone at true if it's not
	if (value && !(grid->bbH[y] & check))
	{
		grid->bbH[y] += check;
		grid->bbV[yV] += checkV;
		grid->bbD[yD] += check;
		grid->bbA[yA] += check;
	}
	// Set stone at false if it's not
	else if (!value && (grid->bbH[y] & check))
	{
		grid->bbH[y] -= check;
		grid->bbV[yV] -= checkV;
		grid->bbD[yD] -= check;
		grid->bbA[yA] -= check;
	}
}


void	clearGrid(t_bitboard *grid)
{
	for (int i = 0; i < GRID_W_INTER; i++)
	{
		grid->bbH[i] = 0;
		grid->bbV[i] = 0;
		grid->bbD[i] = 0;
		grid->bbA[i] = 0;
	}
}


int	getLine(t_bitboard *grid, bitboardAxis bbAxis, int x, int y)
{
	int	yTmp, tmp;

	// Get line horizontal
	if (bbAxis == BBH)
		return (grid->bbH[y]);

	// Get line vertical
	if (bbAxis == BBV)
		return (grid->bbV[x]);

	// Get line diagonal
	if (bbAxis == BBD)
	{
		yTmp = (y + x) % GRID_W_INTER;

		tmp = GRID_W_INTER - yTmp - 1; // @brenaudo validate this line
		if (x <= yTmp)
			return (grid->bbD[yTmp] & (0b11111111 >> tmp));
		else
			return (grid->bbD[yTmp] & ~(0b11111111 >> tmp));
	}

	// Get line anti diagonal
	yTmp = y - x;
	if (yTmp < 0)
		yTmp += GRID_W_INTER;
	tmp = GRID_W_INTER - yTmp;

	if (x >= tmp)
		return (grid->bbA[yTmp] & (0b11111111 << tmp));
	else
		return (grid->bbA[yTmp] & ~(0b11111111 << tmp));
}


int	main(void)
{
	t_bitboard	grid = {0};

	clearGrid(&grid);

	putStone(&grid, 7, 0, true);

	printGrid(&grid);

	int				x, y, line;
	bitboardAxis	axis;

	x = 7;
	y = 1;
	axis = BBA;

	line = getLine(&grid, axis, x, y);

	char	tkt[] = "HVDA";
	printf("line %c for %i %i is ", tkt[axis], x, y);
	printBitLine(line);

	return (0);
}
