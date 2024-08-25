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
void	Grid::tick(Mouse *mouse)
{
	if (mouse->isPressed(MBUT_LEFT))
	{
		if (!mouse->inRectangle(this->x, this->y, this->w, this->h))
			return;
		std::cout << "Mouse x in grid: " << mouse->getX() - this->x << std::endl;
		std::cout << "Mouse y in grid: " << mouse->getY() - this->y << std::endl;
		std::cout << "Mouse in rectangle: " << mouse->inRectangle(this->x, this->y, this->w, this->h) << std::endl;
		// std::cout << "Mouse in intersection: " << this->isOnIntersection(mouse->getX() - this->x, mouse->getY() - this->y) << std::endl;
		sf::Vector2i intersection = this->closestIntersection(mouse->getX() - this->x, mouse->getY() - this->y);

	}
}


void	Grid::draw(sf::RenderWindow *window, sf::Text *text, TextureManager *textureManager)
{
	//draw board
	textureManager->drawTexture(window, SPRITE_GRID, WIN_W / 2, WIN_H / 2, MID_CENTER);

	//draw stones


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