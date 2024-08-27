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

	this->dirs[DIR_L] = sf::Vector2i(-1, 0);
	this->dirs[DIR_UL] = sf::Vector2i(-1, -1);
	this->dirs[DIR_U] = sf::Vector2i(0, -1);
	this->dirs[DIR_UR] = sf::Vector2i(1, -1);
	this->dirs[DIR_R] = sf::Vector2i(1, 0);
	this->dirs[DIR_DR] = sf::Vector2i(1, 1);
	this->dirs[DIR_D] = sf::Vector2i(0, 1);
	this->dirs[DIR_DL] = sf::Vector2i(-1, 1);
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
	int px = (mx - GRID_SQUARE_HALF_SIZE) / GRID_SQUARE_SIZE;
	int py = (my - GRID_SQUARE_HALF_SIZE) / GRID_SQUARE_SIZE;

	if (px == this->previewX && py == this->previewY && !mouse->isPressed(MBUT_LEFT))
		return ;

	this->previewX = px;
	this->previewY = py;
	this->checkIfPreviewLegal(leftPlayer->isPlaying());

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
		if (this->checkWinCondition(leftPlayer, rightPlayer))
		{
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
		if (this->checkWinCondition(rightPlayer, leftPlayer))
		{
			*displayState = DISPLAY_END;
			return ;
		}
		rightPlayer->setPlaying(false);
		leftPlayer->setPlaying(true);
	}
	this->checkIfPreviewLegal(leftPlayer->isPlaying());

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


void	Grid::checkIfPreviewLegal(bool leftPlayer)
{
	this->previewLegal = false;

	if (this->getInterState(this->previewX, this->previewY))
		return ;

	inter_type player = (leftPlayer) ? INTER_LEFT : INTER_RIGHT;
	if (this->checkDoubleFreeThree(player, sf::Vector2i(0, 0))) 
	{
		std::cout << "DOUBLE free three !" << std::endl;
		return;
	}

	this->previewLegal = true;
}

bool	Grid::checkDoubleFreeThree(inter_type interType, sf::Vector2i ignoreDir)
{
	int				x, y;
	bool			empty;
	int				nb_neighbor;
	sf::Vector2i	dir;

	for (int i = 0; i < 8; i++)
	{
		if (this->dirs[i] == ignoreDir)
			continue ;
		empty = false;
		nb_neighbor = 0;
		x = this->previewX;
		y = this->previewY;
		for (int j = 0; j < 4; j++)
		{
			x += this->dirs[i].x;
			y += this->dirs[i].y;
			if (this->getInterState(x, y) == INTER_INVALID)
				break ;
			if (this->getInterState(x, y) != interType && this->getInterState(x, y) != INTER_EMPTY)
				break ;
			if (this->getInterState(x, y) == INTER_EMPTY && empty)
			{
				empty = false;
				break ;
			}
			if (this->getInterState(x, y) == INTER_EMPTY)
			{
				if (nb_neighbor == 1 && j > 1)
					break;
				empty = true;
				continue ;
			}
			empty = false;
			nb_neighbor++;
			if (j > 1 || (j == 1 && nb_neighbor == 2))
			{
				inter_type before = this->getInterState(this->previewX - this->dirs[i].x, this->previewY - this->dirs[i].y);
				inter_type after = this->getInterState(x + this->dirs[i].x, y + this->dirs[i].y);
				if (before == INTER_INVALID && after == INTER_INVALID)
					break ;
				if ((before != interType && before != INTER_EMPTY && before != INTER_INVALID) ||
					(after != interType && after != INTER_EMPTY && after != INTER_INVALID))
					break ;
				if (ignoreDir != sf::Vector2i(0, 0))
					return (true);
				else
					return (this->checkDoubleFreeThree(interType, this->dirs[i]));
			}
		}
	}
	return (false);
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
		inter->neighbor[dir] = nb_neighbor;

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


bool	Grid::checkWinCondition(Player *me, Player *oppenent)
{
	intersection	*inter = this->getIntersection(this->previewX, this->previewY);

	if (!inter)
		return (false);

	// Check if there at least 5 align stones
	if (1 + inter->neighbor[DIR_L] + inter->neighbor[DIR_R] >= WIN_POINT ||
		1 + inter->neighbor[DIR_UL] + inter->neighbor[DIR_DR] >= WIN_POINT ||
		1 + inter->neighbor[DIR_U] + inter->neighbor[DIR_D] >= WIN_POINT ||
		1 + inter->neighbor[DIR_UR] + inter->neighbor[DIR_DL] >= WIN_POINT)
	{
		me->setWinner(true);
		this->previewLegal = false;
		return (true);
	}

	// Check if the player capture at least 10 opponennt's stones
	if (me->getCaptured() >= WIN_CAPTURE)
	{
		me->setWinner(true);
		this->previewLegal = false;
		return (true);
	}

	return (false);
}
