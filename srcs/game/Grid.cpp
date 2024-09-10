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

	this->dirs[DIR_L] = sf::Vector2i(-1, 0);
	this->dirs[DIR_UL] = sf::Vector2i(-1, -1);
	this->dirs[DIR_U] = sf::Vector2i(0, -1);
	this->dirs[DIR_UR] = sf::Vector2i(1, -1);
	this->dirs[DIR_R] = sf::Vector2i(1, 0);
	this->dirs[DIR_DR] = sf::Vector2i(1, 1);
	this->dirs[DIR_D] = sf::Vector2i(0, 1);
	this->dirs[DIR_DL] = sf::Vector2i(-1, 1);

	this->currentMove = "";
	// this->currentBoardState = "";
	this->createBoardState();

	this->bboxUL = sf::Vector2i(GRID_W_INTER - 1, GRID_W_INTER - 1);
	this->bboxDR = sf::Vector2i(0, 0);
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


std::string	Grid::getCurrentMove(void)
{
	return (this->currentMove);
}


std::string	Grid::getCurrentBoardState(void)
{
	return (this->currentBoardState);
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
	for (int i = 0; i < GRID_NB_INTER; i++)
	{
		this->gridState[i].type = grid.gridState[i].type;
		for (int j = 0; j < 8; j++)
			this->gridState[i].neighbor[j] = grid.gridState[i].neighbor[j];
	}

	this->bboxUL = grid.bboxUL;
	this->bboxDR = grid.bboxDR;
	this->currentBoardState = grid.currentBoardState;

	return (*this);
}

////////////////////////////////////////////////////////////////////////////////
// Public methods
////////////////////////////////////////////////////////////////////////////////

