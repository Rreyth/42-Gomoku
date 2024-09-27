#include <AI/AI.hpp>
#include <utils/Functions.hpp>

sf::Vector2i	getBetterRandom(Grid *grid, PlayerInfo *player, PlayerInfo *opponent)
{
	std::vector<sf::Vector2i>	moves;

	// Get interesting move
	moves = grid->getInterestingMoves(player, opponent);
	if (moves.size() == 0)
	{
		if (player->nbMove + opponent->nbMove == 0)
			return(sf::Vector2i(rand_int(0, GRID_W_INTER - 1),
								rand_int(0, GRID_W_INTER - 1)));
		else
			return (sf::Vector2i(-1, -1));
	}

	// Find move to play
	int index = rand_int(0, moves.size() - 1);
	return (moves[index]);
}