#include <AI/AI.hpp>
#include <utils/Functions.hpp>

sf::Vector2i	getBetterEasyMove(
					Grid *grid, PlayerInfo *player, PlayerInfo *opponent,
					Evaluation *evaluator)
{
	std::vector<sf::Vector2i>	moves;
	Move						best, tmp;
	BitBoard					*plBitboard, *opBitboard;

	// Get interesting move
	moves = grid->getInterestingMoves(player, opponent);
	if (moves.size() == 0)
	{
		if (player->nbMove + opponent->nbMove == 0)
			return(sf::Vector2i(GRID_W_INTER / 2, GRID_W_INTER / 2));
		else
			return (sf::Vector2i(-1, -1));
	}

	// Find move to play
	best.eval = CASE_LOOSE_POINT;
	best.pos = sf::Vector2i(-1, -1);
	plBitboard = grid->getBitBoard(player->interType);
	opBitboard = grid->getBitBoard(opponent->interType);
	for (int i = 0; i < moves.size(); i++)
	{
		tmp.pos = moves[i];
		tmp.eval = evaluator->evalPosition(
							plBitboard, opBitboard,
							player->nbCapture, opponent->nbCapture,
							tmp.pos.x, tmp.pos.y);

		if (tmp.eval > best.eval)
		{
			if (tmp.eval >= CASE_WIN_POINT)
				return (tmp.pos);
			best = tmp;
		}
	}
	return (best.pos);
}
