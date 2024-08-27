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
	this->previewX = (mx - GRID_SQUARE_HALF_SIZE) / GRID_SQUARE_SIZE;
	this->previewY = (my - GRID_SQUARE_HALF_SIZE) / GRID_SQUARE_SIZE;

	this->checkIfPreviewLegal();

	if (mouse->isPressed(MBUT_RIGHT))
	{
		intersection	*inter = this->getIntersection(this->previewX, this->previewY);

		if (!inter)
			return ;

		std::cout << "\nNeighbors : L  UL U  UR R  DR D  DL" << std::endl;
		std::cout << "Neighbors : "
					<< inter->neighbor[DIR_L] << "  "
					<< inter->neighbor[DIR_UL] << "  "
					<< inter->neighbor[DIR_U] << "  "
					<< inter->neighbor[DIR_UR] << "  "
					<< inter->neighbor[DIR_R] << "  "
					<< inter->neighbor[DIR_DR] << "  "
					<< inter->neighbor[DIR_D] << "  "
					<< inter->neighbor[DIR_DL] << std::endl;
	}

	if (!mouse->isPressed(MBUT_LEFT) || !this->previewLegal)
		return ;


	if (leftPlayer->isPlaying())
	{
		this->setInterState(this->previewX, this->previewY, INTER_LEFT);
		updateNeighbor(this->previewX, this->previewY);
		leftPlayer->addCaptured(this->checkCapture());
		if (this->checkWinCondition(leftPlayer))
		{
			leftPlayer->setWinner(true);
			this->previewLegal = false;
			*displayState = DISPLAY_END;
			return ;
		}
		leftPlayer->setPlaying(false);
		rightPlayer->setPlaying(true);
	}
	else
	{
		this->setInterState(this->previewX, this->previewY, INTER_RIGHT);
		updateNeighbor(this->previewX, this->previewY);
		rightPlayer->addCaptured(this->checkCapture());
		if (this->checkWinCondition(rightPlayer))
		{
			rightPlayer->setWinner(true);
			this->previewLegal = false;
			*displayState = DISPLAY_END;
			return ;
		}
		rightPlayer->setPlaying(false);
		leftPlayer->setPlaying(true);
	}
}


