#ifndef GRID_HPP
# define GRID_HPP

# include <define.hpp>
# include <ui/Button.hpp>
# include <game/Player.hpp>

# include <vector>

class Grid
{
public:
	Grid(void);
	~Grid();

	int				getX(void);
	int				getY(void);
	int				getW(void);
	int				getH(void);
	std::string		getCurrentMove(void);
	inter_type		getInterState(int x, int y);
	intersection	*getIntersection(int x, int y);

	void		tick(display_state *displayState, Mouse *mouse, Player *leftPlayer, Player *rightPlayer);
	void		draw(sf::RenderWindow *window, sf::Text *text, TextureManager *textureManager);
	void		clearGrid(sprite_name leftStone, sprite_name rightStone, game_rules rule);
	void		goToFirstMove(void);
	void		goToPreviousMove(void);
	void		goToNextMove(void);
	void		goToLastMove(void);

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

	void			setInterState(int x, int y, inter_type interType);
	bool			checkDoubleFreeThree(inter_type plState, inter_type opState);
	void			checkIfPreviewLegal(bool leftPlayer, int nbMoves);
	bool			checkProRule(inter_type interPlayer, int nbMoves);
	int				loopUpdateNeighbor(int x, int y, dir_neighbor dir, inter_type interType);
	void			updateNeighbor(int x, int y);
	int				checkCapture(void);
	bool			checkWinCaptureCase(Player *me, Player *oppenent, dir_neighbor dir, dir_neighbor opdir);
	bool			checkWinCondition(Player *me, Player *oppenent);
	void			addBoardState(void);
	void			setBoardState(int id);
	void			createCurrentMoveText(void);
};

#endif
