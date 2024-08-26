#include <Grid.hpp>
#include <Functions.hpp>

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

	this->clearGrid();
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

////////////////////////////////////////////////////////////////////////////////
// Public methods
////////////////////////////////////////////////////////////////////////////////

void	Grid::tick(Mouse *mouse, Player *leftPlayer, Player *rightPlayer, std::string *turn)
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
	this->previewX = (mx - GRID_SQUARE_HALF_SIZE) / GRID_SQUARE_SIZE;
	this->previewY = (my - GRID_SQUARE_HALF_SIZE) / GRID_SQUARE_SIZE;

	this->checkIfPreviewLegal();

	if (!mouse->isPressed(MBUT_LEFT) || !this->previewLegal)
		return ;

	// if move is valid
	if (*turn == leftPlayer->getName())
	{
		this->setInterState(this->previewX, this->previewY, INTER_LEFT);
		updateNeighbor(this->previewX, this->previewY);
		this->checkWinCondition(turn);
		*turn = rightPlayer->getName();
	}
	else
	{
		this->setInterState(this->previewX, this->previewY, INTER_RIGHT);
		updateNeighbor(this->previewX, this->previewY);
		this->checkWinCondition(turn);
		*turn = leftPlayer->getName();
	}
}


void	Grid::draw(sf::RenderWindow *window, sf::Text *text, TextureManager *textureManager)
{
	textureManager->drawTexture(window, SPRITE_GRID, this->x, this->y, TOP_LEFT);

	std::string	xlabels = "ABCDEFGHIJKLMNOPQRT";
	std::string	label;
	int	interX, interY, drawX, drawY;

	drawY = this->y - 20;
	//letters top
	for (int i = 0; i < GRID_W_INTER - 1; i++)
	{
		drawX = this->x + (i + 1) * GRID_SQUARE_SIZE;
		label = "";
		label += xlabels[i];
		drawText(window, text, label, drawX, drawY, 20, sf::Color::White, MID_CENTER);
	}

	//numbers left
	drawX = this->x - 10;
	for (int i = 1; i < GRID_W_INTER; i++)
	{
		drawY = this->y + i * GRID_SQUARE_SIZE - 4;
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

		drawX = this->x + (interX + 1) * GRID_SQUARE_SIZE;
		drawY = this->y + (interY + 1) * GRID_SQUARE_SIZE;

		if (this->gridState[i].type == INTER_LEFT)
			textureManager->drawTexture(window, SPRITE_STONE_BLUE, drawX, drawY, MID_CENTER);
		else
			textureManager->drawTexture(window, SPRITE_STONE_RED, drawX, drawY, MID_CENTER);
	}

	//draw stone preview
	if (this->previewLegal)
	{
		drawX = this->x + (this->previewX + 1) * GRID_SQUARE_SIZE;
		drawY = this->y + (this->previewY + 1) * GRID_SQUARE_SIZE;
		textureManager->drawTexture(window, SPRITE_STONE_PREVIEW, drawX, drawY, MID_CENTER);
	}
}


