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
	// this->dirs[DIR_L] = sf::Vector2i(-1, 0);
	// this->dirs[DIR_UL] = sf::Vector2i(-1, -1);
	// this->dirs[DIR_U] = sf::Vector2i(0, -1);
	// this->dirs[DIR_UR] = sf::Vector2i(1, -1);
	// this->dirs[DIR_R] = sf::Vector2i(1, 0);
	// this->dirs[DIR_DR] = sf::Vector2i(1, 1);
	// this->dirs[DIR_D] = sf::Vector2i(0, 1);
	// this->dirs[DIR_DL] = sf::Vector2i(-1, 1);

	this->historyIdString = "";
	// this->createBoardState();

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


// std::string	Grid::getCurrentBoardState(void)
// {
// 	return (this->currentBoardState);
// }


// std::string	Grid::getCurrentBoardStateOpti(void)
// {
// 	return (this->currentBoardStateOpti);
// }

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
	// for (int i = 0; i < GRID_NB_INTER; i++)
	// {
	// 	this->gridState[i].type = grid.gridState[i].type;
	// 	for (int j = 0; j < 8; j++)
	// 		this->gridState[i].neighbor[j] = grid.gridState[i].neighbor[j];
	// }

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

	// TODO : CHECK DOUBLE FREE THREE
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

	// TODO : CHECK PRO RULE
	// // check rule
	// if (this->rule == PRO && nbMoves <= 2)
	// 	if (!this->checkProRule(x, y, plType, nbMoves))
	// 		return (false);

	return (true);
}


bool	Grid::putStone(
				sf::Vector2i *move, int nbMoves, Player *player, Player *opponent)
{
	inter_type	plType, opType;
	// int			interId;

	plType = player->getInterType();
	opType = opponent->getInterType();

	if (!this->checkLegalMove(move->x, move->y, nbMoves, plType, opType))
		return (false);

	if (plType == INTER_LEFT)
		this->bitboardL.set(move->x, move->y, true);
	else
		this->bitboardR.set(move->x, move->y, true);

	if (plType == INTER_LEFT)
		player->addCaptured(this->makeCapture(
									move, &this->bitboardL, &this->bitboardR));
	else
		player->addCaptured(this->makeCapture(
									move, &this->bitboardR, &this->bitboardL));

	// this->setInterState(move->x, move->y, plType);
	// player->addCaptured(this->checkCapture(move, plType, opType));
	// this->updateNeighbor(move->x, move->y);

	// interId = move->y * GRID_W_INTER + move->x;
	// if (plType == INTER_LEFT)
	// 	this->updateBoardState(interId, 'L');
	// else if (plType == INTER_RIGHT)
	// 	this->updateBoardState(interId, 'R');
	// else
	// 	this->updateBoardState(interId, 'E');

	return (true);
}


