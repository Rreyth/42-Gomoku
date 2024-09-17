#include <game/Grid.hpp>
#include <utils/Functions.hpp>
#include <game/Player.hpp>
#include <AI/Evaluation.hpp>

////////////////////////////////////////////////////////////////////////////////
// Constructors and destructor
////////////////////////////////////////////////////////////////////////////////

Grid::Grid(void)
{
	this->x = WIN_W / 2 - GRID_SIZE / 2;
	this->y = WIN_H / 2 - GRID_SIZE / 2;
	this->w = GRID_SIZE;
	this->h = GRID_SIZE;

	this->previewX = 0;
	this->previewY = 0;
	this->previewLegal = false;

	this->clearGrid(SPRITE_STONE_BLUE, SPRITE_STONE_RED, STANDARD);

	this->historyIdString = "";

	// this->bboxUL = sf::Vector2i(GRID_W_INTER - 1, GRID_W_INTER - 1);
	// this->bboxDR = sf::Vector2i(0, 0);
}


Grid::~Grid()
{
}

////////////////////////////////////////////////////////////////////////////////
// Getters and setters
////////////////////////////////////////////////////////////////////////////////

int	Grid::getX(void)
{
	return (this->x);
}


int	Grid::getY(void)
{
	return (this->y);
}


int	Grid::getW(void)
{
	return (this->w);
}


int	Grid::getH(void)
{
	return (this->h);
}


std::string	Grid::getHistoryIdString(void)
{
	return (this->historyIdString);
}


BitBoard	*Grid::getBitBoard(inter_type type)
{
	if (type == INTER_LEFT)
		return (&this->bitboardL);
	return (&this->bitboardR);
}

////////////////////////////////////////////////////////////////////////////////
// Public methods
////////////////////////////////////////////////////////////////////////////////

Grid	&Grid::operator=(const Grid &grid)
{
	if (this == &grid)
		return (*this);

	// Copy game rule
	this->rule = grid.rule;

	// Copy board
	this->bitboardL = grid.bitboardL;
	this->bitboardR = grid.bitboardR;

	// this->bboxUL = grid.bboxUL;
	// this->bboxDR = grid.bboxDR;
	// this->currentBoardState = grid.currentBoardState;
	// this->currentBoardStateOpti = grid.currentBoardStateOpti;

	return (*this);
}

////////////////////////////////////////////////////////////////////////////////
// Public methods
////////////////////////////////////////////////////////////////////////////////

void	Grid::tick(display_state *displayState, Mouse *mouse,
					Player *leftPlayer, Player *rightPlayer,
					Evaluation *evaluator)
{
	if (!mouse->inRectangle(this->x, this->y, this->w, this->h))
	{
		this->previewLegal = false;
		return;
	}

	int	mx = mouse->getX();
	int	my = mouse->getY();
	mx -= this->x;
	my -= this->y;
	int px = (mx + GRID_SQUARE_HALF_SIZE) / GRID_SQUARE_SIZE;
	int py = (my + GRID_SQUARE_HALF_SIZE) / GRID_SQUARE_SIZE;

	if (mouse->isPressed(MBUT_RIGHT))
	{
		int	evalL, evalR, captureL, captureR;
		std::string	xaxis;

		captureL = leftPlayer->getCaptured();
		captureR = rightPlayer->getCaptured();

		evalL = evaluator->evaluationPosition(
							&this->bitboardL, &this->bitboardR,
							captureL, captureR, px, py);
		evalR = evaluator->evaluationPosition(
							&this->bitboardR, &this->bitboardL,
							captureR, captureL, px, py);

		xaxis = "ABCDEFGHIJKLMNOPQRSTUVW";
		printf("\nEvaluation of %c%i : %i | %i\n",
				xaxis[px], py + 1, evalL, evalR);
	}

	if (px == this->previewX && py == this->previewY
			&& !mouse->isPressed(MBUT_LEFT))
		return ;

	this->previewX = px;
	this->previewY = py;
	inter_type	plState = INTER_LEFT;
	inter_type	opState = INTER_RIGHT;
	if (!leftPlayer->isPlaying())
	{
		plState = INTER_RIGHT;
		opState = INTER_LEFT;
	}

	this->previewLegal = this->checkLegalMove(px, py,
							rightPlayer->getMoves() + leftPlayer->getMoves(),
							plState, opState);

	if (!mouse->isPressed(MBUT_LEFT) || !this->previewLegal)
		return ;

	this->previewLegal = false;
}


