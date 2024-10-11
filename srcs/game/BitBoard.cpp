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
		this->hash[i] = std::hash<int>{}(this->bbH[i]);
	}
}


BitBoard::~BitBoard()
{

}


////////////////////////////////////////////////////////////////////////////////
// Getters and setters
////////////////////////////////////////////////////////////////////////////////

bool	BitBoard::get(int x, int y) const
{
	return (this->bbH[y] & 1 << x);
}


void	BitBoard::set(int x, int y, bool value)
{
	int	check, checkV, yD, yA;

	check = 1 << x;
	checkV = 1 << y;
	yD = y + x;
	if (yD >= GRID_W_INTER)
		yD -= GRID_W_INTER;
	yA = y - x;
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

	// Update hash line
	this->hash[y] = std::hash<int>{}(this->bbH[y]);
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
		this->hash[i] = obj.hash[i];
	}

	return (*this);
}


bool	BitBoard::operator==(const BitBoard &obj) const
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
		this->hash[i] = 0;
	}
}


int	BitBoard::getLine(bitboardAxis bbAxis, int x, int y) const
{
	int	yTmp, tmp;

	// Get line horizontal
	if (bbAxis == BBH)
		return (this->bbH[y]);

	// Get line vertical
	if (bbAxis == BBV)
		return (this->bbV[x]);

	// Get line diagonal
	if (bbAxis == BBD)
	{
		yTmp = y + x;
		if (yTmp >= GRID_W_INTER)
			yTmp -= GRID_W_INTER;

		tmp = GRID_W_INTER - yTmp - 1; // @brenaudo validate this line
		if (x <= yTmp)
			return (this->bbD[yTmp] & (0b1111111111111111111 >> tmp));
		else
			return (this->bbD[yTmp] & ~(0b1111111111111111111 >> tmp));
	}

	// Get line anti diagonal
	yTmp = y - x;
	if (yTmp < 0)
		yTmp += GRID_W_INTER;
	tmp = GRID_W_INTER - yTmp;

	if (x >= tmp)
		return (this->bbA[yTmp] & (0b1111111111111111111 << tmp));
	else
		return (this->bbA[yTmp] & ~(0b1111111111111111111 << tmp));
}


std::size_t	BitBoard::getHash(BitBoard *bitboard)

{
	std::size_t	res;
	std::size_t	plTmp;
	std::size_t	opTmp;

	res = 0;
	for (int i = 0; i < GRID_W_INTER; i++)
	{
		plTmp = std::hash<int>{}(this->hash[i]);
		opTmp = std::hash<int>{}(bitboard->hash[i]);
		res = plTmp + 0x9e3779b9 + (res<<6) + (res>>2);
		res = opTmp + 0x9e3779b9 + (res<<6) + (res>>2);
	}

	return (res);
}

////////////////////////////////////////////////////////////////////////////////
// Private methods
////////////////////////////////////////////////////////////////////////////////