bool	Grid::checkWinCondition(Player *player, Player *opponent)
{
	int		winCase, check;
	bool	isWin;

	// Check if the player capture at least 10 opponent's stones
	if (player->getCaptured() >= WIN_CAPTURE)
	{
		player->setWinState(WIN_STATE_CAPTURE);
		this->previewLegal = false;
		return (true);
	}

	// TODO: DISABLE WIN WHEN CAPTURE
	// Check win by align
	winCase = 0b11111;

	if (player->getInterType() == INTER_LEFT)
	{
		for (int i = 0; i < GRID_W_INTER; i++)
		{
			check = 0b11111;
			for (int j = 0; j < GRID_W_INTER - 4; j++)
			{
				if ((this->bitboardL.bbH[i] & check) >> j == winCase)
				{
					isWin = this->validateWin(
									player, opponent, &this->bitboardL,
									&this->bitboardR, 'H', j, i);
					if (isWin)
						return (true);
				}
				if ((this->bitboardL.bbV[i] & check) >> j == winCase)
				{
					isWin = this->validateWin(
									player, opponent, &this->bitboardL,
									&this->bitboardR, 'V', j, i);
					if (isWin)
						return (true);
				}
				if ((this->bitboardL.bbD[i] & check) >> j == winCase)
				{
					isWin = this->validateWin(
									player, opponent, &this->bitboardL,
									&this->bitboardR, 'D', j, i);
					if (isWin)
						return (true);
				}
				if ((this->bitboardL.bbA[i] & check) >> j == winCase)
				{
					isWin = this->validateWin(
									player, opponent, &this->bitboardL,
									&this->bitboardR, 'A', j, i);
					if (isWin)
						return (true);
				}
				check <<= 1;
			}
		}
	}
	else
	{
		for (int i = 0; i < GRID_W_INTER; i++)
		{
			check = 0b11111;
			for (int j = 0; j < GRID_W_INTER - 4; j++)
			{
				if ((this->bitboardR.bbH[i] & check) >> j == winCase)
				{
					isWin = this->validateWin(
								player, opponent, &this->bitboardR,
								&this->bitboardL, 'H', j, i);
					if (isWin)
						return (true);
				}
 				if ((this->bitboardR.bbV[i] & check) >> j == winCase)
				{
					isWin = this->validateWin(
								player, opponent, &this->bitboardR,
								&this->bitboardL, 'V', j, i);
					if (isWin)
						return (true);
				}
				if ((this->bitboardR.bbD[i] & check) >> j == winCase)
				{
					isWin = this->validateWin(
								player, opponent, &this->bitboardR,
								&this->bitboardL, 'D', j, i);
					if (isWin)
						return (true);
				}
				if ((this->bitboardR.bbA[i] & check) >> j == winCase)
				{
					isWin = this->validateWin(
								player, opponent, &this->bitboardR,
								&this->bitboardL, 'A', j, i);
					if (isWin)
						return (true);
				}
				check <<= 1;
			}
		}
	}

	return (false);
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


// std::vector<sf::Vector2i>	Grid::getLegalMoves(Player *player, Player *opponent)
// {
// 	std::vector<sf::Vector2i>	legalMoves;
// 	int							nbMoves = player->getMoves() + opponent->getMoves();

// 	inter_type					plState = player->getInterType();
// 	inter_type					opState = opponent->getInterType();

// 	for (int i = 0; i < GRID_W_INTER; i++)
// 	{
// 		for (int j = 0; j < GRID_W_INTER; j++)
// 		{
// 			if (checkLegalMove(i, j, nbMoves, plState, opState))
// 				legalMoves.push_back(sf::Vector2i(i, j));
// 		}
// 	}

// 	return (legalMoves);
// }


// bool	Grid::checkInterestingMove(int x, int y)
// {
// 	int			new_x, new_y;
// 	inter_type	state;

// 	if (this->getInterState(x, y) != INTER_EMPTY)
// 		return (false);

// 	for (int i = 0; i < 8; i++)
// 	{
// 		// Check for intersection next to stone
// 		new_x = x + this->dirs[i].x;
// 		new_y = y + this->dirs[i].y;
// 		state = this->getInterState(new_x, new_y);
// 		if (state && state != INTER_INVALID)
// 			return (true);
// 	}

// 	return (false);
// }


// std::vector<sf::Vector2i>	Grid::getInterestingMoves(Player *player, Player *opponent)
// {
// 	std::vector<sf::Vector2i>	InterestingMoves;
// 	inter_type					plType, opType;
// 	int							nbMoves;

// 	nbMoves = player->getMoves() + opponent->getMoves();
// 	plType = player->getInterType();
// 	opType = opponent->getInterType();
// 	// Case of no bbox set (0 stone on grid)
// 	if (this->bboxUL.x > this->bboxDR.x)
// 	{
// 		for (int i = 0; i < GRID_W_INTER; i++)
// 		{
// 			for (int j = 0; j < GRID_W_INTER; j++)
// 			{
// 				if(checkInterestingMove(i, j))
// 					if (checkLegalMove(i, j, nbMoves, plType, opType))
// 						InterestingMoves.push_back(sf::Vector2i(i, j));
// 			}
// 		}
// 	}
// 	// Check only tile in bbox to know if there are interesting
// 	else
// 	{
// 		for (int i = this->bboxUL.x; i <= this->bboxDR.x; i++)
// 		{
// 			for (int j = this->bboxUL.y; j <= this->bboxDR.y; j++)
// 			{
// 				if(checkInterestingMove(i, j))
// 					if (checkLegalMove(i, j, nbMoves, plType, opType))
// 						InterestingMoves.push_back(sf::Vector2i(i, j));
// 			}
// 		}
// 	}

// 	return (InterestingMoves);
// }


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


// void	Grid::saveWinPos(std::vector<sf::Vector2i> *leftWinPos, std::vector<sf::Vector2i> *rightWinPos)
// {
// 	*leftWinPos = this->leftWinPos;
// 	*rightWinPos = this->rightWinPos;
// }


// void	Grid::loadWinPos(std::vector<sf::Vector2i> *leftWinPos, std::vector<sf::Vector2i> *rightWinPos)
// {
// 	this->leftWinPos = *leftWinPos;
// 	this->rightWinPos = *rightWinPos;
// }


// bool	Grid::checkWinCondition(Player *me, Player *oppenent, sf::Vector2i move)
// {
// 	// Check if the player capture at least 10 opponent's stones
// 	if (me->getCaptured() >= WIN_CAPTURE)
// 	{
// 		me->setWinState(WIN_STATE_CAPTURE);
// 		this->previewLegal = false;
// 		return (true);
// 	}

// 	// Check if there at least 5 align stones
// 	if (this->checkWinCaptureCase(me, oppenent, &move, DIR_L, DIR_R))
// 		return (true);
// 	if (this->checkWinCaptureCase(me, oppenent, &move, DIR_UL, DIR_DR))
// 		return (true);
// 	if (this->checkWinCaptureCase(me, oppenent, &move, DIR_U, DIR_D))
// 		return (true);
// 	if (this->checkWinCaptureCase(me, oppenent, &move, DIR_UR, DIR_DL))
// 		return (true);

// 	// Check if any valid 5 stones line
// 	if (this->getInterState(move.x, move.y) == INTER_LEFT)
// 	{
// 		for (std::size_t i = 0; i < this->leftWinPos.size(); i++)
// 		{
// 			move.x = this->leftWinPos[i].x;
// 			move.y = this->leftWinPos[i].y;
// 			if (this->checkWinCaptureCase(me, oppenent, &move, DIR_L, DIR_R))
// 				return (true);
// 			if (this->checkWinCaptureCase(me, oppenent, &move, DIR_UL, DIR_DR))
// 				return (true);
// 			if (this->checkWinCaptureCase(me, oppenent, &move, DIR_U, DIR_D))
// 				return (true);
// 			if (this->checkWinCaptureCase(me, oppenent, &move, DIR_UR, DIR_DL))
// 				return (true);
// 		}
// 	}
// 	else
// 	{
// 		for (std::size_t i = 0; i < this->rightWinPos.size(); i++)
// 		{
// 			move.x = this->rightWinPos[i].x;
// 			move.y = this->rightWinPos[i].y;
// 			if (this->checkWinCaptureCase(me, oppenent, &move, DIR_L, DIR_R))
// 				return (true);
// 			if (this->checkWinCaptureCase(me, oppenent, &move, DIR_UL, DIR_DR))
// 				return (true);
// 			if (this->checkWinCaptureCase(me, oppenent, &move, DIR_U, DIR_D))
// 				return (true);
// 			if (this->checkWinCaptureCase(me, oppenent, &move, DIR_UR, DIR_DL))
// 				return (true);
// 		}
// 	}

// 	// Check for draw
// 	// Opti by find a 'E' into currentBoardState
// 	if (this->currentBoardState.find('E') == this->currentBoardState.npos)
// 	{
// 		me->setWinState(WIN_STATE_NONE);
// 		oppenent->setWinState(WIN_STATE_NONE);
// 		this->previewLegal = false;
// 		return (true);
// 	}

// 	return (false);
// }


////////////////////////////////////////////////////////////////////////////////
// Private methods
////////////////////////////////////////////////////////////////////////////////

bool	Grid::validateWin(
				Player *player, Player *opponent,
				BitBoard *plBitBoard, BitBoard *opBitBoard,
				char bbType, int x, int y)
{
	int	checkL, checkR,
		checkVL, checkVR,
		plVerify, opVerifyL, opVerifyR,
		shiftL, shiftR,
		shiftVL, shiftVR,
		yD, yA;

	if (bbType == 'V')
	{
		yD = y;
		y = x;
		x = yD;
	}
	else if (bbType == 'D')
	{
		y = y - x;
		if (y < 0)
			y += GRID_W_INTER;
	}
	else if (bbType == 'A')
	{
		y = (y + x) % GRID_W_INTER;
	}

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

		yD = (y + x) % GRID_W_INTER;

		yA = y - x;
		if (yA < 0)
			yA = GRID_W_INTER + yA;

		// Check if stone is capturable on horizontal axis
		if (bbType != 'H' && x > 0 && x < GRID_W_INTER - 1)
		{
			if (((plBitBoard->bbH[y] & checkL) >> shiftL == plVerify
					&& ((opBitBoard->bbH[y] & checkL) >> shiftL == opVerifyL
						|| (opBitBoard->bbH[y] & checkL) >> shiftL == opVerifyR))
				|| ((plBitBoard->bbH[y] & checkR) >> shiftR == plVerify
					&& ((opBitBoard->bbH[y] & checkR) >> shiftR == opVerifyL
						|| (opBitBoard->bbH[y] & checkR) >> shiftR == opVerifyR)))
			{
				if (opponent->getCaptured() >= WIN_CAPTURE - 2)
				{
					opponent->addCaptured(2);
					opponent->setWinState(WIN_STATE_CAPTURE);
					this->previewLegal = false;
					return (true);
				}
				return (false);
			}
		}

		// Check if stone is capturable on vertical axis
		if (bbType != 'V' && y > 0 && y < GRID_W_INTER - 1)
		{
			if (((plBitBoard->bbV[x] & checkVL) >> shiftVL == plVerify
					&& ((opBitBoard->bbV[x] & checkVL) >> shiftVL == opVerifyL
						|| (opBitBoard->bbV[x] & checkVL) >> shiftVL == opVerifyR))
				|| ((plBitBoard->bbV[x] & checkVR) >> shiftVR == plVerify
					&& ((opBitBoard->bbV[x] & checkVR) >> shiftVR == opVerifyL
						|| (opBitBoard->bbV[x] & checkVR) >> shiftVR == opVerifyR)))
			{
				if (opponent->getCaptured() >= WIN_CAPTURE - 2)
				{
					opponent->addCaptured(2);
					opponent->setWinState(WIN_STATE_CAPTURE);
					this->previewLegal = false;
					return (true);
				}
				return (false);
			}
		}

		// Check if stone is capturable on diagonal axis
		if (bbType != 'D')
		{
			if (((plBitBoard->bbD[yD] & checkL) >> shiftL == plVerify
					&& ((opBitBoard->bbD[yD] & checkL) >> shiftL == opVerifyL)
						|| ((opBitBoard->bbD[yD] & checkL) >> shiftL == opVerifyR))
				|| ((plBitBoard->bbD[yD] & checkR) >> shiftR == plVerify
					&& ((opBitBoard->bbD[yD] & checkR) >> shiftR == opVerifyL)
						|| ((opBitBoard->bbD[yD] & checkR) >> shiftR == opVerifyR)))
			{
				if (opponent->getCaptured() >= WIN_CAPTURE - 2)
				{
					opponent->addCaptured(2);
					opponent->setWinState(WIN_STATE_CAPTURE);
					this->previewLegal = false;
					return (true);
				}
				return (false);
			}
		}

		// Check if stone is capturable on diagonal axis
		if (bbType != 'A')
		{
			if (((plBitBoard->bbA[yA] & checkL) >> shiftL == plVerify
					&& ((opBitBoard->bbA[yA] & checkL) >> shiftL == opVerifyL
						|| (opBitBoard->bbA[yA] & checkL) >> shiftL == opVerifyR))
				|| ((plBitBoard->bbA[yA] & checkR) >> shiftR == plVerify
					&& ((opBitBoard->bbA[yA] & checkR) >> shiftR == opVerifyL
						|| (opBitBoard->bbA[yA] & checkR) >> shiftR == opVerifyR)))
			{
				if (opponent->getCaptured() >= WIN_CAPTURE - 2)
				{
					opponent->addCaptured(2);
					opponent->setWinState(WIN_STATE_CAPTURE);
					this->previewLegal = false;
					return (true);
				}
				return (false);
			}
		}

		if (bbType == 'H')
		{
			x = (x + 1) % GRID_W_INTER;
		}
		else if (bbType == 'V')
		{
			y = (y + 1) % GRID_W_INTER;
		}
		else if (bbType == 'D')
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
	int	checkL, checkR,
		checkVL,
		checkVR,
		plVerify, opVerify,
		shiftL, shiftR,
		shiftVL, shiftVR,
		yD, yA,
		nbCapture;

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

	yD = (move->y + move->x) % GRID_W_INTER;

	yA = move->y - move->x;
	if (yA < 0)
		yA = GRID_W_INTER + yA;

	// Check capture in horizontal
	if ((plBitBoard->bbH[move->y] & checkL) >> shiftL == plVerify
		&& (opBitBoard->bbH[move->y] & checkL) >> shiftL == opVerify)
	{
		opBitBoard->set(move->x - 1, move->y, false);
		opBitBoard->set(move->x - 2, move->y, false);
		nbCapture += 2;
	}
	if ((plBitBoard->bbH[move->y] & checkR) >> shiftR == plVerify
		&& (opBitBoard->bbH[move->y] & checkR) >> shiftR == opVerify)
	{
		opBitBoard->set(move->x + 1, move->y, false);
		opBitBoard->set(move->x + 2, move->y, false);
		nbCapture += 2;
	}

	// Check capture in vertical
	if ((plBitBoard->bbV[move->x] & checkVL) >> shiftVL == plVerify
		&& (opBitBoard->bbV[move->x] & checkVL) >> shiftVL == opVerify)
	{
		opBitBoard->set(move->x, move->y - 1, false);
		opBitBoard->set(move->x, move->y - 2, false);
		nbCapture += 2;
	}
	if ((plBitBoard->bbV[move->x] & checkVR) >> shiftVR == plVerify
		&& (opBitBoard->bbV[move->x] & checkVR) >> shiftVR == opVerify)
	{
		opBitBoard->set(move->x, move->y + 1, false);
		opBitBoard->set(move->x, move->y + 2, false);
		nbCapture += 2;
	}

	// Check capture in diagonal
	if ((plBitBoard->bbD[yD] & checkL) >> shiftL == plVerify
		&& (opBitBoard->bbD[yD] & checkL) >> shiftL == opVerify)
	{
		opBitBoard->set(move->x - 1, move->y + 1, false);
		opBitBoard->set(move->x - 2, move->y + 2, false);
		nbCapture += 2;
	}
	if ((plBitBoard->bbD[yD] & checkR) >> shiftR == plVerify
		&& (opBitBoard->bbD[yD] & checkR) >> shiftR == opVerify)
	{
		opBitBoard->set(move->x + 1, move->y - 1, false);
		opBitBoard->set(move->x + 2, move->y - 2, false);
		nbCapture += 2;
	}


	// Check capture in anti diagonal
	if ((plBitBoard->bbA[yA] & checkL) >> shiftL == plVerify
		&& (opBitBoard->bbA[yA] & checkL) >> shiftL == opVerify)
	{
		opBitBoard->set(move->x - 1, move->y - 1, false);
		opBitBoard->set(move->x - 2, move->y - 2, false);
		nbCapture += 2;
	}
	if ((plBitBoard->bbA[yA] & checkR) >> shiftR == plVerify
		&& (opBitBoard->bbA[yA] & checkR) >> shiftR == opVerify)
	{
		opBitBoard->set(move->x + 1, move->y + 1, false);
		opBitBoard->set(move->x + 2, move->y + 2, false);
		nbCapture += 2;
	}

	return (nbCapture);
}


bool	Grid::checkDoubleFreeThree(
				int x, int y, BitBoard *plBitBoard, BitBoard *opBitBoard)
{
	int	verifyL, verifyM, verifyR,
		shiftL, shiftR,
		checkL, checkR;

	// Create vefify
	verifyL = 0b011;
	verifyM = 0b101;
	verifyR = 0b110;

	// TODO : CONTINUE

	// Check for honrizontal

	// Check for vertical

	// Check for diagonal

	// Check for anti diagonal

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



// bool	Grid::checkProRule(int x, int y, inter_type interPlayer, int nbMoves)
// {
// 	// The first stone must be placed in the center of the board.
// 	if (nbMoves == 0 &&
// 		(x != GRID_W_INTER / 2 ||
// 		y != GRID_W_INTER / 2))
// 		return (false);
// 	// The second stone may be placed anywhere on the board.
// 	// The third stone must be placed at least three
// 	// intersections away from the first stone
// 	else if (nbMoves == 2)
// 	{
// 		int	tmp_x, tmp_y;

// 		for (int i = 0; i < 8; i++)
// 		{
// 			tmp_x = x;
// 			tmp_y = y;
// 			for (int j = 0; j < 2; j++)
// 			{
// 				tmp_x += this->dirs[i].x;
// 				tmp_y += this->dirs[i].y;
// 				if (this->getInterState(tmp_x, tmp_y) == interPlayer)
// 					return (false);
// 			}
// 		}
// 	}

// 	return (true);
// }


// bool	Grid::checkDoubleFreeThree(int x, int y, inter_type plState, inter_type opState)
// {
// 	int	tmp_x, tmp_y, nbThreeTree, nbInterMe, nbInterEmpty;
// 	inter_type	tmpState;

// 	nbThreeTree = 0;
// 	for (int i = 0; i < 8; i++)
// 	{
// 		tmp_x = x;
// 		tmp_y = y;

// 		// Check 3 next intersections in dir
// 		nbInterMe = 0;
// 		nbInterEmpty = 0;
// 		for (int j = 0; j < 3; j++)
// 		{
// 			tmp_x += this->dirs[i].x;
// 			tmp_y += this->dirs[i].y;
// 			tmpState = this->getInterState(tmp_x, tmp_y);
// 			if (tmpState == INTER_EMPTY)
// 				nbInterEmpty++;
// 			else if (tmpState == plState)
// 				nbInterMe++;
// 			else
// 				break;
// 		}

// 		// If there is 2 me and one empty, it can be a free three
// 		if (nbInterEmpty != 1 || nbInterMe != 2)
// 			continue;

// 		// Check opponenent obstruction after possible free three
// 		tmp_x += this->dirs[i].x;
// 		tmp_y += this->dirs[i].y;
// 		tmpState = this->getInterState(tmp_x, tmp_y);
// 		if (tmpState == opState)
// 			continue;

// 		// Check opponenent obstruction before possible free three
// 		tmp_x = x - this->dirs[i].x;
// 		tmp_y = y - this->dirs[i].y;
// 		tmpState = this->getInterState(tmp_x, tmp_y);
// 		if (tmpState == opState)
// 			continue;

// 		nbThreeTree++;
// 		if (nbThreeTree > 1)
// 			break;
// 	}

// 	return (nbThreeTree >= 2);
// }


// void	Grid::createCurrentMoveText(void)
// {
// 	this->currentMove = std::to_string(this->idBoardState + 1);
// 	this->currentMove += " / ";
// 	this->currentMove += std::to_string(this->boardStates.size());
// }


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
