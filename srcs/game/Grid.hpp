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
	BitBoard		*getBitBoard(inter_type type);

	void			setBitBoard(BitBoard *bitBoard, inter_type type);

	Grid			&operator=(const Grid &grid);

	void			tick(display_state *displayState, Mouse *mouse, Player *leftPlayer, Player *rightPlayer,
							Evaluation *evaluator);
	void			draw(sf::RenderWindow *window, sf::Text *text, TextureManager *textureManager);
	bool			checkLegalMove(int x, int y, int nbMoves, inter_type plType, inter_type opType);
	bool			putStone(sf::Vector2i *move, int nbMoves, Player *player, Player *opponent);
	void			removeStone(sf::Vector2i *move, inter_type plType);
	bool			checkWinCondition(Player *player, Player *opponent);
	void			clearGrid(sprite_name leftStone, sprite_name rightStone, game_rules rule);
	void			reset(void);
	void			addBoardToHistory(void);
	void			goToHistoryStart(void);
	void			goToHistoryPrevious(void);
	void			goToHistoryNext(void);
	void			goToHistoryEnd(void);
	void			disablePreview(void);

	std::vector<sf::Vector2i>	getLegalMoves(Player *player, Player *opponent);
	std::vector<sf::Vector2i>	getInterestingMoves(Player *player, Player *opponent);
	// bool						checkInterestingMove(int x, int y);
	// std::vector<sf::Vector2i>	getInterestingMovesSorted(Player *player, Player *opponent, Evaluation *evaluator, bool reverse, Tracker *tracker);
	// void						computeInterestingMovesSorted(Player *player, Player *opponent, Evaluation *evaluator, Tracker *tracker);
	// void						updateInterestingMovesSorted(Player *player, Player *opponent, Evaluation *evaluator, Tracker *tracker, sf::Vector2i *move);
	// std::vector<sf::Vector2i>	getInterestingMovesSortedSaved(inter_type plType);
	// void						saveInterestingMovesSorted(std::vector<sf::Vector2i> *movesLeft, std::vector<sf::Vector2i> *movesRight);
	// void						loadInterestingMovesSorted(std::vector<sf::Vector2i> *movesLeft, std::vector<sf::Vector2i> *movesRight);
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

	bool	checkWinByAlign(Player *player, Player *opponent, BitBoard *plBitboard, BitBoard *opBitboard);
	bool	validateWin(Player *player, Player *opponent, BitBoard *plBitBoard, BitBoard *opBitBoard, bitboardAxis bbAxis, int x, int y);
	int		makeCapture(sf::Vector2i *move, BitBoard *plBitBoard, BitBoard *opBitBoard);
	bool	checkDoubleFreeThree(int x, int y, BitBoard *plBitBoard, BitBoard *opBitBoard);
	bool	checkProRule(int x, int y, inter_type interPlayer, int nbMoves);
	void	applyHistoryToGrid(void);

	// void			insertMoves(std::vector<sf::Vector2i> &moves, sf::Vector2i *move, bool reverse, Evaluation *evaluator, Player *player, Player *opponent);
};

#endif
