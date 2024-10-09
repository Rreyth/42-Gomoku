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
	Node(const Node &obj);
	Node(
		PlayerInfo *player, PlayerInfo *opponent,
		Grid *grid, Move *move);
	~Node();

	sf::Vector2i		*getMove(void);

	Node				&operator=(const Node &obj);

	int					getKillerMove(
							Tracker *tracker);
	int					getEvaluation(
							Evaluation *evaluator, Tracker *tracker);
	void				appliedMove(
							int depth, Tracker *tracker);
	void				computeHash(void);
	std::vector<Node>	*getChildren(
							Evaluation *evaluator, Tracker *tracker);

	Node				*getRootNodeFromGrid(
							Grid *grid);
	Node				*getRootNodeFromMove(
							sf::Vector2i *move);

	// TODO: REMOVE
	void	printGrid(void);

private:
	PlayerInfo			player, opponent;
	BitBoard			*plBitboard, *opBitboard;
	Grid				grid;
	Move				move;
	int					evaluation, killerMoveResult, hash;
	bool				evaluationCompute, killerMoveCompute,
						bboxManagerUpdate, childrenCompute,
						moveApplied, hashCompute;
	std::vector<Node>	children;
};

#endif