void	Grid::draw(sf::RenderWindow *window, sf::Text *text,
					TextureManager *textureManager)
{
	textureManager->drawTexture(window, SPRITE_GRID, this->x, this->y, TOP_LEFT);

	std::string	xlabels = "ABCDEFGHIJKLMNOPQRST.";
	std::string	label;
	int	drawX, drawY;

	drawY = this->y - 15;
	//letters top
	for (int i = 0; i < GRID_W_INTER; i++)
	{
		drawX = this->x + i * GRID_SQUARE_SIZE + 10;
		label = "";
		label += xlabels[i];
		drawText(window, text, label, drawX, drawY,
					20, sf::Color::White, MID_CENTER);
	}

	//numbers left
	drawX = this->x - 10;
	for (int i = 0; i < GRID_W_INTER; i++)
	{
		drawY = this->y + i * GRID_SQUARE_SIZE - 4 + 10;
		label = std::to_string(i + 1);
		if (i == 0)
			drawText(window, text, label, drawX - 3, drawY,
						20, sf::Color::White, MID_RIGHT);
		else
			drawText(window, text, label, drawX, drawY,
						20, sf::Color::White, MID_RIGHT);
	}

	//draw stones
	for (int y = 0; y < GRID_W_INTER; y++)
	{
		for (int x = 0; x < GRID_W_INTER; x++)
		{
			drawX = this->x + x * GRID_SQUARE_SIZE + 10;
			drawY = this->y + y * GRID_SQUARE_SIZE + 10;
			if (this->bitboardL.get(x, y))
				textureManager->drawTexture(window, this->leftStone,
											drawX, drawY, MID_CENTER);
			else if (this->bitboardR.get(x, y))
				textureManager->drawTexture(window, this->rightStone,
											drawX, drawY, MID_CENTER);
		}
	}

	//draw stone preview
	if (this->previewLegal)
	{
		drawX = this->x + this->previewX * GRID_SQUARE_SIZE + 10;
		drawY = this->y + this->previewY * GRID_SQUARE_SIZE + 10;
		textureManager->drawTexture(window, SPRITE_STONE_PREVIEW,
									drawX, drawY, MID_CENTER);
	}
}

bool	Grid::checkLegalMove(
				int x, int y, int nbMoves, inter_type plType, inter_type opType)
{
	if (x < 0 || x >= GRID_W_INTER || y < 0 || y >= GRID_W_INTER)
		return (false);

	if (this->bitboardL.get(x, y) || this->bitboardR.get(x, y))
		return (false);

	// Check double free three
	if (plType == INTER_LEFT)
	{
		if (this->checkDoubleFreeThree(
					x, y, &this->bitboardL, &this->bitboardR))
			return (false);
	}
	else
	{
		if (this->checkDoubleFreeThree(
					x, y, &this->bitboardR, &this->bitboardL))
			return (false);
	}

	// check rule
	if (this->rule == PRO && nbMoves <= 2)
		if (!this->checkProRule(x, y, plType, nbMoves))
			return (false);

	return (true);
}


bool	Grid::putStone(
				sf::Vector2i *move, int nbMoves, Player *player, Player *opponent)
{
	inter_type	plType, opType;

	plType = player->getInterType();
	opType = opponent->getInterType();

	if (!this->checkLegalMove(move->x, move->y, nbMoves, plType, opType))
		return (false);

	if (plType == INTER_LEFT)
	{
		this->bitboardL.set(move->x, move->y, true);
		player->addCaptured(this->makeCapture(
									move, &this->bitboardL, &this->bitboardR));
	}
	else
	{
		this->bitboardR.set(move->x, move->y, true);
		player->addCaptured(this->makeCapture(
									move, &this->bitboardR, &this->bitboardL));
	}

	return (true);
}


bool	Grid::checkWinCondition(Player *player, Player *opponent)
{
	// Check if the player capture at least 10 opponent's stones
	if (player->getCaptured() >= WIN_CAPTURE)
	{
		player->setWinState(WIN_STATE_CAPTURE);
		this->previewLegal = false;
		return (true);
	}
	else if (player->getCaptured() == 8)
	{
		if (player->getInterType() == INTER_LEFT)
		{
			if (this->checkWinByAlign(
							opponent, player, &this->bitboardR, &this->bitboardL))
				return (true);
		}
		else
		{
			if (this->checkWinByAlign(
							opponent, player, &this->bitboardL, &this->bitboardR))
				return (true);
		}
	}

	// TODO: FIX OPPONENT CAPTURE FREEING A LINE
	// check both sides win to fix
	if (player->getInterType() == INTER_LEFT)
	{
		return (this->checkWinByAlign(
						player, opponent, &this->bitboardL, &this->bitboardR));
		// if (this->checkWinByAlign(
		// 			player, opponent, &this->bitboardL, &this->bitboardR))
		// 	return (true);
		// return (this->checkWinByAlign(
		// 				opponent, player, &this->bitboardR, &this->bitboardL));
	}

	return (this->checkWinByAlign(
					player, opponent, &this->bitboardR, &this->bitboardL));
	// if (this->checkWinByAlign(
	// 			player, opponent, &this->bitboardR, &this->bitboardL))
	// 	return (true);
	// return (this->checkWinByAlign(
	// 				opponent, player, &this->bitboardL, &this->bitboardR));
}


void	Grid::clearGrid(sprite_name leftStone, sprite_name rightStone,
						game_rules rule)
{
	this->leftStone = leftStone;
	this->rightStone = rightStone;
	this->rule = rule;

	for (int i = 0; i < GRID_W_INTER; i++)
	{
		this->bitboardL.bbH[i] = 0;
		this->bitboardL.bbV[i] = 0;
		this->bitboardL.bbD[i] = 0;
		this->bitboardL.bbA[i] = 0;
		this->bitboardR.bbH[i] = 0;
		this->bitboardR.bbV[i] = 0;
		this->bitboardR.bbD[i] = 0;
		this->bitboardR.bbA[i] = 0;
	}
	this->boardHistoryId = 0;
	this->boardHistory.clear();
	// this->interestingMovesLeft.clear();
	// this->interestingMovesRight.clear();
}


