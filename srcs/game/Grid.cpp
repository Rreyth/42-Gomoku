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


BboxManager	*Grid::getBboxManager(void)
{
	return (&this->bboxManager);
}


void	Grid::setBitBoard(BitBoard *bitBoard, inter_type type)
{
	if (type == INTER_LEFT)
		this->bitboardL = *bitBoard;
	else
		this->bitboardR = *bitBoard;
}


void	Grid::setBboxManager(BboxManager *bboxManager)
{
	this->bboxManager = *bboxManager;
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

	// TODO: REMOVE
	// std::vector<sf::Vector2i>	positions = this->bboxManager.getListPosition();
	// int	x, y;

	// for (int i = 0; i < positions.size(); i++)
	// {
	// 	x = positions[i].x;
	// 	y = positions[i].y;
	// 	if (this->bitboardL.get(x, y) || this->bitboardR.get(x, y))
	// 		continue;

	// 	drawX = this->x + x * GRID_SQUARE_SIZE + 10;
	// 	drawY = this->y + y * GRID_SQUARE_SIZE + 10;
	// 	textureManager->drawTexture(window, SPRITE_STONE_PREVIEW,
	// 								drawX, drawY, MID_CENTER);
	// }
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
	if (!this->putStoneAI(move, nbMoves, player, opponent))
		return (false);

	this->bboxManager.update(move->x, move->y);

	return (true);
}


bool	Grid::putStoneAI(sf::Vector2i *move, int nbMoves, Player *player, Player *opponent)
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



void	Grid::removeStone(sf::Vector2i *move, inter_type plType)
{
	if (plType == INTER_LEFT)
		this->bitboardL.set(move->x, move->y, false);
	else
		this->bitboardR.set(move->x, move->y, false);
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
	// Check capture for auto win
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

	if (player->getInterType() == INTER_LEFT)
		return (this->checkWinByAlign(
						player, opponent, &this->bitboardL, &this->bitboardR));

	return (this->checkWinByAlign(
					player, opponent, &this->bitboardR, &this->bitboardL));
}


void	Grid::updateBboxManager(sf::Vector2i *move)
{
	this->bboxManager.update(move->x, move->y);
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
	this->bboxManager.clear();
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
	this->bboxManager.clear();
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
	int							nbMoves, check, checkV, yD, yA, maxLimit, nbBbox;
	inter_type					plState, opState;
	std::vector<sf::Vector2i>	moves;
	std::vector<Bbox>			*bboxes;

	nbMoves = player->getMoves() + opponent->getMoves();
	plState = player->getInterType();
	opState = opponent->getInterType();
	maxLimit = GRID_W_INTER - 1;

	bboxes = this->bboxManager.getBboxes();
	nbBbox = bboxes->size();

	// Get moves close to stone with bboxManager
	for (int i = 0; i < nbBbox; i++)
	{
		for (int y = bboxes->at(i).Ly; y <= bboxes->at(i).Ry; y++)
		{
			for (int x = bboxes->at(i).Lx; x <= bboxes->at(i).Rx; x++)
			{
				// Remove the move if it's illegal
				if (!checkLegalMove(x, y, nbMoves, plState, opState))
					continue;

				moves.push_back(sf::Vector2i(x, y));
			}
		}
	}

	return (moves);
}


