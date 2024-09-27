#include <AI/AI.hpp>
#include <utils/Functions.hpp>

sf::Vector2i	getRandomMove(Grid *grid, PlayerInfo *player, PlayerInfo *opponent)
{
	std::vector<sf::Vector2i>	moves;

	// Get all legal moves
	moves = grid->getLegalMoves(player, opponent);
	if (moves.size() == 0)
		return (sf::Vector2i(-1, -1));

	// Choose move to play
	int index = rand_int(0, moves.size() - 1);
	return (moves[index]);
}