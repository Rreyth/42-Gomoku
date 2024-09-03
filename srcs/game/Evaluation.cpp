#include <game/Evaluation.hpp>

////////////////////////////////////////////////////////////////////////////////
// Functions
////////////////////////////////////////////////////////////////////////////////

# define COMPLETE_LINE_5 10000000
# define COMPLETE_LINE_4 100000
# define COMPLETE_LINE_3 1000
# define COMPLETE_LINE_2 100
# define COMPLETE_LINE_1 10
# define BLOCK_LINE_5 99900
# define BLOCK_LINE_4 10000
# define BLOCK_LINE_3 100
# define BLOCK_LINE_2 10
# define BLOCK_LINE_1 1

int	evaluationPosition(Grid *grid, inter_type plType, int x, int y)
{
	int				result, completeLine, blockLine, invAxis;
	bool			isCompleteLine, isBlockLine;
	intersection	*inter;

	// TODO: CHANGE DECLARATION POS
	int	completeLinePoint[6] = {0, 10, 100, 1000, 100000, 10000000};
	int	blockLinePoint[6] = {0, 1, 10, 100, 10000, 99900};
	sf::Vector2i	dirs[8];
	dirs[DIR_L] = sf::Vector2i(-1, 0);
	dirs[DIR_UL] = sf::Vector2i(-1, -1);
	dirs[DIR_U] = sf::Vector2i(0, -1);
	dirs[DIR_UR] = sf::Vector2i(1, -1);
	dirs[DIR_R] = sf::Vector2i(1, 0);
	dirs[DIR_DR] = sf::Vector2i(1, 1);
	dirs[DIR_D] = sf::Vector2i(0, 1);
	dirs[DIR_DL] = sf::Vector2i(-1, 1);

	// If intersection isn't empty, return 0;
	if (grid->getInterState(x, y) != INTER_EMPTY)
		return (0);

	result = 0;

	// Compute completion and block line point for each axis
	for (int axis = 0; axis < 4; axis++)
	{
		completeLine = 0;
		blockLine = 0;
		isCompleteLine = false;
		isBlockLine = false;
		invAxis = axis + 4;

		// Check front of axis
		inter = grid->getIntersection(x + dirs[axis].x,
										y + dirs[axis].y);
		if (inter)
		{
			// Stone of player
			if (inter->type == plType)
			{
				completeLine += inter->neighbor[axis] + 1;
				isCompleteLine = true;
			}
			// Stone of opponent
			else if (inter->type != INTER_EMPTY)
			{
				blockLine += inter->neighbor[axis] + 1;
				isBlockLine = true;
			}
		}

		// Check back of axis
		inter = grid->getIntersection(x + dirs[invAxis].x,
										y + dirs[invAxis].y);
		if (inter)
		{
			// Stone of player
			if (inter->type == plType)
			{
				completeLine += inter->neighbor[invAxis] + 1;
				isCompleteLine = true;
			}
			// Stone of opponent
			else if (inter->type != INTER_EMPTY)
			{
				blockLine += inter->neighbor[invAxis] + 1;
				isBlockLine = true;
			}
		}

		if (isCompleteLine)
			completeLine++;
		if (isBlockLine)
			blockLine++;

		// Update result for complete line
		if (completeLine >= 6)
			completeLine = 5;
		result += completeLinePoint[completeLine];

		// Update result for block line
		if (blockLine >= 6)
			blockLine = 5;
		result += blockLinePoint[blockLine];
	}

	return	(result);
}
