#include <AI/AI.hpp>
#include <utils/Functions.hpp>

sf::Vector2i	getEasyMove(
						Grid *grid, PlayerInfo *player, PlayerInfo *opponent,
						Evaluation *evaluator)
{
	int							bestEval, tmpEval, plCapture, opCapture;
	sf::Vector2i				move;
	std::vector<sf::Vector2i>	interestingMoves;
	BitBoard					*plBitBoard, *opBitBoard;

	// Get interesting move
	interestingMoves = grid->getInterestingMoves(player, opponent);
	if (interestingMoves.size() == 0)
	{
		if (player->nbMove + opponent->nbMove == 0)
			return(sf::Vector2i(GRID_W_INTER / 2, GRID_W_INTER / 2));
		else
			return (sf::Vector2i(-1, -1));
	}

	// Compute variables for evaluation
	plCapture = player->nbCapture;
	opCapture = opponent->nbCapture;
	plBitBoard = grid->getBitBoard(player->interType);
	opBitBoard = grid->getBitBoard(opponent->interType);

	// Find move to play
	move = sf::Vector2i(-1, -1);
	bestEval = -1;
	for (int i = 0; i < interestingMoves.size(); i++)
	{
		tmpEval = evaluator->evaluationPosition(
								plBitBoard, opBitBoard, plCapture, opCapture,
								interestingMoves[i].x, interestingMoves[i].y);
		if (tmpEval > bestEval)
		{
			bestEval = tmpEval;
			move = interestingMoves[i];
		}
	}

	return (move);
}