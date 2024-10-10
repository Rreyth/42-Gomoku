#ifndef GRID_HPP
# define GRID_HPP

# include <define.hpp>
# include <ui/Button.hpp>
# include <game/BitBoard.hpp>
# include <game/BboxManager.hpp>
# include <game/PlayerInfo.hpp>

# include <vector>
# include <unordered_map>

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
	BboxManager		*getBboxManager(void);

	void			setBitBoard(BitBoard *bitBoard, inter_type type);
	void			setBboxManager(BboxManager *bboxManager);

	Grid			&operator=(const Grid &grid);

	void			tick(display_state *displayState, Mouse *mouse,
							Player *leftPlayer, Player *rightPlayer,
							Evaluation *evaluator);
	void			draw(sf::RenderWindow *window, sf::Text *text,
							TextureManager *textureManager);
	bool			checkLegalMove(int x, int y, int nbMoves,
									inter_type plType, inter_type opType);
	bool			putStone(sf::Vector2i *move, int nbMoves,
								PlayerInfo *player, PlayerInfo *opponent);
	bool			putStoneAI(sf::Vector2i *move, int nbMoves,
								PlayerInfo *player, PlayerInfo *opponent);
	void			removeStone(sf::Vector2i *move, inter_type plType);
	bool			checkWinCondition(PlayerInfo *player, PlayerInfo *opponent);
	void			updateBboxManager(sf::Vector2i *move);
	void			clearGrid(sprite_name leftStone, sprite_name rightStone, game_rules rule);
	void			reset(void);
	void			addBoardToHistory(void);
	void			goToHistoryStart(void);
	void			goToHistoryPrevious(void);
	void			goToHistoryNext(void);
	void			goToHistoryEnd(void);
	void			disablePreview(void);

	std::vector<sf::Vector2i>	getLegalMoves(PlayerInfo *player, PlayerInfo *opponent);
	std::vector<sf::Vector2i>	getInterestingMoves(PlayerInfo *player, PlayerInfo *opponent);
	void			getInterestingMovesSorted(
									std::vector<Move> *moves,
									Evaluation *evaluator,
									PlayerInfo *player, PlayerInfo *opponent,
									bool reverse, Tracker *tracker);

private:
	int											x, y, w, h, previewX, previewY, boardHistoryId;
	bool										previewLegal;
	sprite_name									leftStone, rightStone;
	game_rules									rule;
	std::string									historyIdString;
	BitBoard									bitboardL, bitboardR;
	BboxManager									bboxManager;
	std::vector<std::pair<BitBoard, BitBoard>>	boardHistory;

	bool	checkWinByAlign(PlayerInfo *player, PlayerInfo *opponent, BitBoard *plBitboard, BitBoard *opBitboard);
	bool	validateWin(PlayerInfo *player, PlayerInfo *opponent, BitBoard *plBitBoard, BitBoard *opBitBoard, bitboardAxis bbAxis, int x, int y);
	int		makeCapture(sf::Vector2i *move, BitBoard *plBitBoard, BitBoard *opBitBoard);
	bool	checkDoubleFreeThree(int x, int y, BitBoard *plBitBoard, BitBoard *opBitBoard);
	bool	checkProRule(int x, int y, inter_type interPlayer, int nbMoves);
	void	applyHistoryToGrid(void);

};

#endif