void	Grid::reset(void)
{
	for (int i = 0; i < GRID_W_INTER; i++)
	{
		this->bitboardL.bbH[i] = 0;
		this->bitboardL.bbV[i] = 0;
		this->bitboardL.bbD[i] = 0;
		this->bitboardL.bbA[i] = 0;
		this->bitboardR.bbH[i] = 0;
		this->bitboardR.bbV[i] = 0;
		this->bitboardR.bbD[i] = 0;
		this->bitboardR.bbA[i] = 0;
	}
	this->boardHistoryId = 0;
	this->boardHistory.clear();
	// this->interestingMovesLeft.clear();
	// this->interestingMovesRight.clear();
}

void	Grid::addBoardToHistory(void)
{
	this->boardHistory.push_back(
			std::pair<BitBoard, BitBoard>(this->bitboardL, this->bitboardR));
}


void	Grid::goToHistoryStart(void)
{
	this->boardHistoryId = 0;
	this->applyHistoryToGrid();
}


void	Grid::goToHistoryPrevious(void)
{
	if (this->boardHistoryId <= 0)
		return ;
	this->boardHistoryId--;
	this->applyHistoryToGrid();
}


void	Grid::goToHistoryNext(void)
{
	if (this->boardHistoryId >= this->boardHistory.size() - 1)
		return ;
	this->boardHistoryId++;
	this->applyHistoryToGrid();
}


void	Grid::goToHistoryEnd(void)
{
	this->boardHistoryId = this->boardHistory.size() - 1;
	this->applyHistoryToGrid();
}


void	Grid::disablePreview(void)
{
	this->previewLegal = false;
}


std::vector<sf::Vector2i>	Grid::getLegalMoves(Player *player, Player *opponent)
{
	int							nbMoves;
	inter_type					plState, opState;
	std::vector<sf::Vector2i>	moves;

	nbMoves = player->getMoves() + opponent->getMoves();
	plState = player->getInterType();
	opState = opponent->getInterType();
	for (int i = 0; i < GRID_W_INTER; i++)
	{
		for (int j = 0; j < GRID_W_INTER; j++)
		{
			if (checkLegalMove(i, j, nbMoves, plState, opState))
				moves.push_back(sf::Vector2i(i, j));
		}
	}

	return (moves);
}


std::vector<sf::Vector2i>	Grid::getInterestingMoves(
									Player *player, Player *opponent)
{
	bool						isStoneAlone;
	int							nbMoves, check, checkV, yD, yA, maxLimit;
	inter_type					plState, opState;
	std::vector<sf::Vector2i>	moves;

	nbMoves = player->getMoves() + opponent->getMoves();
	plState = player->getInterType();
	opState = opponent->getInterType();
	maxLimit = GRID_W_INTER - 1;

	for (int y = 0; y < GRID_W_INTER; y++)
	{
		if (y == 0)
			checkV = 0b10;
		else
			checkV = 0b101 << (y - 1);

		for (int x = 0; x < GRID_W_INTER; x++)
		{
			if (this->bitboardL.get(x, y) || this->bitboardR.get(x, y))
				continue;

			if (x == 0)
				check = 0b10;
			else
				check = 0b101 << (x - 1);

			yD = (y + x) % GRID_W_INTER;
			yA = y - x;
			if (yA < 0)
				yA += GRID_W_INTER;

			isStoneAlone = true;

			// Check if stone isn't alone on horizontal axis
			if (isStoneAlone
					&& ((this->bitboardL.bbH[y] & check) > 0
					|| (this->bitboardR.bbH[y] & check) > 0))
				isStoneAlone = false;

			// Check if stone isn't alone on vertical axis
			if (isStoneAlone
				&& ((this->bitboardL.bbV[x] & checkV) > 0
					|| (this->bitboardR.bbV[x] & checkV) > 0))
				isStoneAlone = false;

			// Check if stone isn't alone on diagonal axis
			if (isStoneAlone && x > 1 && x < maxLimit && y > 1 && y < maxLimit
				&& ((this->bitboardL.bbD[yD] & check) > 0
					|| ((this->bitboardR.bbD[yD] & check) > 0)))
				isStoneAlone = false;

			// Check if stone isn't alone on anti diagonal axis
			if (isStoneAlone && x > 1 && x < maxLimit && y > 1 && y < maxLimit
				&& ((this->bitboardL.bbA[yA] & check) > 0
					|| (this->bitboardR.bbA[yA] & check) > 0))
				isStoneAlone = false;

			if (isStoneAlone)
				continue;

			// Check if the move is legal
			if (!checkLegalMove(x, y, nbMoves, plState, opState))
				continue;
			moves.push_back(sf::Vector2i(x, y));
		}
	}

	return (moves);
}


