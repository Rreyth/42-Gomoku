#include <utils/TextureManager.hpp>

#include <iostream>
#include <stdexcept>

////////////////////////////////////////////////////////////////////////////////
// Constructors and destructor
////////////////////////////////////////////////////////////////////////////////

TextureManager::TextureManager(void)
{
}


TextureManager::~TextureManager(void)
{

}

////////////////////////////////////////////////////////////////////////////////
// Public methods
////////////////////////////////////////////////////////////////////////////////
void	TextureManager::loadTextures(void)
{
	// Load sprites
	this->loadTexture(SPRITE_GRID, "grid.png");
	this->loadTexture(SPRITE_SQUARE_ROUNDED_BUTTON_OFF, "square_rounded_butt(off).png");
	this->loadTexture(SPRITE_SQUARE_ROUNDED_BUTTON_ON, "square_rounded_butt(on).png");
	this->loadTexture(SPRITE_COMPACT_SQUARE_ROUNDED_BUTTON_OFF, "compact_square_rounded_butt(off).png");
	this->loadTexture(SPRITE_COMPACT_SQUARE_ROUNDED_BUTTON_ON, "compact_square_rounded_butt(on).png");
	this->loadTexture(SPRITE_SQUARE_BUTTON_OFF, "square_button_off.png");
	this->loadTexture(SPRITE_SQUARE_BUTTON_ON, "square_button_on.png");
	this->loadTexture(SPRITE_GEAR_BUTTON_OFF, "gear_button_off.png");
	this->loadTexture(SPRITE_GEAR_BUTTON_ON, "gear_button_on.png");
	this->loadTexture(SPRITE_SMALL_ROUND_BUTTON_OFF, "smallRoundToggleoff.png");
	this->loadTexture(SPRITE_SMALL_ROUND_BUTTON_ON, "smallRoundToggleon.png");
	this->loadTexture(SPRITE_STONE_RED, "stone_red.png");
	this->loadTexture(SPRITE_STONE_BLUE, "stone_blue.png");
	this->loadTexture(SPRITE_COIN_RED, "coin_red.png");
	this->loadTexture(SPRITE_COIN_BLUE, "coin_blue.png");
	this->loadTexture(SPRITE_AMOGUS_RED, "amogus_red.png");
	this->loadTexture(SPRITE_AMOGUS_BLUE, "amogus_blue.png");
	this->loadTexture(SPRITE_STONE_PREVIEW, "stone_preview.png");
	this->loadTexture(SPRITE_ARROW, "down_arrow.png");
	this->loadTexture(LINE_H, "lineH.png");
	this->loadTexture(LINE_V, "lineV.png");
	this->loadTexture(LINE_D, "lineD.png");
	this->loadTexture(LINE_A, "lineA.png");

	// Scale sprites
	// this->scaleSprite(SPRITE_SQUARE_ROUNDED_BUTTON_OFF, 2.0f, 2.0f);
	// this->scaleSprite(SPRITE_SQUARE_ROUNDED_BUTTON_ON, 2.0f, 2.0f);
	this->scaleSprite(SPRITE_SQUARE_BUTTON_OFF, 1.9f, 1.2f);
	this->scaleSprite(SPRITE_SQUARE_BUTTON_ON, 1.9f, 1.2f);
	this->scaleSprite(SPRITE_GEAR_BUTTON_OFF, 0.5f, 0.5f);
	this->scaleSprite(SPRITE_GEAR_BUTTON_ON, 0.5f, 0.5f);
}


void TextureManager::scaleSprite(sprite_name name, float x, float y)
{
	this->sprites[name].setScale(x, y);
}


void	TextureManager::drawTexture(sf::RenderWindow *window, sprite_name name, int x, int y, draw_pos pos)
{
	int w, h;

	sf::Vector2u txt_size = this->textures[name].getSize();
	sf::Vector2f scale = this->sprites[name].getScale();

	w = txt_size.x * scale.x;
	h = txt_size.y * scale.y;

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
		throw std::invalid_argument("");
	this->sprites[name].setTexture(this->textures[name]);
}