std::vector<Move>	Grid::getInterestingMovesSorted(
							Evaluation *evaluator,
							Player *player, Player *opponent,
							bool reverse, Tracker *tracker)
{
	int							size, eval, j, nbMove, plCapture, opCapture;
	Move						tmp;
	std::vector<Move>			moves;
	std::vector<sf::Vector2i>	movesPosition;
	BitBoard					plBitboard, opBitboard;

	// TODO : REMOVE
	std::clock_t	start;
	int				diff;
	start = std::clock();

	movesPosition = this->getInterestingMoves(player, opponent);

	diff = ((double)(std::clock() - start) / CLOCKS_PER_SEC) * 1000000;
	tracker->getMoveTime += diff;
	tracker->getSortMoveNumber++;

	size = movesPosition.size();

	tracker->sortSizeTotal += size;
	if (size < tracker->sortSizeMin)
		tracker->sortSizeMin = size;
	if (size > tracker->sortSizeMax)
		tracker->sortSizeMax = size;

	if (size < 2)
		return (moves);

	start = std::clock();

	plCapture = player->getCaptured();
	opCapture = opponent->getCaptured();
	nbMove = player->getMoves() + opponent->getMoves();

	if (player->getInterType() == INTER_LEFT)
	{
		plBitboard = this->bitboardL;
		opBitboard = this->bitboardR;
	}
	else
	{
		plBitboard = this->bitboardR;
		opBitboard = this->bitboardL;
	}
	for (int i = 0; i < size; i++)
	{
		Move	move;
		move.pos = movesPosition[i];
		
		move.eval = evaluator->evaluationPosition(
											&plBitboard, &opBitboard,
											plCapture, opCapture,
											move.pos.x, move.pos.y);
		if (!reverse)
			move.eval = -move.eval;
		moves.push_back(move);
	}

	if (!reverse)
	{
		for (int gap = size / 2; gap > 0; gap /= 2)
		{
			for (int i = gap; i < size; i++)
			{
				tmp = moves[i];

				for (j = i; j >= gap && moves[j - gap] > tmp; j -= gap)
				{
					moves[j] = moves[j - gap];
				}

				moves[j] = tmp;
			}
		}
	}
	else
	{
		for (int gap = size / 2; gap > 0; gap /= 2)
		{
			for (int i = gap; i < size; i++)
			{
				tmp = moves[i];

				for (j = i; j >= gap && moves[j - gap] < tmp; j -= gap)
				{
					moves[j] = moves[j - gap];
				}

				moves[j] = tmp;
			}
		}
	}

	diff = ((double)(std::clock() - start) / CLOCKS_PER_SEC) * 1000000;
	tracker->sortMoveTime += diff;

	return (moves);
}

////////////////////////////////////////////////////////////////////////////////
// Private methods
////////////////////////////////////////////////////////////////////////////////

bool	Grid::checkWinByAlign(
				Player *player, Player *opponent,
				BitBoard *plBitboard, BitBoard *opBitboard)
{
	int		winCase, check, checkV, test, lines[4];
	bool	isWin;

	// Check win by axis
	winCase = 0b11111;
	std::vector<Bbox> *bboxes = this->bboxManager.getBboxes();
	int	nbBoxes = bboxes->size();

	for (int i = 0; i < nbBoxes; i++)
	{
		for (int y = bboxes->at(i).Ly; y <= bboxes->at(i).Ry; y++)
		{
			checkV = 0b11111 << y;
			lines[0] = plBitboard->getLine(BBH, 0, y);
			for (int x = bboxes->at(i).Lx; x <= bboxes->at(i).Rx; x++)
			{
				check = 0b11111 << x;
				lines[1] = plBitboard->getLine(BBV, x, y);
				lines[2] = plBitboard->getLine(BBD, x, y);
				lines[3] = plBitboard->getLine(BBA, x, y);
				for (int axis = 0; axis < 4; axis++)
				{
					if (axis == 1)
						test = (lines[axis] & checkV) >> y;
					else
						test = (lines[axis] & check) >> x;
					if (test == winCase)
					{
						isWin = this->validateWin(
										player, opponent, plBitboard,
										opBitboard, (bitboardAxis)axis, x, y);
						if (isWin)
							return (true);
					}
				}
			}
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
		if (bbAxis != BBH && x >= 0 && x < GRID_W_INTER - 1)
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
		if (bbAxis != BBV && y >= 0 && y < GRID_W_INTER - 1)
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
			&& x >= 0 && x < GRID_W_INTER - 1
			&& y >= 0 && y < GRID_W_INTER - 1)
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
			&& x >= 0 && x < GRID_W_INTER - 1
			&& y >= 0 && y < GRID_W_INTER - 1)
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