// std::vector<sf::Vector2i>	Grid::getInterestingMovesSorted(
// 									Player *player, Player *opponent,
// 									Evaluation *evaluator, bool reverse,
// 									Tracker *tracker)
// {
// 	int							size, eval, j, plCapture, opCapture;
// 	sf::Vector2i				tmp;
// 	std::vector<sf::Vector2i>	moves;
// 	std::vector<int>			evaluations;
// 	inter_type					plType, opType;

// 	// TODO : REMOVE
// 	std::clock_t	start;
// 	int				diff;
// 	start = std::clock();

// 	tracker->getSortMoveNumber++;
// 	moves = this->getInterestingMoves(player, opponent);
// 	diff = ((double)(std::clock() - start) / CLOCKS_PER_SEC) * 1000000;
// 	tracker->getMoveTime += diff;

// 	start = std::clock();

// 	size = moves.size();
// 	if (size < 2)
// 		return (moves);

// 	plType = player->getInterType();
// 	opType = opponent->getInterType();
// 	plCapture = player->getCaptured();
// 	opCapture = opponent->getCaptured();

// 	for (int i = 0; i < size; i++)
// 		evaluations.push_back(evaluator->evaluationPosition(this,
// 											plType, opType, plCapture, opCapture,
// 											moves[i].x, moves[i].y));

// 	for (int i = 1; i < size; i++)
// 	{
// 		tmp = moves[i];
// 		eval = evaluations[i];
// 		j = i - 1;

// 		// Classic order
// 		if (!reverse)
// 		{
// 			while (j >= 0 && evaluations[j] > eval)
// 			{
// 				moves[j + 1] = moves[j];
// 				evaluations[j + 1] = evaluations[j];
// 				j = j - 1;
// 			}
// 		}
// 		// Reverse order
// 		else
// 		{
// 			while (j >= 0 && evaluations[j] < eval)
// 			{
// 				moves[j + 1] = moves[j];
// 				evaluations[j + 1] = evaluations[j];
// 				j = j - 1;
// 			}
// 		}

// 		moves[j + 1] = tmp;
// 		evaluations[j + 1] = eval;
// 	}

// 	diff = ((double)(std::clock() - start) / CLOCKS_PER_SEC) * 1000000;
// 	tracker->sortMoveTime += diff;

// 	return (moves);
// }


// void	Grid::computeInterestingMovesSorted(
// 				Player *player, Player *opponent, Evaluation *evaluator,
// 				Tracker *tracker)
// {
// 	if (player->getInterType() == INTER_LEFT)
// 	{
// 		this->interestingMovesLeft = this->getInterestingMovesSorted(
// 											player, opponent, evaluator,
// 											true, tracker);
// 		this->interestingMovesRight = this->getInterestingMovesSorted(
// 											opponent, player, evaluator,
// 											false, tracker);
// 	}
// 	else
// 	{
// 		this->interestingMovesRight = this->getInterestingMovesSorted(
// 											player, opponent, evaluator,
// 											true, tracker);
// 		this->interestingMovesLeft = this->getInterestingMovesSorted(
// 											opponent, player, evaluator,
// 											false, tracker);
// 	}
// }


// void	Grid::updateInterestingMovesSorted(
// 				Player *player, Player *opponent, Evaluation *evaluator,
// 				Tracker *tracker, sf::Vector2i *move)
// {
// 	int									nbMoves;
// 	inter_type							type, plType, opType;
// 	sf::Vector2i						tmpMove;
// 	std::vector<sf::Vector2i>::iterator	it;

// 	// TODO : REMOVE
// 	std::clock_t	start;
// 	int				diff;
// 	start = std::clock();

// 	nbMoves = player->getMoves() + opponent->getMoves();
// 	plType = player->getInterType();
// 	opType = opponent->getInterType();

// 	// Remove new placed stone from left interesting moves
// 	it = std::find(this->interestingMovesLeft.begin(), this->interestingMovesLeft.end(), *move);
// 	if (it != this->interestingMovesLeft.end())
// 		this->interestingMovesLeft.erase(it);

// 	// Remove new placed stone from right interesting moves
// 	it = std::find(this->interestingMovesRight.begin(), this->interestingMovesRight.end(), *move);
// 	if (it != this->interestingMovesRight.end())
// 		this->interestingMovesRight.erase(it);

// 	// For each stone arround the new placed stone
// 	for (int i = 0; i < 8; i++)
// 	{
// 		tmpMove = *move + this->dirs[i];

// 		// Check if the intersection is empty
// 		type = this->getInterState(tmpMove.x, tmpMove.y);
// 		if (type != INTER_EMPTY)
// 			continue;

// 		// Check if the move is already in the left moves
// 		it = std::find(this->interestingMovesLeft.begin(),
// 						this->interestingMovesLeft.end(), tmpMove);

// 		// Check if the move is legal for player
// 		if (it == this->interestingMovesLeft.end()
// 			&& checkLegalMove(tmpMove.x, tmpMove.y, nbMoves, plType, opType))
// 		{
// 			if (plType == INTER_LEFT)
// 				this->insertMoves(
// 						this->interestingMovesLeft, &tmpMove, true,
// 						evaluator, player, opponent);
// 			else
// 				this->insertMoves(
// 						this->interestingMovesRight, &tmpMove, false,
// 						evaluator, player, opponent);
// 		}

