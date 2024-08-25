#include <Grid.hpp>
#include <Functions.hpp>

////////////////////////////////////////////////////////////////////////////////
// Constructors and destructor
////////////////////////////////////////////////////////////////////////////////

Grid::Grid(void)
{
	this->w = 647;
	this->h = 647;
	this->x = WIN_W / 2 - this->w / 2;
	this->y = WIN_H / 2 - this->h / 2;
}

Grid::~Grid()
{
}

////////////////////////////////////////////////////////////////////////////////
// Public methods
////////////////////////////////////////////////////////////////////////////////
void	Grid::tick(Mouse *mouse, Player *leftPlayer, Player *rightPlayer, std::string *turn)
{
	if (mouse->isPressed(MBUT_LEFT))
	{
		if (!mouse->inRectangle(this->x, this->y, this->w, this->h))
			return;

		//if valid move
		if (*turn == leftPlayer->getName())
			*turn = rightPlayer->getName();
		else
			*turn = leftPlayer->getName();

		std::cout << "Mouse x in grid: " << mouse->getX() - this->x << std::endl;
		std::cout << "Mouse y in grid: " << mouse->getY() - this->y << std::endl;
		std::cout << "Mouse in rectangle: " << mouse->inRectangle(this->x, this->y, this->w, this->h) << std::endl;
		// std::cout << "Mouse in intersection: " << this->isOnIntersection(mouse->getX() - this->x, mouse->getY() - this->y) << std::endl;
		sf::Vector2i intersection = this->closestIntersection(mouse->getX() - this->x, mouse->getY() - this->y);

	}
}


void	Grid::draw(sf::RenderWindow *window, sf::Text *text, TextureManager *textureManager)
{
	textureManager->drawTexture(window, SPRITE_GRID, this->x, this->y, TOP_LEFT);

	//letters top
	drawText(window, text, "A", this->x + 34, this->y - 20, 20, sf::Color::White, MID_CENTER);
	//numbers left
	drawText(window, text, "1", this->x - 20, this->y + 30, 20, sf::Color::White, MID_CENTER);

	//draw stones
	// textureManager->drawTexture(window, SPRITE_STONE_RED, this->x + 34, this->y + 34, MID_CENTER);
	// textureManager->drawTexture(window, SPRITE_STONE_BLUE, this->x + 68, this->y + 68, MID_CENTER);
	// textureManager->drawTexture(window, SPRITE_STONE_BLUE, this->x + 68 + 34, this->y + 68, MID_CENTER);
}

int		Grid::getW(void)
{
	return this->w;
}

int		Grid::getH(void)
{
	return this->h;
}

int		Grid::getX(void)
{
	return this->x;
}

int		Grid::getY(void)
{
	return this->y;
}

////////////////////////////////////////////////////////////////////////////////
// Private methods
////////////////////////////////////////////////////////////////////////////////

sf::Vector2i	Grid::closestIntersection(int x, int y)
{
	std::cout << "x inter: " << x / 19 << std::endl;
	std::cout << "y inter: " << y / 19 << std::endl;

	return sf::Vector2i(x / 19, y / 19);
}