void	Grid::draw(sf::RenderWindow *window, sf::Text *text, TextureManager *textureManager)
{
	textureManager->drawTexture(window, SPRITE_GRID, this->x, this->y, TOP_LEFT);

	std::string	xlabels = ".ABCDEFGHIJKLMNOPQR.";
	std::string	label;
	int	interX, interY, drawX, drawY;

	drawY = this->y - 20;
	//letters top
	for (int i = 1; i <= GRID_W_INTER; i++)
	{
		drawX = this->x + i * GRID_SQUARE_SIZE;
		label = "";
		label += xlabels[i];
		drawText(window, text, label, drawX, drawY, 20, sf::Color::White, MID_CENTER);
	}

	//numbers left
	drawX = this->x - 10;
	for (int i = 1; i <= GRID_W_INTER; i++)
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

int	Grid::loopUpdateNeighbor(int x, int y, dir_neighbor dir, inter_type interType)
{
	intersection	*inter = this->getIntersection(x, y);
	int				nb_neighbor = 0;

	while (inter && inter->type == interType)
	{
		nb_neighbor++;

		switch (dir)
		{
		case DIR_L:
			x -= 1;
			inter->neighbor[DIR_R] = nb_neighbor;
			break;
		case DIR_UL:
			x -= 1;
			y -= 1;
			inter->neighbor[DIR_DR] = nb_neighbor;
			break;
		case DIR_U:
			y -= 1;
			inter->neighbor[DIR_D] = nb_neighbor;
			break;
		case DIR_UR:
			x += 1;
			y -= 1;
			inter->neighbor[DIR_DL] = nb_neighbor;
			break;
		case DIR_R:
			x += 1;
			inter->neighbor[DIR_L] = nb_neighbor;
			break;
		case DIR_DR:
			x += 1;
			y += 1;
			inter->neighbor[DIR_UL] = nb_neighbor;
			break;
		case DIR_D:
			y += 1;
			inter->neighbor[DIR_U] = nb_neighbor;
			break;
		case DIR_DL:
			x -= 1;
			y += 1;
			inter->neighbor[DIR_UR] = nb_neighbor;
			break;
		}

		inter = this->getIntersection(x, y);
	}

	return (nb_neighbor);
}

void	Grid::updateNeighbor(int x, int y)
{
	intersection	*inter = this->getIntersection(x, y);

	inter->neighbor[DIR_L] = this->loopUpdateNeighbor(x - 1, y, DIR_L, this->getInterState(x, y));
	inter->neighbor[DIR_UL] = this->loopUpdateNeighbor(x - 1, y - 1, DIR_UL, this->getInterState(x, y));
	inter->neighbor[DIR_U] = this->loopUpdateNeighbor(x, y - 1, DIR_U, this->getInterState(x, y));
	inter->neighbor[DIR_UR] = this->loopUpdateNeighbor(x + 1, y - 1, DIR_UR, this->getInterState(x, y));
	inter->neighbor[DIR_R] = this->loopUpdateNeighbor(x + 1, y, DIR_R, this->getInterState(x, y));
	inter->neighbor[DIR_DR] = this->loopUpdateNeighbor(x + 1, y + 1, DIR_DR, this->getInterState(x, y));
	inter->neighbor[DIR_D] = this->loopUpdateNeighbor(x, y + 1, DIR_D, this->getInterState(x, y));
	inter->neighbor[DIR_DL] = this->loopUpdateNeighbor(x - 1, y + 1, DIR_DL, this->getInterState(x, y));
}


int	Grid::checkCapture(void)
{
	intersection	*inter = this->getIntersection(this->previewX, this->previewY);
	intersection	*inters[3];
	sf::Vector2i	dirs[8];
	int				x, y, nbCapture;

	dirs[DIR_L] = sf::Vector2i(-1, 0);
	dirs[DIR_UL] = sf::Vector2i(-1, -1);
	dirs[DIR_U] = sf::Vector2i(0, -1);
	dirs[DIR_UR] = sf::Vector2i(1, -1);
	dirs[DIR_R] = sf::Vector2i(1, 0);
	dirs[DIR_DR] = sf::Vector2i(1, 1);
	dirs[DIR_D] = sf::Vector2i(0, 1);
	dirs[DIR_DL] = sf::Vector2i(-1, 1);

	nbCapture = 0;
	for (int i = 0; i < 8; i++)
	{
		x = this->previewX;
		y = this->previewY;

		// Get and check 3 next intersections
		x += dirs[i].x;
		y += dirs[i].y;
		inters[0] = this->getIntersection(x, y);
		if (!inters[0] || inters[0]->type == INTER_EMPTY ||
			inters[0]->type == inter->type)
			continue;

		x += dirs[i].x;
		y += dirs[i].y;
		inters[1] = this->getIntersection(x, y);
		if (!inters[1] || inters[1]->type == INTER_EMPTY ||
			inters[1]->type == inter->type)
			continue;

		x += dirs[i].x;
		y += dirs[i].y;
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
			x += dirs[i].x;
			y += dirs[i].y;
		}

		nbCapture += 2;
	}

	return (nbCapture);
}


bool	Grid::checkWinCondition(Player *player)
{
	intersection	*inter = this->getIntersection(this->previewX, this->previewY);

	if (!inter)
		return (false);

	// Check if there at least 5 align stones
	if (1 + inter->neighbor[DIR_L] + inter->neighbor[DIR_R] >= WIN_POINT ||
		1 + inter->neighbor[DIR_UL] + inter->neighbor[DIR_DR] >= WIN_POINT ||
		1 + inter->neighbor[DIR_U] + inter->neighbor[DIR_D] >= WIN_POINT ||
		1 + inter->neighbor[DIR_UR] + inter->neighbor[DIR_DL] >= WIN_POINT)
		return (true);

	// Check if the player capture at least 10 opponennt's stones
	if (player->getCaptured() >= WIN_CAPTURE)
		return (true);

	return (false);
}
