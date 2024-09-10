#include <iostream>

#define BOARD_SIZE 289
#define BOARD_LINE_SIZE 17

#define OPTI_BOARD_SIZE 73
#define OPTI_BOARD_LINE_SIZE 5

void	printBoard(std::string &board)
{
	for (int i = 0; i < BOARD_SIZE; i++)
	{
		if (i > 0 && i % BOARD_LINE_SIZE == 0)
			std::cout << std::endl;
		if (board[i] == 'E')
			std::cout << ".  ";
		else
			std::cout << board[i] << "  ";
	}
	std::cout << std::endl;
}


void	printOptiBoard(std::string &board)
{
	char	tmp;
	char	bitPrint[8];

	for (int i = 0; i < OPTI_BOARD_SIZE; i++)
	{
		// if (i > 0 && i % OPTI_BOARD_LINE_SIZE == 0)
		// 	std::cout << std::endl;

		tmp = board[i];

		for (int j = 0; j < 8; j++)
		{
			if (tmp & 1 << (7 - j))
				bitPrint[j] = '1';
			else
				bitPrint[j] = '0';
		}

		for (int j = 0; j < 4; j++)
		{
			if (i > 0 && (i * 4 + j) % 17 == 0)
				std::cout << std::endl;

			if (bitPrint[j * 2] == '0' && bitPrint[j * 2 + 1] == '0')
				std::cout << ".. ";
			else
			{
				std::cout << bitPrint[j * 2];
				std::cout << bitPrint[j * 2 + 1];
				std::cout << " ";
			}
		}
	}
	std::cout << std::endl;
}


std::string	createOptiBoardFromBoard(std::string &board)
{
	char		tmp;
	std::string	optiBoard = "                                                                         ";

	for (int i = 0; i < OPTI_BOARD_SIZE; i++)
	{
		tmp = 0;

		for (int j = 0; j < 4; j++)
		{
			int id = (i + 1) * 4 - (j + 1);

			if (id >= BOARD_SIZE)
				continue;

			if (board[id] == 'E')
				tmp += 0b00 << (j * 2);
			else if (board[id] == 'L')
				tmp += 0b01 << (j * 2);
			else if (board[id] == 'R')
				tmp += 0b10 << (j * 2);
			else
				tmp += 0b11 << (j * 2);
		}

		optiBoard[i] = tmp;
	}

	return (optiBoard);
}


void	setOptiBoard(std::string &board, int id, char c)
{
	int		i, j;
	char	tmp, newChar;

	i = id / 4;
	j = 3 - (id % 4);

	tmp = board[i];
	newChar = 0;

	for (int k = 0; k < 4; k++)
	{
		if (k == j)
		{
			if (c == 'E')
				newChar += 0b00 << (k * 2);
			else if (c == 'L')
				newChar += 0b01 << (k * 2);
			else if (c == 'R')
				newChar += 0b10 << (k * 2);
			else
				newChar += 0b11 << (k * 2);
		}
		else
		{
			newChar += tmp & 0b11 << (k * 2);
		}
	}

	board[i] = newChar;
}


int	main(void)
{
	std::string board = "EEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEREEEEEEEEEEEEEEELLEEEEEEEEEEEEEELRRRLEEEEEEEEEEELEEEEEEEEEEEEEEEREEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE";

	printBoard(board);

	std::string optiBoard = "";

	optiBoard = createOptiBoardFromBoard(board);

	std::cout << std::endl;

	printOptiBoard(optiBoard);

	std::cout << std::endl;

	int		idChange = 142;
	char	charChange = 'E';
	board[idChange] = charChange;
	setOptiBoard(optiBoard, idChange, charChange);

	printBoard(board);
	std::cout << std::endl;
	printOptiBoard(optiBoard);

	std::cout << std::endl;
	std::cout << "size of board : " << board.capacity() << std::endl;
	std::cout << "size of opti  : " << optiBoard.capacity() << std::endl;

	return (0);
}
