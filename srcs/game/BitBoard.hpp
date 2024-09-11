#ifndef BITBOARD_HPP
# define BITBOARD_HPP

# include <define.hpp>

class BitBoard
{
public:
	// bb = bitboard
	// H = horizontal | V = vertical | D = diagonal | A = anti diagonal
	int	bbH[GRID_W_INTER];
	int	bbV[GRID_W_INTER];
	int	bbD[GRID_W_INTER];
	int	bbA[GRID_W_INTER];

	BitBoard(void);
	BitBoard(const BitBoard &obj);
	~BitBoard();

	bool		get(int x, int y);
	void		set(int x, int y, bool value);

	BitBoard	&operator=(const BitBoard &obj);
	bool		operator==(const BitBoard &obj);

	void		clear(void);

private:
};

#endif
