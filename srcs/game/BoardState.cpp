#include <game/BoardState.hpp>

////////////////////////////////////////////////////////////////////////////////
// Constructors and destructor
////////////////////////////////////////////////////////////////////////////////

BoardState::BoardState(void)
{
	for (int i = 0; i < GRID_W_INTER; i++)
	{
		this->plBitBoard[i] = 0;
		this->opBitBoard[i] = 0;
	}
}


BoardState::BoardState(BitBoard *plBitBoard, BitBoard *opBitBoard)
{
	for (int i = 0; i < GRID_W_INTER; i++)
	{
		this->plBitBoard[i] = plBitBoard->bbH[i];
		this->opBitBoard[i] = opBitBoard->bbH[i];
	}
}


BoardState::BoardState(const BoardState &obj)
{
	for (int i = 0; i < GRID_W_INTER; i++)
	{
		this->plBitBoard[i] = obj.plBitBoard[i];
		this->opBitBoard[i] = obj.opBitBoard[i];
	}
}


BoardState::~BoardState()
{

}


////////////////////////////////////////////////////////////////////////////////
// Getters and setters
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Operators
////////////////////////////////////////////////////////////////////////////////

BoardState	&BoardState::operator=(const BoardState &boardMemoryid)
{
	if (this == &boardMemoryid)
		return (*this);
	for (int i = 0; i < GRID_W_INTER; i++)
	{
		this->plBitBoard[i] = boardMemoryid.plBitBoard[i];
		this->opBitBoard[i] = boardMemoryid.opBitBoard[i];
	}
	return (*this);
}

bool	BoardState::operator==(const BoardState &boardMemoryid) const
{
	for (int i = 0; i < GRID_W_INTER; i++)
	{
		if (this->plBitBoard[i] != boardMemoryid.plBitBoard[i]
				|| this->opBitBoard[i] != boardMemoryid.opBitBoard[i])
			return (false);
	}
	return (true);
}


////////////////////////////////////////////////////////////////////////////////
// Public methods
////////////////////////////////////////////////////////////////////////////////

std::size_t	BoardState::getHash(void) const
{
	std::size_t	res;
	std::size_t	plTmp;
	std::size_t	opTmp;
	
	res = 0;
	for (int i = 0; i < GRID_W_INTER; i++)
	{
		plTmp = std::hash<int>{}(this->plBitBoard[i]);
		opTmp = std::hash<int>{}(this->opBitBoard[i]);
		res = plTmp + 0x9e3779b9 + (res<<6) + (res>>2);
		res = opTmp + 0x9e3779b9 + (res<<6) + (res>>2);
	}
	return (res);
}

////////////////////////////////////////////////////////////////////////////////
// Private methods
////////////////////////////////////////////////////////////////////////////////
