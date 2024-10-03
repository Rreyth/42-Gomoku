#ifndef NODE_HPP
# define NODE_HPP

# include <define.hpp>
# include <game/BitBoard.hpp>
# include <game/PlayerInfo.hpp>
# include <game/Grid.hpp>
# include <AI/Evaluation.hpp>

# include <vector>

class Node
{
public:
	Node(void);
	Node(
		Grid *grid, PlayerInfo *player, PlayerInfo *opponent,
		bool playerTurn, Move *moveFrom);
	Node(const Node &obj);
	~Node();

	Move	*getMove(void);
	int		getNbMove(void);
	int		getKillerScore(void);

	Node	&operator=(const Node &obj);

	// Update node methods
	Node	*getActualNode(Grid *grid);
	Node	*getNextNode(sf::Vector2i *move);

	// Recursion methods
	int					getEvaluation(Evaluation *evaluator, Tracker *tracker);
	std::vector<Node>	*getChildren(Evaluation *evaluator, Tracker *tracker);
	void				applyMove(int depth, Tracker *tracker);
	bool				isKillerMove(void);

	// TODO: REMOVE
	void	printGrid(void);

private:
	int					evaluation, hash, killerMoveScore;
	bool				playerTurn,
						childrenCompute, evaluationCompute,
						moveCompute, bboxManagerUpdate,
						killerMoveCompute, killerMoveResult;
	Grid				grid;
	BitBoard			*plBitboard, *opBitboard;
	PlayerInfo			player, opponent;
	Move				lastMove;
	std::vector<Node>	children;
};

#endif
