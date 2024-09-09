#ifndef GRID_HPP
# define GRID_HPP

# include <define.hpp>
# include <ui/Button.hpp>

# include <vector>

class Player;
class Evaluation;

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
	std::string					getCurrentBoardState(void);
	inter_type					getInterState(int x, int y);
	intersection				*getIntersection(int x, int y);

	Grid						&operator=(const Grid &grid);

	void						tick(display_state *displayState, Mouse *mouse, Player *leftPlayer, Player *rightPlayer,
										Evaluation *evaluator);
	void						draw(sf::RenderWindow *window, sf::Text *text, TextureManager *textureManager);
	void						clearGrid(sprite_name leftStone, sprite_name rightStone, game_rules rule);
	void						goToFirstMove(void);
	void						goToPreviousMove(void);
	void						goToNextMove(void);
	void						goToLastMove(void);
	bool						checkLegalMove(int x, int y, int nbMoves, inter_type plState, inter_type opState);
	std::vector<sf::Vector2i>	getLegalMoves(Player *leftPlayer, Player *rightPlayer);
	bool						checkInterestingMove(int x, int y);
	std::vector<sf::Vector2i>	getInterestingMoves(Player *player, Player *opponent);
	std::vector<sf::Vector2i>	getInterestingMovesSorted(Player *player, Player *opponent, Evaluation *evaluator, bool reverse, Tracker *tracker);
	bool						putStone(sf::Vector2i *move, int nbMoves, Player *player, Player *opponent);
	void						removeStone(sf::Vector2i *move);
	void						resetGridByBoardState(std::string boardState);
	void						saveBbox(sf::Vector2i *bboxUL, sf::Vector2i *bboxDR);
	void						loadBbox(sf::Vector2i *bboxUL, sf::Vector2i *bboxDR);
	bool						checkWinCondition(Player *me, Player *oppenent, sf::Vector2i move);
	void						addBoardState(void);
	void						disablePreview(void);
	void						reset(void);

private:
	int							x, y, w, h, previewX, previewY, idBoardState;
	bool						previewLegal;
	intersection				gridState[GRID_NB_INTER];
	sf::Vector2i				dirs[8], bboxUL, bboxDR;
	sprite_name					leftStone, rightStone;
	game_rules					rule;
	std::vector<sf::Vector2i>	leftWinPos, rightWinPos;
	std::vector<std::string>	boardStates;
	std::string					currentBoardState;
	std::string					currentMove;

	void			setInterState(int x, int y, inter_type interType);
	bool			checkDoubleFreeThree(int x, int y, inter_type plType, inter_type opType);
	int				checkCapture(sf::Vector2i *move, inter_type plType, inter_type opType);
	bool			checkProRule(int x, int y, inter_type interPlayer, int nbMoves);
	void			loopUpdateNeighbor(int x, int y, dir_neighbor dir, inter_type plType);
	void			updateNeighbor(int x, int y);
	bool			checkWinCaptureCase(Player *me, Player *oppenent, sf::Vector2i *move, dir_neighbor dir, dir_neighbor opdir);
	void			setBoardState(int id);
	void			createBoardState(void);
	void			createCurrentMoveText(void);
};

#endif
