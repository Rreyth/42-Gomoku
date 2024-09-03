#ifndef GRID_HPP
# define GRID_HPP

# include <define.hpp>
# include <ui/Button.hpp>

# include <vector>

# define GRID_SQUARE_HALF_SIZE 17
# define GRID_SQUARE_SIZE 34
# define GRID_SIZE 633
# define GRID_W_INTER 17
# define GRID_NB_INTER 289 // 289 = 17 * 17

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
	INTER_EMPTY = 0,
	INTER_LEFT,
	INTER_RIGHT,
	INTER_INVALID,
}	inter_type;


typedef struct s_intersection
{
	inter_type	type;
	int			neighbor[8];
}	intersection;

class Player;

class Grid
{
public:
	Grid(void);
	~Grid();

	int							getX(void);
	int							getY(void);
	int							getW(void);
	int							getH(void);
	std::string					getCurrentMove(void);

	void						tick(display_state *displayState, Mouse *mouse, Player *leftPlayer, Player *rightPlayer);
	void						draw(sf::RenderWindow *window, sf::Text *text, TextureManager *textureManager);
	void						clearGrid(sprite_name leftStone, sprite_name rightStone, game_rules rule);
	void						goToFirstMove(void);
	void						goToPreviousMove(void);
	void						goToNextMove(void);
	void						goToLastMove(void);
	int							checkCapture(void);
	bool						checkLegalMove(int x, int y, int nbMoves, inter_type plState, inter_type opState);
	std::vector<sf::Vector2i>	getLegalMoves(Player *leftPlayer, Player *rightPlayer);
	bool						putStone(sf::Vector2i move, int nbMoves, inter_type plState, inter_type opState);
	bool						checkWinCondition(Player *me, Player *oppenent);

private:
	int							x, y, w, h, previewX, previewY, idBoardState;
	bool						previewLegal;
	intersection				gridState[GRID_NB_INTER];
	sf::Vector2i				dirs[8];
	sprite_name					leftStone, rightStone;
	game_rules					rule;
	std::vector<sf::Vector2i>	leftWinPos, rightWinPos;
	std::vector<std::string>	boardStates;
	std::string					currentMove;

	intersection				*getIntersection(int x, int y);
	inter_type					getInterState(int x, int y);
	void						setInterState(int x, int y, inter_type interType);
	bool						checkDoubleFreeThree(int x, int y, inter_type plState, inter_type opState);
	void						checkIfPreviewLegal(bool leftPlayer, int nbMoves);
	bool						checkProRule(int x, int y, inter_type interPlayer, int nbMoves);
	int							loopUpdateNeighbor(int x, int y, dir_neighbor dir, inter_type interType);
	void						updateNeighbor(int x, int y);
	bool						checkWinCaptureCase(Player *me, Player *oppenent, dir_neighbor dir, dir_neighbor opdir);
	void						addBoardState(void);
	void						setBoardState(int id);
	void						createCurrentMoveText(void);
};

#endif