// 		// Check if the move is already in the right moves
// 		it = std::find(this->interestingMovesRight.begin(),
// 						this->interestingMovesRight.end(), tmpMove);

// 		// Check if the move is legal for opponent
// 		if (it == this->interestingMovesRight.end()
// 			&& checkLegalMove(tmpMove.x, tmpMove.y, nbMoves, opType, plType))
// 		{
// 			if (opType == INTER_LEFT)
// 				this->insertMoves(
// 						this->interestingMovesLeft, &tmpMove, true,
// 						evaluator, player, opponent);
// 			else
// 				this->insertMoves(
// 						this->interestingMovesRight, &tmpMove, false,
// 						evaluator, player, opponent);
// 		}
// 	}

// 	diff = ((double)(std::clock() - start) / CLOCKS_PER_SEC) * 1000000;
// 	tracker->updateMoveNumber++;
// 	tracker->updateMoveTime += diff;
// }


// std::vector<sf::Vector2i>	Grid::getInterestingMovesSortedSaved(
// 									inter_type plType)
// {
// 	if (plType == INTER_LEFT)
// 		return (this->interestingMovesLeft);
// 	return (this->interestingMovesRight);
// }


// void	Grid::saveInterestingMovesSorted(
// 				std::vector<sf::Vector2i> *movesLeft,
// 				std::vector<sf::Vector2i> *movesRight)
// {
// 	*movesLeft = this->interestingMovesLeft;
// 	*movesRight = this->interestingMovesRight;
// }


// void	Grid::loadInterestingMovesSorted(
// 				std::vector<sf::Vector2i> *movesLeft,
// 				std::vector<sf::Vector2i> *movesRight)
// {
// 	this->interestingMovesLeft = *movesLeft;
// 	this->interestingMovesRight = *movesRight;
// }


// void	Grid::removeStone(sf::Vector2i *move, Tracker *tracker)
// {
// 	int				nbNeighbor, x, y, invAxis;
// 	intersection	*inter, *interTmp;

// 	// TODO : REMOVE
// 	std::clock_t	start;
// 	int				diff;
// 	start = std::clock();

// 	// Get the intersection to clean
// 	inter = this->getIntersection(move->x, move->y);
// 	if (!inter)
// 		return ;

// 	// If the intersection is already empty, stop remove process here
// 	if (inter->type == INTER_EMPTY)
// 		return ;

// 	// Put the intersection to empty
// 	inter->type = INTER_EMPTY;

// 	// Update board state
// 	this->updateBoardState(move->x + move->y * GRID_W_INTER, 'E');

// 	// For each axis
// 	for (int axis = 0; axis < 8; axis++)
// 	{
// 		// Get the number of neighbor in this axis
// 		nbNeighbor = inter->neighbor[axis];

// 		// If there are no neighbor, go to next axis
// 		if (nbNeighbor == 0)
// 			continue;

// 		// Reset the number of neighbor in this direction
// 		inter->neighbor[axis] = 0;

// 		// Reverse axis
// 		invAxis = (axis + 4) % 8;

// 		// For each neighbor in the axis, remove one neighbor in the reverse axis
// 		x = move->x;
// 		y = move->y;
// 		for (int i = 0; i < nbNeighbor; i++)
// 		{
// 			x += this->dirs[axis].x;
// 			y += this->dirs[axis].y;
// 			interTmp = this->getIntersection(x, y);
// 			if (!interTmp)
// 				continue;
// 			interTmp->neighbor[invAxis]--;
// 		}

// 	}

// 	diff = ((double)(std::clock() - start) / CLOCKS_PER_SEC) * 1000000;
// 	if (!tracker)
// 		return ;
// 	tracker->removeStoneTime += diff;
// }


// void	Grid::resetGridByBoardState(std::string boardState, Tracker *tracker)
// {
// 	char	c;
// 	int		x, y;

// 	// TODO : REMOVE
// 	std::clock_t	start;
// 	int				diff;
// 	start = std::clock();

// 	// For each intersection
// 	for (int i = 0; i < GRID_NB_INTER; i++)
// 	{
// 		c = boardState[i];

// 		// If inter is empty and c != E, there was a capture
// 		if (c != 'E' && this->gridState[i].type == INTER_EMPTY)
// 		{
// 			// Get x and y
// 			x = i % 17;
// 			y = i / 17;

// 			// Reput the previous type
// 			if (c == 'L')
// 			{
// 				this->gridState[i].type = INTER_LEFT;
// 				// Update board state
// 				this->updateBoardState(i, 'L');
// 			}
// 			else
// 			{
// 				this->gridState[i].type = INTER_RIGHT;
// 				// Update board state
// 				this->updateBoardState(i, 'R');
// 			}

// 			// Update neighbor
// 			this->updateNeighbor(x, y);
// 		}
// 	}

// 	diff = ((double)(std::clock() - start) / CLOCKS_PER_SEC) * 1000000;
// 	if (!tracker)
// 		return ;
// 	tracker->reverseStoneCaptureNumber++;
// 	tracker->reverseStoneCaptureTime += diff;
// }


