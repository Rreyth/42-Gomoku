#ifndef GRID_HPP
# define GRID_HPP

# include <define.hpp>
# include <ui/Button.hpp>
# include <game/BitBoard.hpp>

# include <vector>

class Player;
class Evaluation;

class Grid
{
public:
	Grid(void);
	~Grid();

	int				getX(void);
	int				getY(void);
	int				getW(void);
	int				getH(void);
	std::string		getHistoryIdString(void);
	inter_type		getInterState(int x, int y);
	intersection	*getIntersection(int x, int y);

	Grid			&operator=(const Grid &grid);

	void			tick(display_state *displayState, Mouse *mouse, Player *leftPlayer, Player *rightPlayer,
							Evaluation *evaluator);
	void			draw(sf::RenderWindow *window, sf::Text *text, TextureManager *textureManager);
	bool			checkLegalMove(int x, int y, int nbMoves, inter_type plType, inter_type opType);
	bool			putStone(sf::Vector2i *move, int nbMoves, Player *player, Player *opponent);
	bool			checkWinCondition(Player *player, Player *opponent);
	void			clearGrid(sprite_name leftStone, sprite_name rightStone, game_rules rule);
	void			reset(void);
	void			addBoardToHistory(void);
	void			goToHistoryStart(void);
	void			goToHistoryPrevious(void);
	void			goToHistoryNext(void);
	void			goToHistoryEnd(void);
	void			disablePreview(void);

	// std::string					getCurrentBoardState(void);
	// std::vector<sf::Vector2i>	getLegalMoves(Player *leftPlayer, Player *rightPlayer);
	// bool						checkInterestingMove(int x, int y);
	// std::vector<sf::Vector2i>	getInterestingMoves(Player *player, Player *opponent);
	// std::vector<sf::Vector2i>	getInterestingMovesSorted(Player *player, Player *opponent, Evaluation *evaluator, bool reverse, Tracker *tracker);
	// void						computeInterestingMovesSorted(Player *player, Player *opponent, Evaluation *evaluator, Tracker *tracker);
	// void						updateInterestingMovesSorted(Player *player, Player *opponent, Evaluation *evaluator, Tracker *tracker, sf::Vector2i *move);
	// std::vector<sf::Vector2i>	getInterestingMovesSortedSaved(inter_type plType);
	// void						saveInterestingMovesSorted(std::vector<sf::Vector2i> *movesLeft, std::vector<sf::Vector2i> *movesRight);
	// void						loadInterestingMovesSorted(std::vector<sf::Vector2i> *movesLeft, std::vector<sf::Vector2i> *movesRight);
	// void						removeStone(sf::Vector2i *move, Tracker *tracker);
	// void						resetGridByBoardState(std::string boardState, Tracker *tracker);
	// void						saveBbox(sf::Vector2i *bboxUL, sf::Vector2i *bboxDR);
	// void						loadBbox(sf::Vector2i *bboxUL, sf::Vector2i *bboxDR);
	// void						saveWinPos(std::vector<sf::Vector2i> *leftWinPos, std::vector<sf::Vector2i> *rightWinPos);
	// void						loadWinPos(std::vector<sf::Vector2i> *leftWinPos, std::vector<sf::Vector2i> *rightWinPos);

private:
	int											x, y, w, h, previewX, previewY, boardHistoryId;
	bool										previewLegal;
	sprite_name									leftStone, rightStone;
	game_rules									rule;
	std::string									historyIdString;
	BitBoard									bitboardL, bitboardR;
	std::vector<std::pair<BitBoard, BitBoard>>	boardHistory;

	// intersection				gridState[GRID_NB_INTER];
	// sf::Vector2i				dirs[8], bboxUL, bboxDR;
	// std::vector<sf::Vector2i>	leftWinPos, rightWinPos, interestingMovesLeft, interestingMovesRight;
	// std::vector<std::string>	boardStates;
	// std::string					currentBoardState, currentBoardStateOpti;

	bool	validateWin(Player *player, Player *opponent, BitBoard *plBitBoard, BitBoard *opBitBoard, char bbType, int x, int y);
	int		makeCapture(sf::Vector2i *move, BitBoard *plBitBoard, BitBoard *opBitBoard);
	bool	checkDoubleFreeThree(int x, int y, BitBoard *plBitBoard, BitBoard *opBitBoard);
	void	applyHistoryToGrid(void);

	// void			setInterState(int x, int y, inter_type interType);
	// bool			checkProRule(int x, int y, inter_type interPlayer, int nbMoves);
	// void			loopUpdateNeighbor(int x, int y, dir_neighbor dir, inter_type plType);
	// void			updateNeighbor(int x, int y);
	// bool			checkWinCaptureCase(Player *me, Player *oppenent, sf::Vector2i *move, dir_neighbor dir, dir_neighbor opdir);
	// void			setBoardState(int id);
	// void			createBoardState(void);
	// void			updateBoardState(int id, char c);
	// void			createCurrentMoveText(void);
	// void			insertMoves(std::vector<sf::Vector2i> &moves, sf::Vector2i *move, bool reverse, Evaluation *evaluator, Player *player, Player *opponent);
};

#endif
