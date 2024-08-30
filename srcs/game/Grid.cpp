#include <game/Grid.hpp>
#include <utils/Functions.hpp>

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


////////////////////////////////////////////////////////////////////////////////
// Public methods
////////////////////////////////////////////////////////////////////////////////

void	Grid::tick(display_state *displayState, Mouse *mouse, Player *leftPlayer, Player *rightPlayer)
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

	if (px == this->previewX && py == this->previewY && !mouse->isPressed(MBUT_LEFT))
		return ;

	this->previewX = px;
	this->previewY = py;
	this->checkIfPreviewLegal(leftPlayer->isPlaying(), rightPlayer->getMoves() + leftPlayer->getMoves());

	if (!mouse->isPressed(MBUT_LEFT) || !this->previewLegal)
		return ;


	if (leftPlayer->isPlaying())
	{
		leftPlayer->addMove();
		this->setInterState(this->previewX, this->previewY, INTER_LEFT);
		this->updateNeighbor(this->previewX, this->previewY);
		leftPlayer->addCaptured(this->checkCapture());
		this->addBoardState();
		if (this->checkWinCondition(leftPlayer, rightPlayer))
		{
			this->goToLastMove();
			*displayState = DISPLAY_END;
			return ;
		}
		leftPlayer->setPlaying(false);
		rightPlayer->setPlaying(true);
	}
	else
	{
		rightPlayer->addMove();
		this->setInterState(this->previewX, this->previewY, INTER_RIGHT);
		this->updateNeighbor(this->previewX, this->previewY);
		rightPlayer->addCaptured(this->checkCapture());
		this->addBoardState();
		if (this->checkWinCondition(rightPlayer, leftPlayer))
		{
			this->goToLastMove();
			*displayState = DISPLAY_END;
			return ;
		}
		rightPlayer->setPlaying(false);
		leftPlayer->setPlaying(true);
	}
	this->checkIfPreviewLegal(leftPlayer->isPlaying(), rightPlayer->getMoves() + leftPlayer->getMoves());

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
}


void	Grid::checkIfPreviewLegal(bool leftPlayer, int nbMoves)
{
	this->previewLegal = false;

	if (this->getInterState(this->previewX, this->previewY))
		return ;

	inter_type plState = (leftPlayer) ? INTER_LEFT : INTER_RIGHT;
	inter_type opState = (leftPlayer) ? INTER_RIGHT : INTER_LEFT;
	if (this->checkDoubleFreeThree(plState, opState))
		return;

	//check rule
	if (this->rule == PRO && nbMoves <= 2)
		if (!this->checkProRule(plState, nbMoves))
			return ;

	this->previewLegal = true;
}

bool	Grid::checkProRule(inter_type interPlayer, int nbMoves)
{
	// The first stone must be placed in the center of the board.
	if (nbMoves == 0 &&
		(this->previewX != GRID_W_INTER / 2 ||
		this->previewY != GRID_W_INTER / 2))
		return (false);
	// The second stone may be placed anywhere on the board.
	// The third stone must be placed at least three intersections away from the first stone
	else if (nbMoves == 2)
	{
		int	x, y;

		for (int i = 0; i < 8; i++)
		{
			x = this->previewX;
			y = this->previewY;
			for (int j = 0; j < 2; j++)
			{
				x += this->dirs[i].x;
				y += this->dirs[i].y;
				if (this->getInterState(x, y) == interPlayer)
					return (false);
			}
		}
	}

	return (true);
}