// void	Grid::saveBbox(sf::Vector2i *bboxUL, sf::Vector2i *bboxDR)
// {
// 	*bboxUL = this->bboxUL;
// 	*bboxDR = this->bboxDR;
// }


// void	Grid::loadBbox(sf::Vector2i *bboxUL, sf::Vector2i *bboxDR)
// {
// 	this->bboxUL = *bboxUL;
// 	this->bboxDR = *bboxDR;
// }


////////////////////////////////////////////////////////////////////////////////
// Private methods
////////////////////////////////////////////////////////////////////////////////

bool	Grid::checkWinByAlign(
				Player *player, Player *opponent,
				BitBoard *plBitboard, BitBoard *opBitboard)
{
	int		winCase, check, plLine;
	bool	isWin;

	// Check win by a(bitboardAxis)lign
	winCase = 0b11111;

	for (int i = 0; i < GRID_W_INTER; i++)
	{
		check = 0b11111;
		for (int j = 0; j < GRID_W_INTER - 4; j++)
		{
			for (int axis = 0; axis < 4; axis++)
			{
				if (axis == 1)
					plLine = plBitboard->getLine((bitboardAxis)axis, i, j);
				else
					plLine = plBitboard->getLine((bitboardAxis)axis, j, i);
				if ((plLine & check) >> j == winCase)
				{
					isWin = this->validateWin(
									player, opponent, plBitboard,
									opBitboard, (bitboardAxis)axis, j, i);
					if (isWin)
						return (true);
				}
			}
			check <<= 1;
		}
	}

	return (false);
}


bool	Grid::validateWin(
				Player *player, Player *opponent,
				BitBoard *plBitBoard, BitBoard *opBitBoard,
				bitboardAxis bbAxis, int x, int y)
{
	int	checkL, checkR, checkVL, checkVR,
		plVerify, opVerifyL, opVerifyR,
		shiftL, shiftR, shiftVL, shiftVR,
		tmp;

	int plLine, opLine;

	// TODO: DIAGONAL CHECK AND ANTI DIAGONAL CHECK ARE NOT WORKING
	// when D or A line

	if (bbAxis == BBV)
	{
		tmp = y;
		y = x;
		x = tmp;
	}
	else if (bbAxis == BBD)
	{
		y = y - x;
		if (y < 0)
			y += GRID_W_INTER;
	}
	else if (bbAxis == BBA)
		y = (y + x) % GRID_W_INTER;

	plVerify = 0b0110;
	opVerifyL = 0b0001;
	opVerifyR = 0b1000;

	for (int i = 0; i < 5; i++)
	{
		shiftR = x - 1;
		shiftL = shiftR - 1;
		checkL = 0b1111 << shiftL;
		checkR = 0b1111 << shiftR;

		shiftVR = y - 1;
		shiftVL = shiftVR - 1;
		checkVL = 0b1111 << shiftVL;
		checkVR = 0b1111 << shiftVR;

		// Check if stone is capturable on horizontal axis
		if (bbAxis != BBH && x > 0 && x < GRID_W_INTER - 1)
		{
			plLine = plBitBoard->getLine(BBH, x, y);
			opLine = opBitBoard->getLine(BBH, x, y);
			if (((plLine & checkL) >> shiftL == plVerify
					&& ((opLine & checkL) >> shiftL == opVerifyL
						|| (opLine & checkL) >> shiftL == opVerifyR))
				|| ((plLine & checkR) >> shiftR == plVerify
					&& ((opLine & checkR) >> shiftR == opVerifyL
						|| (opLine & checkR) >> shiftR == opVerifyR)))
			{
				if (opponent->getCaptured() >= WIN_CAPTURE - 2)
				{
					opponent->setWinState(WIN_STATE_AUTO_CAPTURE);
					this->previewLegal = false;
					return (true);
				}
				return (false);
			}
		}

		// Check if stone is capturable on vertical axis
		if (bbAxis != BBV && y > 0 && y < GRID_W_INTER - 1)
		{
			plLine = plBitBoard->getLine(BBV, x, y);
			opLine = opBitBoard->getLine(BBV, x, y);
			if (((plLine & checkVL) >> shiftVL == plVerify
					&& ((opLine & checkVL) >> shiftVL == opVerifyL
						|| (opLine & checkVL) >> shiftVL == opVerifyR))
				|| ((plLine & checkVR) >> shiftVR == plVerify
					&& ((opLine & checkVR) >> shiftVR == opVerifyL
						|| (opLine & checkVR) >> shiftVR == opVerifyR)))
			{
				if (opponent->getCaptured() >= WIN_CAPTURE - 2)
				{
					opponent->setWinState(WIN_STATE_AUTO_CAPTURE);
					this->previewLegal = false;
					return (true);
				}
				return (false);
			}
		}

		// Check if stone is capturable on diagonal axis
		if (bbAxis != BBD
			&& x > 3 && x < GRID_W_INTER - 4
			&& y > 3 && y < GRID_W_INTER - 4)
		{
			plLine = plBitBoard->getLine(BBD, x, y);
			opLine = opBitBoard->getLine(BBD, x, y);
			if (((plLine & checkL) >> shiftL == plVerify
					&& ((opLine & checkL) >> shiftL == opVerifyL
						|| (opLine & checkL) >> shiftL == opVerifyR))
				|| ((plLine & checkR) >> shiftR == plVerify
					&& ((opLine & checkR) >> shiftR == opVerifyL
						|| (opLine & checkR) >> shiftR == opVerifyR)))
			{
				if (opponent->getCaptured() >= WIN_CAPTURE - 2)
				{
					opponent->setWinState(WIN_STATE_AUTO_CAPTURE);
					this->previewLegal = false;
					return (true);
				}
				return (false);
			}
		}

		// Check if stone is capturable on anti diagonal axis
		if (bbAxis != BBA
			&& x > 3 && x < GRID_W_INTER - 4
			&& y > 3 && y < GRID_W_INTER - 4)
		{
			plLine = plBitBoard->getLine(BBA, x, y);
			opLine = opBitBoard->getLine(BBA, x, y);
			if (((plLine & checkL) >> shiftL == plVerify
					&& ((opLine & checkL) >> shiftL == opVerifyL
						|| (opLine & checkL) >> shiftL == opVerifyR))
				|| ((plLine & checkR) >> shiftR == plVerify
					&& ((opLine & checkR) >> shiftR == opVerifyL
						|| (opLine & checkR) >> shiftR == opVerifyR)))
			{
				if (opponent->getCaptured() >= WIN_CAPTURE - 2)
				{
					opponent->setWinState(WIN_STATE_AUTO_CAPTURE);
					this->previewLegal = false;
					return (true);
				}
				return (false);
			}
		}

		if (bbAxis == BBH)
		{
			x = (x + 1) % GRID_W_INTER;
		}
		else if (bbAxis == BBV)
		{
			y = (y + 1) % GRID_W_INTER;
		}
		else if (bbAxis == BBD)
		{
			x = (x + 1) % GRID_W_INTER;
			y--;
			if (y < 0)
				y += GRID_W_INTER;
		}
		else
		{
			x = (x + 1) % GRID_W_INTER;
			y = (y + 1) % GRID_W_INTER;
		}
	}

	player->setWinState(WIN_STATE_ALIGN);
	this->previewLegal = false;
	return (true);
}


