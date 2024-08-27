#ifndef GRID_HPP
# define GRID_HPP

# include <define.hpp>
# include <ui/Button.hpp>
# include <game/Player.hpp>

# define GRID_SQUARE_HALF_SIZE 17
# define GRID_SQUARE_SIZE 34
# define GRID_SIZE 647
# define GRID_W_INTER 18
# define GRID_NB_INTER 324 // 324 = 18 * 18

typedef enum e_dir_neighbor
{
	DIR_L = 0,
	DIR_UL,
	DIR_U,
	DIR_UR,
	DIR_R,
	DIR_DR,
	DIR_D,
	DIR_DL,
}	dir_neighbor;


typedef enum e_inter_type
{
	INTER_EMPTY,
	INTER_LEFT,
	INTER_RIGHT,
	INTER_INVALID,
}	inter_type;


typedef struct s_intersection
{
	inter_type	type;
	int			neighbor[8];
}	intersection;


class Grid
{
public:
	Grid(void);
	~Grid();

	int			getX(void);
	int			getY(void);
	int			getW(void);
	int			getH(void);

	void		tick(display_state *displayState, Mouse *mouse, Player *leftPlayer, Player *rightPlayer);
	void		draw(sf::RenderWindow *window, sf::Text *text, TextureManager *textureManager);
	void		clearGrid(void);

private:
	int				x, y, w, h, previewX, previewY;
	bool			previewLegal;
	intersection	gridState[GRID_NB_INTER];
	sf::Vector2i	dirs[8];

	intersection	*getIntersection(int x, int y);
	inter_type		getInterState(int x, int y);
	void			setInterState(int x, int y, inter_type interType);
	void			checkIfPreviewLegal(bool leftPlayer);
	int				loopUpdateNeighbor(int x, int y, dir_neighbor dir, inter_type interType);
	void			updateNeighbor(int x, int y);
	int				checkCapture(void);
	bool			checkWinCondition(Player *me, Player *oppenent);
	bool			checkDoubleFreeThree(inter_type interType, sf::Vector2i ignoreDir);
};

#endif