bool	Grid::checkDoubleFreeThree(inter_type plState, inter_type opState)
{
	int	x, y, nbThreeTree, nbInterMe, nbInterEmpty;
	inter_type	tmpState;

	nbThreeTree = 0;
	for (int i = 0; i < 8; i++)
	{
		x = this->previewX;
		y = this->previewY;

		// Check 3 next intersections in dir
		nbInterMe = 0;
		nbInterEmpty = 0;
		for (int j = 0; j < 3; j++)
		{
			x += this->dirs[i].x;
			y += this->dirs[i].y;
			tmpState = this->getInterState(x, y);
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
		x += this->dirs[i].x;
		y += this->dirs[i].y;
		tmpState = this->getInterState(x, y);
		if (tmpState == opState)
			continue;

		// Check opponenent obstruction before possible free three
		x = this->previewX - this->dirs[i].x;
		y = this->previewY - this->dirs[i].y;
		tmpState = this->getInterState(x, y);
		if (tmpState == opState)
			continue;

		nbThreeTree++;
		if (nbThreeTree > 1)
			break;
	}

	return (nbThreeTree >= 2);
}


int	Grid::loopUpdateNeighbor(int x, int y, dir_neighbor dir, inter_type interType)
{
	intersection	*inter = this->getIntersection(x, y);
	int				nb_neighbor = 0;

	while (inter && inter->type == interType)
	{
		nb_neighbor++;

		x += this->dirs[dir].x;
		y += this->dirs[dir].y;
		inter->neighbor[(dir + 4) % 8] = nb_neighbor;

		inter = this->getIntersection(x, y);
	}

	return (nb_neighbor);
}

void	Grid::updateNeighbor(int x, int y)
{
	intersection	*inter = this->getIntersection(x, y);

	if (inter->type != INTER_LEFT && inter->type != INTER_RIGHT)
		return ;

	for (int i = 0; i < 8; i++)
		inter->neighbor[i] = this->loopUpdateNeighbor(
									x + this->dirs[i].x,
									y + this->dirs[i].y,
									(dir_neighbor)i,
									this->getInterState(x, y));
}


int	Grid::checkCapture(void)
{
	intersection	*inter = this->getIntersection(this->previewX, this->previewY);
	intersection	*inters[3];
	int				x, y, nbCapture;

	nbCapture = 0;
	for (int i = 0; i < 8; i++)
	{
		x = this->previewX;
		y = this->previewY;

		// Get and check 3 next intersections
		x += this->dirs[i].x;
		y += this->dirs[i].y;
		inters[0] = this->getIntersection(x, y);
		if (!inters[0] || inters[0]->type == INTER_EMPTY ||
			inters[0]->type == inter->type)
			continue;

		x += this->dirs[i].x;
		y += this->dirs[i].y;
		inters[1] = this->getIntersection(x, y);
		if (!inters[1] || inters[1]->type == INTER_EMPTY ||
			inters[1]->type == inter->type)
			continue;

		x += this->dirs[i].x;
		y += this->dirs[i].y;
		inters[2] = this->getIntersection(x, y);
		if (!inters[2] || inters[2]->type != inter->type)
			continue;

		// Capture stones
		inters[0]->type = inter->type;
		inters[1]->type = inter->type;

		// Update neighbor
		x = this->previewX;
		y = this->previewY;
		for (int j = 0; j < 3; j++)
		{
			this->updateNeighbor(x, y);
			x += this->dirs[i].x;
			y += this->dirs[i].y;
		}

		nbCapture += 2;
	}

	return (nbCapture);
}


bool	Grid::checkWinCaptureCase(Player *me, Player *oppenent, dir_neighbor dir, dir_neighbor opdir)
{
	intersection	*inter, *tmpInter;
	inter_type		opType, tmpType1, tmpType2;
	sf::Vector2i	tmpWinpos;
	int				x, y, tmpX, tmpY, length;
	bool			canBeCapture;

	inter = this->getIntersection(this->previewX, this->previewY);
	// Check if me can win
	if (!inter || inter->neighbor[dir] + inter->neighbor[opdir] + 1 < WIN_POINT)
		return (false);

	// Get opponent type
	if (inter->type == INTER_LEFT)
		opType = INTER_RIGHT;
	else
		opType = INTER_LEFT;

	x = this->previewX;
	y = this->previewY;

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



bool	Grid::checkWinCondition(Player *me, Player *oppenent)
{
	// Check if there at least 5 align stones
	if (this->checkWinCaptureCase(me, oppenent, DIR_L, DIR_R))
		return (true);
	if (this->checkWinCaptureCase(me, oppenent, DIR_UL, DIR_DR))
		return (true);
	if (this->checkWinCaptureCase(me, oppenent, DIR_U, DIR_D))
		return (true);
	if (this->checkWinCaptureCase(me, oppenent, DIR_UR, DIR_DL))
		return (true);

	// Check if any valid 5 stones line
	if (this->getInterState(this->previewX, this->previewY) == INTER_LEFT)
	{
		for (std::size_t i = 0; i < this->leftWinPos.size(); i++)
		{
			this->previewX = this->leftWinPos[i].x;
			this->previewY = this->leftWinPos[i].y;
			if (this->checkWinCaptureCase(me, oppenent, DIR_L, DIR_R))
				return (true);
			if (this->checkWinCaptureCase(me, oppenent, DIR_UL, DIR_DR))
				return (true);
			if (this->checkWinCaptureCase(me, oppenent, DIR_U, DIR_D))
				return (true);
			if (this->checkWinCaptureCase(me, oppenent, DIR_UR, DIR_DL))
				return (true);
		}
	}
	else
	{
		for (std::size_t i = 0; i < this->rightWinPos.size(); i++)
		{
			this->previewX = this->rightWinPos[i].x;
			this->previewY = this->rightWinPos[i].y;
			if (this->checkWinCaptureCase(me, oppenent, DIR_L, DIR_R))
				return (true);
			if (this->checkWinCaptureCase(me, oppenent, DIR_UL, DIR_DR))
				return (true);
			if (this->checkWinCaptureCase(me, oppenent, DIR_U, DIR_D))
				return (true);
			if (this->checkWinCaptureCase(me, oppenent, DIR_UR, DIR_DL))
				return (true);
		}
	}

	// Check if the player capture at least 10 opponent's stones
	if (me->getCaptured() >= WIN_CAPTURE)
	{
		me->setWinState(WIN_STATE_CAPTURE);
		this->previewLegal = false;
		return (true);
	}

	// Check for draw
	int nbMoves = me->getMoves() + oppenent->getMoves();
	if (nbMoves >= GRID_NB_INTER)
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
	std::string	boardState = "";

	for (int i = 0; i < GRID_NB_INTER; i++)
	{
		if (this->gridState[i].type == INTER_LEFT)
			boardState += 'L';
		else if (this->gridState[i].type == INTER_RIGHT)
			boardState += 'R';
		else
			boardState += 'E';
	}

	this->boardStates.push_back(boardState);
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


void	Grid::createCurrentMoveText(void)
{
	this->currentMove = std::to_string(this->idBoardState + 1);
	this->currentMove += " / ";
	this->currentMove += std::to_string(this->boardStates.size());
}