int	Grid::makeCapture(
			sf::Vector2i *move,
			BitBoard *plBitBoard, BitBoard *opBitBoard)
{
	int	checkL, checkR, checkVL, checkVR,
		shiftL, shiftR, shiftVL, shiftVR,
		plVerify, opVerify,
		plLine, opLine,
		nbCapture;

	int	dirx[4] = {1 , 0, 1, 1};
	int diry[4] = {0, 1, -1, 1};

	nbCapture = 0;
	plVerify = 0b1001;
	opVerify = 0b0110;

	shiftR = move->x;
	shiftL = shiftR - 3;
	checkL = 0b1111 << shiftL;
	checkR = 0b1111 << shiftR;

	shiftVR = move->y;
	shiftVL = shiftVR - 3;
	checkVL = 0b1111 << shiftVL;
	checkVR = 0b1111 << shiftVR;

	// for each axis make capture
	for (int axis = 0; axis < 4; axis++)
	{
		plLine = plBitBoard->getLine((bitboardAxis)axis, move->x, move->y);
		opLine = opBitBoard->getLine((bitboardAxis)axis, move->x, move->y);

		if (axis == 1)
		{
			if ((plLine & checkVL) >> shiftVL == plVerify
				&& (opLine & checkVL) >> shiftVL == opVerify)
			{
				opBitBoard->set(move->x, move->y - 1, false);
				opBitBoard->set(move->x, move->y - 2, false);
				nbCapture += 2;
			}
			if ((plLine & checkVR) >> shiftVR == plVerify
				&& (opLine & checkVR) >> shiftVR == opVerify)
			{
				opBitBoard->set(move->x, move->y + 1, false);
				opBitBoard->set(move->x, move->y + 2, false);
				nbCapture += 2;
			}
		}
		else
		{
			if ((plLine & checkL) >> shiftL == plVerify
				&& (opLine & checkL) >> shiftL == opVerify)
			{
				opBitBoard->set(move->x - dirx[axis], move->y - diry[axis], false);
				opBitBoard->set(move->x - (2 * dirx[axis]), move->y - (2 * diry[axis]), false);
				nbCapture += 2;
			}
			if ((plLine & checkR) >> shiftR == plVerify
				&& (opLine & checkR) >> shiftR == opVerify)
			{
				opBitBoard->set(move->x + dirx[axis], move->y + diry[axis], false);
				opBitBoard->set(move->x + (2 * dirx[axis]), move->y + (2 * diry[axis]), false);
				nbCapture += 2;
			}
		}
	}

	return (nbCapture);
}