void	Grid::tick(display_state *displayState, Mouse *mouse, Player *leftPlayer, Player *rightPlayer,
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
	int px = (mx - GRID_SQUARE_HALF_SIZE) / GRID_SQUARE_SIZE;
	int py = (my - GRID_SQUARE_HALF_SIZE) / GRID_SQUARE_SIZE;

	// TODO: REMOVE PRINT TEST PART
	if (mouse->isPressed(MBUT_RIGHT))
	{
		std::string	xaxis = "ABCDEFGHIJKLMNOPQ";
		intersection	*inter = this->getIntersection(px, py);
		std::cout << "\nPosition " << xaxis[px] << py + 1 << " :" << std::endl;
		std::cout << "    Evalutation Left  : "
					<< evaluator->evaluationPosition(
									this, INTER_LEFT, INTER_RIGHT,
									leftPlayer->getCaptured(),
									rightPlayer->getCaptured(),
									px, py)
					<< std::endl;
		std::cout << "    Evalutation Right : "
					<< evaluator->evaluationPosition(
									this, INTER_RIGHT, INTER_LEFT,
									rightPlayer->getCaptured(),
									leftPlayer->getCaptured(),
									px, py)
					<< std::endl;
		std::cout << "    Neighbors :" << std::endl;
		std::cout << "        " << inter->neighbor[DIR_UL] << " " << inter->neighbor[DIR_U] <<  " " << inter->neighbor[DIR_UR] << std::endl;
		std::cout << "        " << inter->neighbor[DIR_L] << " S " << inter->neighbor[DIR_R] << std::endl;
		std::cout << "        " << inter->neighbor[DIR_DL] << " " << inter->neighbor[DIR_D] <<  " " << inter->neighbor[DIR_DR] << std::endl;
	}

	if (px == this->previewX && py == this->previewY && !mouse->isPressed(MBUT_LEFT))
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


void	Grid::draw(sf::RenderWindow *window, sf::Text *text, TextureManager *textureManager)
{
	textureManager->drawTexture(window, SPRITE_GRID, this->x, this->y, TOP_LEFT);

	std::string	xlabels = ".ABCDEFGHIJKLMNOPQR.";
	std::string	label;
	int	interX, interY, drawX, drawY;

	drawY = this->y - 15;
	//letters top
	for (int i = 1; i <= GRID_W_INTER; i++)
	{
		drawX = this->x + i * GRID_SQUARE_SIZE + 10;
		label = "";
		label += xlabels[i];
		drawText(window, text, label, drawX, drawY, 20, sf::Color::White, MID_CENTER);
	}

	//numbers left
	drawX = this->x - 10;
	for (int i = 1; i <= GRID_W_INTER; i++)
	{
		drawY = this->y + i * GRID_SQUARE_SIZE - 4 + 10;
		label = std::to_string(i);
		if (i == 1)
			drawText(window, text, label, drawX - 3, drawY, 20, sf::Color::White, MID_RIGHT);
		else
			drawText(window, text, label, drawX, drawY, 20, sf::Color::White, MID_RIGHT);
	}

	//draw stones
	for (int i = 0; i < GRID_NB_INTER; i++)
	{
		if (this->gridState[i].type == INTER_EMPTY || this->gridState[i].type == INTER_INVALID)
			continue;

		interX = i % GRID_W_INTER;
		interY = i / GRID_W_INTER;

		drawX = this->x + (interX + 1) * GRID_SQUARE_SIZE + 10;
		drawY = this->y + (interY + 1) * GRID_SQUARE_SIZE + 10;

		if (this->gridState[i].type == INTER_LEFT)
			textureManager->drawTexture(window, this->leftStone, drawX, drawY, MID_CENTER);
		else
			textureManager->drawTexture(window, this->rightStone, drawX, drawY, MID_CENTER);
	}

	//draw stone preview
	if (this->previewLegal)
	{
		drawX = this->x + (this->previewX + 1) * GRID_SQUARE_SIZE + 10;
		drawY = this->y + (this->previewY + 1) * GRID_SQUARE_SIZE + 10;
		textureManager->drawTexture(window, SPRITE_STONE_PREVIEW, drawX, drawY, MID_CENTER);
	}
}


void	Grid::clearGrid(sprite_name leftStone, sprite_name rightStone, game_rules rule)
{
	this->leftStone = leftStone;
	this->rightStone = rightStone;
	this->rule = rule;
	for (int i = 0; i < GRID_NB_INTER; i++)
	{
		this->gridState[i].type = INTER_EMPTY;
		for (int j = 0; j < 8; j++)
			this->gridState[i].neighbor[j] = 0;
	}
	this->boardStates.clear();
	this->leftWinPos.clear();
	this->rightWinPos.clear();
	this->interestingMovesLeft.clear();
	this->interestingMovesRight.clear();
	this->createBoardState();
}

void	Grid::goToFirstMove(void)
{
	this->idBoardState = 0;
	this->createCurrentMoveText();
	this->setBoardState(this->idBoardState);
}


void	Grid::goToPreviousMove(void)
{
	if (this->idBoardState <= 0)
		return ;

	this->idBoardState -= 1;
	this->createCurrentMoveText();
	this->setBoardState(this->idBoardState);
}


void	Grid::goToNextMove(void)
{
	if (this->idBoardState >= this->boardStates.size() - 1)
		return ;

	this->idBoardState += 1;
	this->createCurrentMoveText();
	this->setBoardState(this->idBoardState);
}


void	Grid::goToLastMove(void)
{
	this->idBoardState = this->boardStates.size() - 1;
	this->createCurrentMoveText();
	this->setBoardState(this->idBoardState);
}


bool	Grid::checkLegalMove(int x, int y, int nbMoves, inter_type plState, inter_type opState)
{
	if (x < 0 || x >= GRID_W_INTER || y < 0 || y >= GRID_W_INTER)
		return (false);

	if (this->getInterState(x, y) != INTER_EMPTY)
		return (false);

	if (this->checkDoubleFreeThree(x, y, plState, opState))
		return (false);

	// check rule
	if (this->rule == PRO && nbMoves <= 2)
		if (!this->checkProRule(x, y, plState, nbMoves))
			return (false);

	return (true);
}


std::vector<sf::Vector2i>	Grid::getLegalMoves(Player *player, Player *opponent)
{
	std::vector<sf::Vector2i>	legalMoves;
	int							nbMoves = player->getMoves() + opponent->getMoves();

	inter_type					plState = player->getInterType();
	inter_type					opState = opponent->getInterType();

	for (int i = 0; i < GRID_W_INTER; i++)
	{
		for (int j = 0; j < GRID_W_INTER; j++)
		{
			if (checkLegalMove(i, j, nbMoves, plState, opState))
				legalMoves.push_back(sf::Vector2i(i, j));
		}
	}

	return (legalMoves);
}


bool	Grid::checkInterestingMove(int x, int y)
{
	int			new_x, new_y;
	inter_type	state;

	if (this->getInterState(x, y) != INTER_EMPTY)
		return (false);

	for (int i = 0; i < 8; i++)
	{
		// Check for intersection next to stone
		new_x = x + this->dirs[i].x;
		new_y = y + this->dirs[i].y;
		state = this->getInterState(new_x, new_y);
		if (state && state != INTER_INVALID)
			return (true);
	}

	return (false);
}


std::vector<sf::Vector2i>	Grid::getInterestingMoves(Player *player, Player *opponent)
{
	std::vector<sf::Vector2i>	InterestingMoves;
	inter_type					plType, opType;
	int							nbMoves;

	nbMoves = player->getMoves() + opponent->getMoves();
	plType = player->getInterType();
	opType = opponent->getInterType();
	// Case of no bbox set (0 stone on grid)
	if (this->bboxUL.x > this->bboxDR.x)
	{
		for (int i = 0; i < GRID_W_INTER; i++)
		{
			for (int j = 0; j < GRID_W_INTER; j++)
			{
				if(checkInterestingMove(i, j))
					if (checkLegalMove(i, j, nbMoves, plType, opType))
						InterestingMoves.push_back(sf::Vector2i(i, j));
			}
		}
	}
	// Check only tile in bbox to know if there are interesting
	else
	{
		for (int i = this->bboxUL.x; i <= this->bboxDR.x; i++)
		{
			for (int j = this->bboxUL.y; j <= this->bboxDR.y; j++)
			{
				if(checkInterestingMove(i, j))
					if (checkLegalMove(i, j, nbMoves, plType, opType))
						InterestingMoves.push_back(sf::Vector2i(i, j));
			}
		}
	}

	return (InterestingMoves);
}


std::vector<sf::Vector2i>	Grid::getInterestingMovesSorted(
									Player *player, Player *opponent,
									Evaluation *evaluator, bool reverse,
									Tracker *tracker)
{
	int							size, eval, j, plCapture, opCapture;
	sf::Vector2i				tmp;
	std::vector<sf::Vector2i>	moves;
	std::vector<int>			evaluations;
	inter_type					plType, opType;

	// TODO : REMOVE
	std::clock_t	start;
	int				diff;
	start = std::clock();

	tracker->getSortMoveNumber++;
	moves = this->getInterestingMoves(player, opponent);
	diff = ((double)(std::clock() - start) / CLOCKS_PER_SEC) * 1000000;
	tracker->getMoveTime += diff;

	start = std::clock();

	size = moves.size();
	if (size < 2)
		return (moves);

	plType = player->getInterType();
	opType = opponent->getInterType();
	plCapture = player->getCaptured();
	opCapture = opponent->getCaptured();

	for (int i = 0; i < size; i++)
		evaluations.push_back(evaluator->evaluationPosition(this,
											plType, opType, plCapture, opCapture,
											moves[i].x, moves[i].y));

	for (int i = 1; i < size; i++)
	{
		tmp = moves[i];
		eval = evaluations[i];
		j = i - 1;

		// Classic order
		if (!reverse)
		{
			while (j >= 0 && evaluations[j] > eval)
			{
				moves[j + 1] = moves[j];
				evaluations[j + 1] = evaluations[j];
				j = j - 1;
			}
		}
		// Reverse order
		else
		{
			while (j >= 0 && evaluations[j] < eval)
			{
				moves[j + 1] = moves[j];
				evaluations[j + 1] = evaluations[j];
				j = j - 1;
			}
		}

		moves[j + 1] = tmp;
		evaluations[j + 1] = eval;
	}

	diff = ((double)(std::clock() - start) / CLOCKS_PER_SEC) * 1000000;
	tracker->sortMoveTime += diff;

	return (moves);
}


void	Grid::computeInterestingMovesSorted(
				Player *player, Player *opponent, Evaluation *evaluator,
				Tracker *tracker)
{
	if (player->getInterType() == INTER_LEFT)
	{
		this->interestingMovesLeft = this->getInterestingMovesSorted(
											player, opponent, evaluator,
											true, tracker);
		this->interestingMovesRight = this->getInterestingMovesSorted(
											opponent, player, evaluator,
											false, tracker);
	}
	else
	{
		this->interestingMovesRight = this->getInterestingMovesSorted(
											player, opponent, evaluator,
											true, tracker);
		this->interestingMovesLeft = this->getInterestingMovesSorted(
											opponent, player, evaluator,
											false, tracker);
	}
}


void	Grid::updateInterestingMovesSorted(
				Player *player, Player *opponent, Evaluation *evaluator,
				Tracker *tracker, sf::Vector2i *move)
{
	int									nbMoves;
	inter_type							type, plType, opType;
	sf::Vector2i						tmpMove;
	std::vector<sf::Vector2i>::iterator	it;

	// TODO : REMOVE
	std::clock_t	start;
	int				diff;
	start = std::clock();

	nbMoves = player->getMoves() + opponent->getMoves();
	plType = player->getInterType();
	opType = opponent->getInterType();

	// Remove new placed stone from left interesting moves
	it = std::find(this->interestingMovesLeft.begin(), this->interestingMovesLeft.end(), *move);
	if (it != this->interestingMovesLeft.end())
		this->interestingMovesLeft.erase(it);

	// Remove new placed stone from right interesting moves
	it = std::find(this->interestingMovesRight.begin(), this->interestingMovesRight.end(), *move);
	if (it != this->interestingMovesRight.end())
		this->interestingMovesRight.erase(it);

	// For each stone arround the new placed stone
	for (int i = 0; i < 8; i++)
	{
		tmpMove = *move + this->dirs[i];

		// Check if the intersection is empty
		type = this->getInterState(tmpMove.x, tmpMove.y);
		if (type != INTER_EMPTY)
			continue;

		// Check if the move is already in the left moves
		it = std::find(this->interestingMovesLeft.begin(),
						this->interestingMovesLeft.end(), tmpMove);

		// Check if the move is legal for player
		if (it == this->interestingMovesLeft.end()
			&& checkLegalMove(tmpMove.x, tmpMove.y, nbMoves, plType, opType))
		{
			if (plType == INTER_LEFT)
				this->insertMoves(
						this->interestingMovesLeft, &tmpMove, true,
						evaluator, player, opponent);
			else
				this->insertMoves(
						this->interestingMovesRight, &tmpMove, false,
						evaluator, player, opponent);
		}

		// Check if the move is already in the right moves
		it = std::find(this->interestingMovesRight.begin(),
						this->interestingMovesRight.end(), tmpMove);

		// Check if the move is legal for opponent
		if (it == this->interestingMovesRight.end()
			&& checkLegalMove(tmpMove.x, tmpMove.y, nbMoves, opType, plType))
		{
			if (opType == INTER_LEFT)
				this->insertMoves(
						this->interestingMovesLeft, &tmpMove, true,
						evaluator, player, opponent);
			else
				this->insertMoves(
						this->interestingMovesRight, &tmpMove, false,
						evaluator, player, opponent);
		}
	}

	diff = ((double)(std::clock() - start) / CLOCKS_PER_SEC) * 1000000;
	tracker->updateMoveNumber++;
	tracker->updateMoveTime += diff;
}


std::vector<sf::Vector2i>	Grid::getInterestingMovesSortedSaved(
									inter_type plType)
{
	if (plType == INTER_LEFT)
		return (this->interestingMovesLeft);
	return (this->interestingMovesRight);
}


void	Grid::saveInterestingMovesSorted(
				std::vector<sf::Vector2i> *movesLeft,
				std::vector<sf::Vector2i> *movesRight)
{
	*movesLeft = this->interestingMovesLeft;
	*movesRight = this->interestingMovesRight;
}


void	Grid::loadInterestingMovesSorted(
				std::vector<sf::Vector2i> *movesLeft,
				std::vector<sf::Vector2i> *movesRight)
{
	this->interestingMovesLeft = *movesLeft;
	this->interestingMovesRight = *movesRight;
}




bool	Grid::putStone(
				sf::Vector2i *move, int nbMoves, Player *player, Player *opponent)
{
	inter_type	plType, opType;
	int			interId;

	plType = player->getInterType();
	opType = opponent->getInterType();

	if (!this->checkLegalMove(move->x, move->y, nbMoves, plType, opType))
		return (false);

	this->setInterState(move->x, move->y, plType);
	player->addCaptured(this->checkCapture(move, plType, opType));
	this->updateNeighbor(move->x, move->y);

	interId = move->y * GRID_W_INTER + move->x;
	if (plType == INTER_LEFT)
		this->currentBoardState[interId] = 'L';
	else if (plType == INTER_RIGHT)
		this->currentBoardState[interId] = 'R';
	else
		this->currentBoardState[interId] = 'E';

	return (true);
}


void	Grid::removeStone(sf::Vector2i *move, Tracker *tracker)
{
	int				nbNeighbor, x, y, invAxis;
	intersection	*inter, *interTmp;

	// TODO : REMOVE
	std::clock_t	start;
	int				diff;
	start = std::clock();

	// Get the intersection to clean
	inter = this->getIntersection(move->x, move->y);
	if (!inter)
		return ;

	// If the intersection is already empty, stop remove process here
	if (inter->type == INTER_EMPTY)
		return ;

	// Put the intersection to empty
	inter->type = INTER_EMPTY;

	// Update board state
	this->currentBoardState[move->x + move->y * GRID_W_INTER] = 'E';

	// For each axis
	for (int axis = 0; axis < 8; axis++)
	{
		// Get the number of neighbor in this axis
		nbNeighbor = inter->neighbor[axis];

		// If there are no neighbor, go to next axis
		if (nbNeighbor == 0)
			continue;

		// Reset the number of neighbor in this direction
		inter->neighbor[axis] = 0;

		// Reverse axis
		invAxis = (axis + 4) % 8;

		// For each neighbor in the axis, remove one neighbor in the reverse axis
		x = move->x;
		y = move->y;
		for (int i = 0; i < nbNeighbor; i++)
		{
			x += this->dirs[axis].x;
			y += this->dirs[axis].y;
			interTmp = this->getIntersection(x, y);
			if (!interTmp)
				continue;
			interTmp->neighbor[invAxis]--;
		}

	}

	diff = ((double)(std::clock() - start) / CLOCKS_PER_SEC) * 1000000;
	if (!tracker)
		return ;
	tracker->removeStoneTime += diff;
}


void	Grid::resetGridByBoardState(std::string boardState, Tracker *tracker)
{
	char	c;
	int		x, y;

	// TODO : REMOVE
	std::clock_t	start;
	int				diff;
	start = std::clock();

	// For each intersection
	for (int i = 0; i < GRID_NB_INTER; i++)
	{
		c = boardState[i];

		// If inter is empty and c != E, there was a capture
		if (c != 'E' && this->gridState[i].type == INTER_EMPTY)
		{
			// Get x and y
			x = i % 17;
			y = i / 17;

			// Reput the previous type
			if (c == 'L')
			{
				this->gridState[i].type = INTER_LEFT;
				// Update board state
				this->currentBoardState[i] = 'L';
			}
			else
			{
				this->gridState[i].type = INTER_RIGHT;
				// Update board state
				this->currentBoardState[i] = 'R';
			}

			// Update neighbor
			this->updateNeighbor(x, y);
		}
	}

	diff = ((double)(std::clock() - start) / CLOCKS_PER_SEC) * 1000000;
	if (!tracker)
		return ;
	tracker->reverseStoneCaptureNumber++;
	tracker->reverseStoneCaptureTime += diff;
}


void	Grid::saveBbox(sf::Vector2i *bboxUL, sf::Vector2i *bboxDR)
{
	*bboxUL = this->bboxUL;
	*bboxDR = this->bboxDR;
}


void	Grid::loadBbox(sf::Vector2i *bboxUL, sf::Vector2i *bboxDR)
{
	this->bboxUL = *bboxUL;
	this->bboxDR = *bboxDR;
}


void	Grid::saveWinPos(std::vector<sf::Vector2i> *leftWinPos, std::vector<sf::Vector2i> *rightWinPos)
{
	*leftWinPos = this->leftWinPos;
	*rightWinPos = this->rightWinPos;
}


void	Grid::loadWinPos(std::vector<sf::Vector2i> *leftWinPos, std::vector<sf::Vector2i> *rightWinPos)
{
	this->leftWinPos = *leftWinPos;
	this->rightWinPos = *rightWinPos;
}


bool	Grid::checkWinCondition(Player *me, Player *oppenent, sf::Vector2i move)
{
	// Check if the player capture at least 10 opponent's stones
	if (me->getCaptured() >= WIN_CAPTURE)
	{
		me->setWinState(WIN_STATE_CAPTURE);
		this->previewLegal = false;
		return (true);
	}

	// Check if there at least 5 align stones
	if (this->checkWinCaptureCase(me, oppenent, &move, DIR_L, DIR_R))
		return (true);
	if (this->checkWinCaptureCase(me, oppenent, &move, DIR_UL, DIR_DR))
		return (true);
	if (this->checkWinCaptureCase(me, oppenent, &move, DIR_U, DIR_D))
		return (true);
	if (this->checkWinCaptureCase(me, oppenent, &move, DIR_UR, DIR_DL))
		return (true);

	// Check if any valid 5 stones line
	if (this->getInterState(move.x, move.y) == INTER_LEFT)
	{
		for (std::size_t i = 0; i < this->leftWinPos.size(); i++)
		{
			move.x = this->leftWinPos[i].x;
			move.y = this->leftWinPos[i].y;
			if (this->checkWinCaptureCase(me, oppenent, &move, DIR_L, DIR_R))
				return (true);
			if (this->checkWinCaptureCase(me, oppenent, &move, DIR_UL, DIR_DR))
				return (true);
			if (this->checkWinCaptureCase(me, oppenent, &move, DIR_U, DIR_D))
				return (true);
			if (this->checkWinCaptureCase(me, oppenent, &move, DIR_UR, DIR_DL))
				return (true);
		}
	}
	else
	{
		for (std::size_t i = 0; i < this->rightWinPos.size(); i++)
		{
			move.x = this->rightWinPos[i].x;
			move.y = this->rightWinPos[i].y;
			if (this->checkWinCaptureCase(me, oppenent, &move, DIR_L, DIR_R))
				return (true);
			if (this->checkWinCaptureCase(me, oppenent, &move, DIR_UL, DIR_DR))
				return (true);
			if (this->checkWinCaptureCase(me, oppenent, &move, DIR_U, DIR_D))
				return (true);
			if (this->checkWinCaptureCase(me, oppenent, &move, DIR_UR, DIR_DL))
				return (true);
		}
	}

	// Check for draw
	// Opti by find a 'E' into currentBoardState
	if (this->currentBoardState.find('E') == this->currentBoardState.npos)
	{
		me->setWinState(WIN_STATE_NONE);
		oppenent->setWinState(WIN_STATE_NONE);
		this->previewLegal = false;
		return (true);
	}

	return (false);
}


void	Grid::addBoardState(void)
{
	this->boardStates.push_back(this->currentBoardState);
}


void	Grid::disablePreview(void)
{
	this->previewLegal = false;
}


void	Grid::reset(void)
{
	for (int i = 0; i < GRID_NB_INTER; i++)
	{
		this->gridState[i].type = INTER_EMPTY;
		for (int j = 0; j < 8; j++)
			this->gridState[i].neighbor[j] = 0;
	}
	this->boardStates.clear();
	this->leftWinPos.clear();
	this->rightWinPos.clear();
	this->interestingMovesLeft.clear();
	this->interestingMovesRight.clear();
	this->createBoardState();
}


////////////////////////////////////////////////////////////////////////////////
// Private methods
////////////////////////////////////////////////////////////////////////////////

intersection	*Grid::getIntersection(int x, int y)
{
	if (x < 0 || x >= GRID_W_INTER || y < 0 || y >= GRID_W_INTER)
		return (NULL);

	int interId = y * GRID_W_INTER + x;
	return (&this->gridState[interId]);
}


inter_type	Grid::getInterState(int x, int y)
{
	if (x < 0 || x >= GRID_W_INTER || y < 0 || y >= GRID_W_INTER)
		return (INTER_INVALID);

	int interId = y * GRID_W_INTER + x;
	return (this->gridState[interId].type);
}


void	Grid::setInterState(int x, int y, inter_type interType)
{
	if (x < 0 || x >= GRID_W_INTER || y < 0 || y >= GRID_W_INTER)
		return ;

	int interId = y * GRID_W_INTER + x;
	this->gridState[interId].type = interType;

	if (interType == INTER_EMPTY)
		return ;

	// Update bbox
	if (bboxUL.x >= x)
	{
		bboxUL.x = x - 1;
		if (bboxUL.x < 0)
			bboxUL.x = 0;
	}
	if (bboxUL.y >= y)
	{
		bboxUL.y = y - 1;
		if (bboxUL.y < 0)
			bboxUL.y = 0;
	}
	if (bboxDR.x <= x)
	{
		bboxDR.x = x + 1;
		if (bboxDR.x >= GRID_W_INTER)
			bboxDR.x = GRID_W_INTER - 1;
	}
	if (bboxDR.y <= y)
	{
		bboxDR.y = y + 1;
		if (bboxDR.y >= GRID_W_INTER)
			bboxDR.y = GRID_W_INTER - 1;
	}
}


bool	Grid::checkProRule(int x, int y, inter_type interPlayer, int nbMoves)
{
	// The first stone must be placed in the center of the board.
	if (nbMoves == 0 &&
		(x != GRID_W_INTER / 2 ||
		y != GRID_W_INTER / 2))
		return (false);
	// The second stone may be placed anywhere on the board.
	// The third stone must be placed at least three intersections away from the first stone
	else if (nbMoves == 2)
	{
		int	tmp_x, tmp_y;

		for (int i = 0; i < 8; i++)
		{
			tmp_x = x;
			tmp_y = y;
			for (int j = 0; j < 2; j++)
			{
				tmp_x += this->dirs[i].x;
				tmp_y += this->dirs[i].y;
				if (this->getInterState(tmp_x, tmp_y) == interPlayer)
					return (false);
			}
		}
	}

	return (true);
}


bool	Grid::checkDoubleFreeThree(int x, int y, inter_type plState, inter_type opState)
{
	int	tmp_x, tmp_y, nbThreeTree, nbInterMe, nbInterEmpty;
	inter_type	tmpState;

	nbThreeTree = 0;
	for (int i = 0; i < 8; i++)
	{
		tmp_x = x;
		tmp_y = y;

		// Check 3 next intersections in dir
		nbInterMe = 0;
		nbInterEmpty = 0;
		for (int j = 0; j < 3; j++)
		{
			tmp_x += this->dirs[i].x;
			tmp_y += this->dirs[i].y;
			tmpState = this->getInterState(tmp_x, tmp_y);
			if (tmpState == INTER_EMPTY)
				nbInterEmpty++;
			else if (tmpState == plState)
				nbInterMe++;
			else
				break;
		}

		// If there is 2 me and one empty, it can be a free three
		if (nbInterEmpty != 1 || nbInterMe != 2)
			continue;

		// Check opponenent obstruction after possible free three
		tmp_x += this->dirs[i].x;
		tmp_y += this->dirs[i].y;
		tmpState = this->getInterState(tmp_x, tmp_y);
		if (tmpState == opState)
			continue;

		// Check opponenent obstruction before possible free three
		tmp_x = x - this->dirs[i].x;
		tmp_y = y - this->dirs[i].y;
		tmpState = this->getInterState(tmp_x, tmp_y);
		if (tmpState == opState)
			continue;

		nbThreeTree++;
		if (nbThreeTree > 1)
			break;
	}

	return (nbThreeTree >= 2);
}


void	Grid::loopUpdateNeighbor(int x, int y, dir_neighbor dir, inter_type plType)
{
	intersection	*inter;
	int				nb_neighbor, invDir;

	nb_neighbor = 0;
	invDir = (dir + 4) % 8;

	// Go inverse direction to find start of line
	x += this->dirs[invDir].x;
	y += this->dirs[invDir].y;
	while (this->getInterState(x, y) == plType)
	{
		x += this->dirs[invDir].x;
		y += this->dirs[invDir].y;
	}

	// Get first stone of line
	x += this->dirs[dir].x;
	y += this->dirs[dir].y;
	inter = this->getIntersection(x, y);

	// Update each stone of line
	while (inter && inter->type == plType)
	{
		inter->neighbor[invDir] = nb_neighbor;
		x += this->dirs[dir].x;
		y += this->dirs[dir].y;
		inter = this->getIntersection(x, y);
		nb_neighbor++;
	}
}


void	Grid::updateNeighbor(int x, int y)
{
	intersection	*inter;
	inter_type		plType;

	// Get current intersection
	inter = this->getIntersection(x, y);
	if (!inter || (inter->type != INTER_LEFT && inter->type != INTER_RIGHT))
		return ;

	// Compute neighbors for each direction
	plType = this->getInterState(x, y);
	for (int i = 0; i < 8; i++)
		this->loopUpdateNeighbor(x, y, (dir_neighbor)i, plType);
}


int	Grid::checkCapture(sf::Vector2i *move, inter_type plType, inter_type opType)
{
	intersection	*inter;
	intersection	*inters[3];
	int				x, y, nbCapture, invAxis, interId;

	inter = this->getIntersection(move->x, move->y);
	if (!inter)
		return (0);

	plType = inter->type;
	if (plType == INTER_LEFT)
		opType = INTER_RIGHT;
	else if (plType == INTER_RIGHT)
		opType = INTER_LEFT;
	else
		return (0);

	nbCapture = 0;
	for (int axis = 0; axis < 8; axis++)
	{
		x = move->x;
		y = move->y;

		// Get and check 3 next intersections
		x += this->dirs[axis].x;
		y += this->dirs[axis].y;
		inters[0] = this->getIntersection(x, y);
		if (!inters[0] || inters[0]->type != opType)
			continue;

		x += this->dirs[axis].x;
		y += this->dirs[axis].y;
		inters[1] = this->getIntersection(x, y);
		if (!inters[1] || inters[1]->type != opType)
			continue;

		x += this->dirs[axis].x;
		y += this->dirs[axis].y;
		inters[2] = this->getIntersection(x, y);
		if (!inters[2] || inters[2]->type != plType)
			continue;

		// Remove captured stones from board
		inters[0]->type = INTER_EMPTY;
		inters[1]->type = INTER_EMPTY;
		for (int i = 0; i < 8; i++)
		{
			inters[0]->neighbor[i] = 0;
			inters[1]->neighbor[i] = 0;
		}

		// Update stone arround first captured stone
		x = move->x + this->dirs[axis].x;
		y = move->y + this->dirs[axis].y;
		for (int i = 0; i < 8; i++)
			this->updateNeighbor(x + this->dirs[i].x, y + this->dirs[i].y);
		// Update current board state
		this->currentBoardState[y * GRID_W_INTER + x] = 'E';

		// Update stone arround second captured stone
		x += this->dirs[axis].x;
		y += this->dirs[axis].y;
		for (int i = 0; i < 8; i++)
			this->updateNeighbor(x + this->dirs[i].x, y + this->dirs[i].y);
		// Update current board state
		this->currentBoardState[y * GRID_W_INTER + x] = 'E';

		// Update neighbors in the axis
		this->updateNeighbor(x + this->dirs[axis].x * 4,
							y + this->dirs[axis].y * 4);

		// Update neighbors in the inverse axis
		invAxis = (axis + 4) % 8;
		this->updateNeighbor(x + this->dirs[invAxis].x * 4,
							y + this->dirs[invAxis].y * 4);

		nbCapture += 2;
	}

	return (nbCapture);
}


bool	Grid::checkWinCaptureCase(Player *me, Player *oppenent, sf::Vector2i *move, dir_neighbor dir, dir_neighbor opdir)
{
	intersection	*inter, *tmpInter;
	inter_type		opType, tmpType1, tmpType2;
	sf::Vector2i	tmpWinpos;
	int				x, y, tmpX, tmpY, length;
	bool			canBeCapture;

	inter = this->getIntersection(move->x, move->y);
	// Check if me can win
	if (!inter || inter->neighbor[dir] + inter->neighbor[opdir] + 1 < WIN_POINT)
		return (false);

	// Get opponent type
	if (inter->type == INTER_LEFT)
		opType = INTER_RIGHT;
	else
		opType = INTER_LEFT;

	x = move->x;
	y = move->y;

	// Go to the start of the win line
	for (int i = 0; i < inter->neighbor[opdir]; i++)
	{
		x += this->dirs[opdir].x;
		y += this->dirs[opdir].y;
	}

	length = 0;
	// For each stone in win line
	for (int i = 0; i < inter->neighbor[dir] + inter->neighbor[opdir] + 1; i++)
	{
		tmpInter = this->getIntersection(x, y);
		canBeCapture = false;
		tmpWinpos = sf::Vector2i(x, y);

		// Check if the stone can be capture for each axis
		for (int j = 0; j < 4; j++)
		{
			// If axis is the win line axis, skip
			if (j == dir || j == opdir)
				continue;

			// If there isn't 2 stones on axis, stone cannot be capture on this axis
			if (tmpInter->neighbor[j] + tmpInter->neighbor[j + 4] + 1 != 2)
				continue;

			// Get stone before my 2 stones
			tmpX = x;
			tmpY = y;
			for (int k = 0; k < tmpInter->neighbor[j] + 1; k++)
			{
				tmpX += this->dirs[j].x;
				tmpY += this->dirs[j].y;
			}
			tmpType1 = this->getInterState(tmpX, tmpY);

			if (tmpType1 != INTER_EMPTY && tmpType1 != opType)
				continue;

			// Get stone after my 2 stones
			tmpX = x;
			tmpY = y;
			for (int k = 0; k < tmpInter->neighbor[j + 4] + 1; k++)
			{
				tmpX += this->dirs[j + 4].x;
				tmpY += this->dirs[j + 4].y;
			}
			tmpType2 = this->getInterState(tmpX, tmpY);

			// In this case, my stone can be capture
			if (tmpType1 == INTER_EMPTY && tmpType2 == opType ||
				tmpType1 == opType && tmpType2 == INTER_EMPTY)
			{
				if (opType == INTER_RIGHT)
				{
					if (std::find(this->leftWinPos.begin(), this->leftWinPos.end(), tmpWinpos)
							== this->leftWinPos.end())
						this->leftWinPos.push_back(tmpWinpos);
				}
				else
				{
					if (std::find(this->rightWinPos.begin(), this->rightWinPos.end(), tmpWinpos)
							== this->rightWinPos.end())
						this->rightWinPos.push_back(tmpWinpos);
				}
				canBeCapture = true;
				break;
			}
		}

		// If stone can be capture, reset the length
		if (canBeCapture)
			length = 0;
		else
			length++;

		if (length >= WIN_POINT)
		{
			me->setWinState(WIN_STATE_ALIGN);
			this->previewLegal = false;
			return (true);
		}

		x += this->dirs[dir].x;
		y += this->dirs[dir].y;
	}

	if (length >= WIN_POINT)
	{
		me->setWinState(WIN_STATE_ALIGN);
		this->previewLegal = false;
		return (true);
	}

	if (oppenent->getCaptured() >= WIN_CAPTURE - 2)
	{
		oppenent->setWinState(WIN_STATE_CAPTURE);
		oppenent->addCaptured(2);
		this->previewLegal = false;
		return (true);
	}

	return (false);
}


void	Grid::setBoardState(int id)
{
	std::string	boardState = this->boardStates[id];

	for (int i = 0; i < GRID_NB_INTER; i++)
	{
		if (boardState[i] == 'L')
			this->gridState[i].type = INTER_LEFT;
		else if (boardState[i] == 'R')
			this->gridState[i].type = INTER_RIGHT;
		else
			this->gridState[i].type = INTER_EMPTY;
	}
}


void	Grid::createBoardState(void)
{
	this->currentBoardState = "";

	for (int i = 0; i < GRID_NB_INTER; i++)
	{
		if (this->gridState[i].type == INTER_LEFT)
			this->currentBoardState += 'L';
		else if (this->gridState[i].type == INTER_RIGHT)
			this->currentBoardState += 'R';
		else
			this->currentBoardState += 'E';
	}
}


void	Grid::createCurrentMoveText(void)
{
	this->currentMove = std::to_string(this->idBoardState + 1);
	this->currentMove += " / ";
	this->currentMove += std::to_string(this->boardStates.size());
}


void	Grid::insertMoves(
				std::vector<sf::Vector2i> &moves, sf::Vector2i *move,
				bool reverse, Evaluation *evaluator,
				Player *player, Player *opponent)
{
	int			eval, tmpEval, plCapture, opCapture, start, end, mid;
	inter_type	plType, opType;

	plCapture = player->getCaptured();
	opCapture = opponent->getCaptured();
	plType = player->getInterType();
	opType = opponent->getInterType();

	eval = evaluator->evaluationPosition(
						this, plType, opType, plCapture, opCapture,
						move->x, move->y);

	start = 0;
	end = moves.size();
	if (end == 0)
	{
		moves.push_back(*move);
		return ;
	}

	while (start < end)
	{
		mid = (start + end) / 2;

		tmpEval = evaluator->evaluationPosition(
						this, plType, opType, plCapture, opCapture,
						moves[mid].x, moves[mid].y);
		if (!reverse)
		{
			if (tmpEval < eval)
				start = mid + 1;
			else if (tmpEval > eval)
				end = mid - 1;
			else
				break;
		}
		else
		{
			if (tmpEval > eval)
				start = mid + 1;
			else if (tmpEval < eval)
				end = mid - 1;
			else
				break;
		}
	}

	if (start == end)
		mid = start;

	if (mid == moves.size() - 1)
		moves.push_back(*move);
	else
		moves.insert(moves.begin() + mid, *move);
}
