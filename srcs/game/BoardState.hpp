#ifndef BOARDSTATE_HPP
# define BOARDSTATE_HPP

# include <define.hpp>
# include <game/BitBoard.hpp>

class BoardState
{
public:
	BoardState(void);
	BoardState(BitBoard *plBitBoard, BitBoard *opBitBoard);
	BoardState(const BoardState &boardMemoryid);
	~BoardState();

	BoardState	&operator=(const BoardState &boardMemoryid);
	bool		operator==(const BoardState &boardMemoryid) const;

	std::size_t	getHash(void) const;

private:
	int	plBitBoard[GRID_W_INTER],
		opBitBoard[GRID_W_INTER];
};


// Custom specialization of std::hash can be injected in namespace std.
template<>
struct std::hash<BoardState>
{
	std::size_t operator()(const BoardState& boardMemoryid) const noexcept
	{
		return (boardMemoryid.getHash());
	}
};

#endif