bool	Grid::checkDoubleFreeThree(
				int x, int y, BitBoard *plBitBoard, BitBoard *opBitBoard)
{
	int	plLVerifyL, plLVerifyM, plLVerifyR,
		plRVerifyL, plRVerifyM, plRVerifyR,
		shiftL, shiftR, shiftVL, shiftVR,
		checkL, checkR, checkVL, checkVR,
		opCheckL, opCheckR, opCheckVL, opCheckVR,
		tmpL, tmpR,
		nbThreeTree;
	int	plLine, opLine;

	nbThreeTree = 0;

	// Create vefify
	plLVerifyL = 0b00011;
	plLVerifyM = 0b00101;
	plLVerifyR = 0b00110;
	plRVerifyL = 0b01100;
	plRVerifyM = 0b10100;
	plRVerifyR = 0b11000;

	// Create shift and check
	shiftR = x - 1;
	shiftL = shiftR - 2;
	checkL = 0b11111 << shiftL;
	checkR = 0b11111 << shiftR;
	shiftVR = y - 1;
	shiftVL = shiftVR - 2;
	checkVL = 0b11111 << shiftVL;
	checkVR = 0b11111 << shiftVR;

	opCheckL = 0b111111 << (x - 4);
	opCheckR = 0b111111 << (x - 1);
	opCheckVL = 0b111111 << (y - 4);
	opCheckVR = 0b111111 << (y - 1);

	// for each axis
	for (int axis = 0; axis < 4; axis++)
	{
		plLine = plBitBoard->getLine((bitboardAxis)axis, x, y);
		opLine = opBitBoard->getLine((bitboardAxis)axis, x, y);

		if (axis == 1)
		{
			tmpL = (plLine & checkVL) >> shiftVL;
			tmpR = (plLine & checkVR) >> shiftVR;

			if ((tmpL == plLVerifyL || tmpL == plLVerifyM || tmpL == plLVerifyR)
					&& (opLine & opCheckVL) == 0)
				nbThreeTree++;
			if ((tmpR == plRVerifyL || tmpR == plRVerifyM || tmpR == plRVerifyR)
					&& (opLine & opCheckVR) == 0)
				nbThreeTree++;
		}
		else
		{
			tmpL = (plLine & checkL) >> shiftL;
			tmpR = (plLine & checkR) >> shiftR;

			if ((tmpL == plLVerifyL || tmpL == plLVerifyM || tmpL == plLVerifyR)
					&& (opLine & opCheckL) == 0)
				nbThreeTree++;
			if ((tmpR == plRVerifyL || tmpR == plRVerifyM || tmpR == plRVerifyR)
					&& (opLine & opCheckR) == 0)
				nbThreeTree++;
		}
	}

	return (nbThreeTree >= 2);
}


bool	Grid::checkProRule(int x, int y, inter_type interPlayer, int nbMoves)
{
	int	center, tmpX, tmpY, tmp;

	center = GRID_W_INTER / 2;
	// The first stone must be placed in the center of the board.
	if (nbMoves == 0 && (x != center || y != center))
		return (false);
	// The second stone may be placed anywhere on the board.
	// The third stone must be placed at least three
	// intersections away from the first stone
	else if (nbMoves == 2)
	{
		tmpX = center - x;
		tmpY = center - y;
		tmp = tmpX * tmpX + tmpY * tmpY;
		if (tmp <= 4 || tmp == 8)
			return (false);
	}

	return (true);
}


void	Grid::applyHistoryToGrid(void)
{
	std::pair<BitBoard, BitBoard>	savedBoard;

	savedBoard = this->boardHistory[this->boardHistoryId];
	this->bitboardL = savedBoard.first;
	this->bitboardR = savedBoard.second;

	this->historyIdString = std::to_string(this->boardHistoryId + 1) + " / "
								+ std::to_string(this->boardHistory.size());
}

// void	Grid::insertMoves(
// 				std::vector<sf::Vector2i> &moves, sf::Vector2i *move,
// 				bool reverse, Evaluation *evaluator,
// 				Player *player, Player *opponent)
// {
// 	int			eval, tmpEval, plCapture, opCapture, start, end, mid;
// 	inter_type	plType, opType;

// 	plCapture = player->getCaptured();
// 	opCapture = opponent->getCaptured();
// 	plType = player->getInterType();
// 	opType = opponent->getInterType();

// 	eval = evaluator->evaluationPosition(
// 						this, plType, opType, plCapture, opCapture,
// 						move->x, move->y);

// 	start = 0;
// 	end = moves.size();
// 	if (end == 0)
// 	{
// 		moves.push_back(*move);
// 		return ;
// 	}

// 	while (start < end)
// 	{
// 		mid = (start + end) / 2;

// 		tmpEval = evaluator->evaluationPosition(
// 						this, plType, opType, plCapture, opCapture,
// 						moves[mid].x, moves[mid].y);
// 		if (!reverse)
// 		{
// 			if (tmpEval < eval)
// 				start = mid + 1;
// 			else if (tmpEval > eval)
// 				end = mid - 1;
// 			else
// 				break;
// 		}
// 		else
// 		{
// 			if (tmpEval > eval)
// 				start = mid + 1;
// 			else if (tmpEval < eval)
// 				end = mid - 1;
// 			else
// 				break;
// 		}
// 	}

// 	if (start == end)
// 		mid = start;

// 	if (mid == moves.size() - 1)
// 		moves.push_back(*move);
// 	else
// 		moves.insert(moves.begin() + mid, *move);
// }
