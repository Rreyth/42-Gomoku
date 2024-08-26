#ifndef GRID_HPP
# define GRID_HPP

# include <define.hpp>
# include <Button.hpp>
# include <Player.hpp>

# define GRID_SQUARE_HALF_SIZE 17
# define GRID_SQUARE_SIZE 34
# define GRID_W_INTER 18
# define GRID_NB_INTER 324 // 324 = 18 * 18

typedef enum e_inter_type
{
	INTER_EMPTY,
	INTER_LEFT,
	INTER_RIGHT,
	INTER_INVALID,
}	inter_type;

class Grid
{
public:
	Grid(void);
	~Grid();

	int			getX(void);
	int			getY(void);
	int			getW(void);
	int			getH(void);

	void		tick(Mouse *mouse, Player *leftPlayer, Player *rightPlayer, std::string *turn);
	void		draw(sf::RenderWindow *window, sf::Text *text, TextureManager *textureManager);
	void		clearGrid(void);

private:
	int			x, y, w, h;
	inter_type	gridState[GRID_NB_INTER];

	inter_type	getInterState(int x, int y);
	void		setInterState(int x, int y, inter_type interType);
};

#endif