void	Grid::clearGrid(void)
{
	for (int i = 0; i < GRID_NB_INTER; i++)
	{
		this->gridState[i].type = INTER_EMPTY;
		for (int j = 0; j < 8; j++)
			this->gridState[i].neighbor[j] = 0;
	}
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


void	Grid::checkIfPreviewLegal(void)
{
	this->previewLegal = false;

	if (this->getInterState(this->previewX, this->previewY) != INTER_EMPTY)
		return ;

	this->previewLegal = true;
}

int	Grid::recusiveUpdateNeighbor(int x, int y, int suite, dir_neighbor dir, inter_type interType)
{
	intersection	*inter = this->getIntersection(x, y);
	intersection	*interNext;
	int				nextX, nextY;

	// Out of grid
	if (inter == NULL)
		return (0);

	if (dir == DIR_L)
	{
		nextX = x - 1;
		nextY = y;
		interNext = this->getIntersection(nextX, nextY);
		inter->neighbor[DIR_R] = suite;
		if (interNext == NULL || interType != interNext->type)
			inter->neighbor[DIR_L] = 0;
		else
			inter->neighbor[DIR_L] = this->recusiveUpdateNeighbor(nextX, nextY, suite + 1, dir, interType);
		return (inter->neighbor[DIR_L] + 1);
	}

	else if (dir == DIR_UL)
	{
		nextX = x - 1;
		nextY = y - 1;
		interNext = this->getIntersection(nextX, nextY);
		inter->neighbor[DIR_DR] = suite;
		if (interNext == NULL || interType != interNext->type)
			inter->neighbor[DIR_UL] = 0;
		else
			inter->neighbor[DIR_UL] = this->recusiveUpdateNeighbor(nextX, nextY, suite + 1, dir, interType);
		return (inter->neighbor[DIR_UL] + 1);
	}

	else if (dir == DIR_U)
	{
		nextX = x;
		nextY = y - 1;
		interNext = this->getIntersection(nextX, nextY);
		inter->neighbor[DIR_D] = suite;
		if (interNext == NULL || interType != interNext->type)
			inter->neighbor[DIR_U] = 0;
		else
			inter->neighbor[DIR_U] = this->recusiveUpdateNeighbor(nextX, nextY, suite + 1, dir, interType);
		return (inter->neighbor[DIR_U] + 1);
	}

	else if (dir == DIR_UR)
	{
		nextX = x + 1;
		nextY = y - 1;
		interNext = this->getIntersection(nextX, nextY);
		inter->neighbor[DIR_DL] = suite;
		if (interNext == NULL || interType != interNext->type)
			inter->neighbor[DIR_UR] = 0;
		else
			inter->neighbor[DIR_UR] = this->recusiveUpdateNeighbor(nextX, nextY, suite + 1, dir, interType);
		return (inter->neighbor[DIR_UR] + 1);
	}

	else if (dir == DIR_R)
	{
		nextX = x + 1;
		nextY = y;
		interNext = this->getIntersection(nextX, nextY);
		inter->neighbor[DIR_L] = suite;
		if (interNext == NULL || interType != interNext->type)
			inter->neighbor[DIR_R] = 0;
		else
			inter->neighbor[DIR_R] = this->recusiveUpdateNeighbor(nextX, nextY, suite + 1, dir, interType);
		return (inter->neighbor[DIR_R] + 1);
	}

	else if (dir == DIR_DR)
	{
		nextX = x + 1;
		nextY = y + 1;
		interNext = this->getIntersection(nextX, nextY);
		inter->neighbor[DIR_UL] = suite;
		if (interNext == NULL || interType != interNext->type)
			inter->neighbor[DIR_DR] = 0;
		else
			inter->neighbor[DIR_DR] = this->recusiveUpdateNeighbor(nextX, nextY, suite + 1, dir, interType);
		return (inter->neighbor[DIR_DR] + 1);
	}

	else if (dir == DIR_D)
	{
		nextX = x;
		nextY = y + 1;
		interNext = this->getIntersection(nextX, nextY);
		inter->neighbor[DIR_U] = suite;
		if (interNext == NULL || interType != interNext->type)
			inter->neighbor[DIR_D] = 0;
		else
			inter->neighbor[DIR_D] = this->recusiveUpdateNeighbor(nextX, nextY, suite + 1, dir, interType);
		return (inter->neighbor[DIR_D] + 1);
	}

	else if (dir == DIR_DL)
	{
		nextX = x;
		nextY = y + 1;
		interNext = this->getIntersection(nextX, nextY);
		inter->neighbor[DIR_UR] = suite;
		if (interNext == NULL || interType != interNext->type)
			inter->neighbor[DIR_DL] = 0;
		else
			inter->neighbor[DIR_DL] = this->recusiveUpdateNeighbor(nextX, nextY, suite + 1, dir, interType);
		return (inter->neighbor[DIR_DL] + 1);
	}

	return (0);
}

void	Grid::updateNeighbor(int x, int y)
{
	this->recusiveUpdateNeighbor(x, y, 0, DIR_L, this->getInterState(x, y));
	this->recusiveUpdateNeighbor(x, y, 0, DIR_UL, this->getInterState(x, y));
	this->recusiveUpdateNeighbor(x, y, 0, DIR_U, this->getInterState(x, y));
	this->recusiveUpdateNeighbor(x, y, 0, DIR_UR, this->getInterState(x, y));
	this->recusiveUpdateNeighbor(x, y, 0, DIR_R, this->getInterState(x, y));
	this->recusiveUpdateNeighbor(x, y, 0, DIR_DR, this->getInterState(x, y));
	this->recusiveUpdateNeighbor(x, y, 0, DIR_D, this->getInterState(x, y));
	this->recusiveUpdateNeighbor(x, y, 0, DIR_DL, this->getInterState(x, y));
}


void	Grid::checkCapture(void)
{
	// TODO: Check capture
}


void	Grid::checkWinCondition(std::string *turn)
{
	// TODO: 5+ align stone
	intersection	*inter = this->getIntersection(this->previewX, this->previewY);

	if (!inter)
		return ;

	// TODO: NEIGHBORS NOT WORKING !!!
	std::cout << "Neighbors : "
				<< inter->neighbor[DIR_L] << " "
				<< inter->neighbor[DIR_UL] << " "
				<< inter->neighbor[DIR_U] << " "
				<< inter->neighbor[DIR_UR] << " "
				<< inter->neighbor[DIR_R] << " "
				<< inter->neighbor[DIR_DR] << " "
				<< inter->neighbor[DIR_D] << " "
				<< inter->neighbor[DIR_DL] << std::endl;

	if (inter->neighbor[DIR_L] + inter->neighbor[DIR_R] >= 5)
		std::cout << *turn << " win the game !" << std::endl;

	else if (inter->neighbor[DIR_UL] + inter->neighbor[DIR_DR] >= 5)
		std::cout << *turn << " win the game !" << std::endl;

	else if (inter->neighbor[DIR_U] + inter->neighbor[DIR_D] >= 5)
		std::cout << *turn << " win the game !" << std::endl;

	else if (inter->neighbor[DIR_UR] + inter->neighbor[DIR_DL] >= 5)
		std::cout << *turn << " win the game !" << std::endl;
}
