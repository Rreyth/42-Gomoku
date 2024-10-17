#include <AI/AI.hpp>
#include <utils/Functions.hpp>

sf::Vector2i	getBetterRandom(Grid *grid, PlayerInfo *player, PlayerInfo *opponent)
{
	std::vector<sf::Vector2i>	moves;

	printf("\nFIND BEST EVAL AT TURN %i\n", player->nbMove + opponent->nbMove);
	// Get interesting move
	moves = grid->getInterestingMoves(player, opponent);
	if (moves.size() == 0)
	{
		if (player->nbMove + opponent->nbMove == 0)
		{
			printf("Start at center\n");
			return(sf::Vector2i(GRID_W_INTER / 2,
								GRID_W_INTER / 2));
			// return(sf::Vector2i(rand_int(0, GRID_W_INTER - 1),
			// 					rand_int(0, GRID_W_INTER - 1)));
		}
		else
		{
			printf("Nik\n");
			return (sf::Vector2i(-1, -1));
		}
	}

	// // Find move to play
	// int index = rand_int(0, moves.size() - 1);
	// return (moves[index]);

	Evaluation	evaluator;
	int	best, tmp;
	sf::Vector2i	move = sf::Vector2i(-1, -1);
	char	*xaxis = "ABCDEFGHIJKLMNOPQRSTU";

	best = CASE_LOOSE_POINT;
	for (int i = 0; i < moves.size(); i++)
	{
		tmp = evaluator.evalPosition(
							grid->getBitBoard(player->interType),
							grid->getBitBoard(opponent->interType),
							player->nbCapture,
							opponent->nbCapture,
							moves[i].x, moves[i].y);
		printf(" - pos %c %2i for %i\n", xaxis[moves[i].x], moves[i].y + 1, tmp);

		if (tmp > best)
		{
			best = tmp;
			move = moves[i];
		}
	}
	printf("best %c %2i for %i\n", xaxis[move.x], move.y + 1, best);
	return (move);
}
