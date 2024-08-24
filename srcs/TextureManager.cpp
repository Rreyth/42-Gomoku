#include <TextureManager.hpp>
#include <LoadException.hpp>

#include <iostream>

////////////////////////////////////////////////////////////////////////////////
// Constructors and destructor
////////////////////////////////////////////////////////////////////////////////

TextureManager::TextureManager(void)
{
	this->loadTexture(SPRITE_GRID, "grid.png");
}


TextureManager::~TextureManager(void)
{

}

////////////////////////////////////////////////////////////////////////////////
// Public methods
////////////////////////////////////////////////////////////////////////////////

void	TextureManager::drawTexture(sf::RenderWindow *window, sprite_name name, int x, int y, draw_pos pos)
{
	sf::Vector2u size;
	int w, h;

	size = this->textures[name].getSize();
	w = size.x;
	h = size.y;

	if (pos == TOP_LEFT)
		this->sprites[name].setPosition(x, y);
	else if (pos == TOP_CENTER)
		this->sprites[name].setPosition(x - w / 2, y);
	else if (pos == TOP_RIGHT)
		this->sprites[name].setPosition(x - w, y);
	else if (pos == MID_LEFT)
		this->sprites[name].setPosition(x, y - h / 2);
	else if (pos == MID_CENTER)
		this->sprites[name].setPosition(x - w / 2, y - h / 2);
	else if (pos == MID_RIGHT)
		this->sprites[name].setPosition(x - w, y - h / 2);
	else if (pos == BOT_LEFT)
		this->sprites[name].setPosition(x, y - h);
	else if (pos == BOT_CENTER)
		this->sprites[name].setPosition(x - w / 2, y - h);
	else // BOT_RIGHT
		this->sprites[name].setPosition(x - w, y - h);
	window->draw(this->sprites[name]);
}

////////////////////////////////////////////////////////////////////////////////
// Private methods
////////////////////////////////////////////////////////////////////////////////

void	TextureManager::loadTexture(sprite_name name, std::string filename)
{
	if (!this->textures[name].loadFromFile("data/sprites/" + filename))
	{
		throw LoadException(filename + " not found");
	}
	this->sprites->setTexture(this->textures[name]);
}
