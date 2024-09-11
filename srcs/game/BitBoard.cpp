#include <game/BitBoard.hpp>

////////////////////////////////////////////////////////////////////////////////
// Constructors and destructor
////////////////////////////////////////////////////////////////////////////////

BitBoard::BitBoard(void)
{
	this->clear();
}


BitBoard::BitBoard(const BitBoard &obj)
{
	for (int i = 0; i < GRID_W_INTER; i++)
	{
		this->bbH[i] = obj.bbH[i];
		this->bbV[i] = obj.bbV[i];
		this->bbD[i] = obj.bbD[i];
		this->bbA[i] = obj.bbA[i];
	}
}


BitBoard::~BitBoard()
{

}


////////////////////////////////////////////////////////////////////////////////
// Getters and setters
////////////////////////////////////////////////////////////////////////////////

bool	BitBoard::get(int x, int y)
{
	return (this->bbH[y] & 1 << (GRID_W_INTER - x - 1));
}


void	BitBoard::set(int x, int y, bool value)
{
	int	check, checkV, yD, yA;

	check = 1 << (GRID_W_INTER - x - 1);
	checkV = 1 << y;
	yD = (y + x) % GRID_W_INTER;
	yA = (y - x) % GRID_W_INTER;
	if (yA < 0)
		yA = GRID_W_INTER + yA;

	// Set value at true if it's not
	if (value && !(this->bbH[y] & check))
	{
		this->bbH[y] += check;
		this->bbV[x] += checkV;
		this->bbD[yD] += check;
		this->bbA[yA] += check;
	}
	// Set value at false if it's not
	else if (!value && (this->bbH[y] & check))
	{
		this->bbH[y] -= check;
		this->bbV[x] -= checkV;
		this->bbD[yD] -= check;
		this->bbA[yA] -= check;
	}
}

////////////////////////////////////////////////////////////////////////////////
// Operators
////////////////////////////////////////////////////////////////////////////////

BitBoard	&BitBoard::operator=(const BitBoard &obj)
{
	if (this == &obj)
		return (*this);

	for (int i = 0; i < GRID_W_INTER; i++)
	{
		this->bbH[i] = obj.bbH[i];
		this->bbV[i] = obj.bbV[i];
		this->bbD[i] = obj.bbD[i];
		this->bbA[i] = obj.bbA[i];
	}

	return (*this);
}


bool	BitBoard::operator==(const BitBoard &obj)
{
	if (this == &obj)
		return (true);

	for (int i = 0; i < GRID_W_INTER; i++)
	{
		if (this->bbH[i] != obj.bbH[i])
			return (false);
	}
	return (true);
}

////////////////////////////////////////////////////////////////////////////////
// Public methods
////////////////////////////////////////////////////////////////////////////////

void	BitBoard::clear(void)
{
	for (int i = 0; i < GRID_W_INTER; i++)
	{
		this->bbH[i] = 0;
		this->bbV[i] = 0;
		this->bbD[i] = 0;
		this->bbA[i] = 0;
	}
}

////////////////////////////////////////////////////////////////////////////////
// Private methods
////////////////////////////////////////////////////////////////////////////////
