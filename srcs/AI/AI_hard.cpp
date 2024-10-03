#include <AI/AI.hpp>
#include <utils/Functions.hpp>

static Move	negaMax(
				Node *node, Evaluation *evaluator,
				int alpha, int beta, int depth, Tracker *tracker);


sf::Vector2i	getHardMove(
					Node *rootNode, Evaluation *evaluator, Tracker *tracker)
{
	Move	bestMove;

	bestMove = negaMax(rootNode, evaluator,
						-1000000001, 1000000001, 1, tracker);

	return (bestMove.pos);
}


static Move	negaMax(
				Node *node, Evaluation *evaluator,
				int alpha, int beta, int depth, Tracker *tracker)
{
	Move				bestMove, tmpMove;
	Node				*child;
	std::vector<Node>	*children;

	// In case of depth 0, evaluate the grid
	if (depth <= 0)
	{
		tmpMove.pos = sf::Vector2i(-1, -1);
		tmpMove.eval = node->getEvaluation(evaluator, tracker);
		return (tmpMove);
	}

	// Get children of node
	children = node->getChildren(evaluator, tracker);

	// If there is no children
	if (children->size() == 0)
	{
		tmpMove.eval = 0;
		// If it's the first move, put stone at center of grid
		if (node->getNbMove() == 0)
			tmpMove.pos = sf::Vector2i(GRID_W_INTER / 2, GRID_W_INTER / 2);
		// Else it's an end game
		else
			tmpMove.pos = sf::Vector2i(-1, -1);
		return (tmpMove);
	}

	bestMove.pos = sf::Vector2i(-1, -1);
	bestMove.eval = -1000000001;
	for (int i = 0; i < children->size(); i++)
	{
		// Get child
		child = &children->at(i);

		// Apply move to child
		child->applyMove(depth, tracker);

		if (child->isKillerMove())
		{
			tmpMove.eval = child->getKillerScore();
		}
		else
		{
			// Get evaluation of the position
			tmpMove = negaMax(child, evaluator, -beta, -alpha, depth - 1, tracker);
		}
		tmpMove.eval = -tmpMove.eval;

		if (tmpMove.eval > bestMove.eval)
		{
			bestMove.eval = tmpMove.eval;
			bestMove.pos = child->getMove()->pos;
		}

		if (tmpMove.eval > alpha)
			alpha = tmpMove.eval;

		if (alpha >= beta)
			break;
	}

	return (bestMove);
}
