#ifndef BOARDSTATE_HPP
# define BOARDSTATE_HPP

# include <define.hpp>
# include <game/BitBoard.hpp>

class BoardState
{
public:
	int	plBitBoard[GRID_W_INTER],
		opBitBoard[GRID_W_INTER];

	BoardState(void);
	BoardState(BitBoard *bitBoardL, BitBoard *bitBoardR);
	BoardState(const BoardState &boardMemoryid);
	~BoardState();

	BoardState	&operator=(const BoardState &boardMemoryid)
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

	bool	operator==(const BoardState &boardMemoryid) const
	{
		for (int i = 0; i < GRID_W_INTER; i++)
		{
			if (this->plBitBoard[i] != boardMemoryid.plBitBoard[i]
					|| this->opBitBoard[i] != boardMemoryid.opBitBoard[i])
				return (false);
		}
		return (true);
	}


};
// Custom specialization of std::hash can be injected in namespace std.
template<>
struct std::hash<BoardState>
{
	std::size_t operator()(const BoardState& boardMemoryid) const noexcept
	{
		std::size_t	res;
		std::size_t	plTmp;
		std::size_t	opTmp;
		res = 0;
		for (int i = 0; i < GRID_W_INTER; i++)
		{
			plTmp = std::hash<int>{}(boardMemoryid.plBitBoard[i]);
			opTmp = std::hash<int>{}(boardMemoryid.opBitBoard[i]);
			res = plTmp + 0x9e3779b9 + (res<<6) + (res>>2);
			res = opTmp + 0x9e3779b9 + (res<<6) + (res>>2);
		}
		return (res);
	}
};

#endif
