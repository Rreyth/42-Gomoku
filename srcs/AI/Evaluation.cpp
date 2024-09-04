#include <AI/Evaluation.hpp>

////////////////////////////////////////////////////////////////////////////////
// Constructors and destructor
////////////////////////////////////////////////////////////////////////////////

Evaluation::Evaluation(void)
{
	// Set points for complete a line
	this->completeLinePoint[0] = 0; // Line of 0 stone
	this->completeLinePoint[1] = 10; // Line of 1 stone
	this->completeLinePoint[2] = 100; // Line of 2 stones
	this->completeLinePoint[3] = 1000; // Line of 3 stones
	this->completeLinePoint[4] = 100000; // Line of 4 stones
	this->completeLinePoint[5] = 10000000; // Line of 5 stones

	// Set points for block a line
	this->blockLinePoint[0] = 0; // Line of 0 stone
	this->blockLinePoint[1] = 1; // Line of 1 stone
	this->blockLinePoint[2] = 10; // Line of 2 stones
	this->blockLinePoint[3] = 100; // Line of 3 stones
	this->blockLinePoint[4] = 10000; // Line of 4 stones
	this->blockLinePoint[5] = 999000; // Line of 5 stones

	// Number for update x and y to go along each axis
	this->dirs[DIR_L] = sf::Vector2i(-1, 0);
	this->dirs[DIR_UL] = sf::Vector2i(-1, -1);
	this->dirs[DIR_U] = sf::Vector2i(0, -1);
	this->dirs[DIR_UR] = sf::Vector2i(1, -1);
	this->dirs[DIR_R] = sf::Vector2i(1, 0);
	this->dirs[DIR_DR] = sf::Vector2i(1, 1);
	this->dirs[DIR_D] = sf::Vector2i(0, 1);
	this->dirs[DIR_DL] = sf::Vector2i(-1, 1);
}


Evaluation::~Evaluation()
{

}

////////////////////////////////////////////////////////////////////////////////
// Public methods
////////////////////////////////////////////////////////////////////////////////

int	Evaluation::evaluationPosition(Grid *grid, inter_type plType,
									inter_type opType,
									int plCapture, int opCapture,
									int x, int y)
{
	int				result, completeLine, blockLine, invAxis;
	bool			isCompleteLine, isBlockLine;
	intersection	*inter;

	plCapture = plCapture * 4 + 1;
	opCapture = opCapture * 4 + 1;

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
		this->updateCompleteAndBlockLine(
				grid, plType, opType,
				plCapture, opCapture, x, y, axis,
				&completeLine, &isCompleteLine,
				&blockLine, &isBlockLine, &result);

		// Check back of axis
		this->updateCompleteAndBlockLine(
				grid, plType, opType,
				plCapture, opCapture, x, y, invAxis,
				&completeLine, &isCompleteLine,
				&blockLine, &isBlockLine, &result);

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

////////////////////////////////////////////////////////////////////////////////
// Private methods
////////////////////////////////////////////////////////////////////////////////

void	Evaluation::updateCompleteAndBlockLine(
						Grid *grid, inter_type plType, inter_type opType,
						int plCapture, int opCapture,
						int x, int y, int axis,
						int *completeLine, bool *isCompleteLine,
						int *blockLine, bool *isBlockLine, int *result)
{
	intersection	*inter, *interTmp;

	inter = grid->getIntersection(x + this->dirs[axis].x,
								y + this->dirs[axis].y);
	if (inter)
	{
		// Stone of player
		if (inter->type == plType)
		{
			*completeLine += inter->neighbor[axis] + 1;
			*isCompleteLine = true;

			// Check if a capture is possible for opponent
			if (inter->neighbor[axis] != 1)
				return ;

			interTmp = grid->getIntersection(x + this->dirs[axis].x * 3,
											y + this->dirs[axis].y * 3);
			if (!interTmp || interTmp->type != opType)
				return ;

			// Opponent capture possible !
			if (interTmp->neighbor[axis] > 0)
				*result += this->blockLinePoint[5] * opCapture;
			else
				*result += this->blockLinePoint[4] * opCapture;
		}

		// Stone of opponent
		else if (inter->type == opType)
		{
			*blockLine += inter->neighbor[axis] + 1;
			*isBlockLine = true;

			// Check if a capture is possible
			if (inter->neighbor[axis] != 1)
				return ;

			interTmp = grid->getIntersection(x + this->dirs[axis].x * 3,
											y + this->dirs[axis].y * 3);
			if (!interTmp || interTmp->type != plType)
				return ;

			// Capture possible !
			if (interTmp->neighbor[axis] > 0)
				*result += this->completeLinePoint[5] * plCapture;
			else
				*result += this->completeLinePoint[4] * plCapture;

		}
	}
}