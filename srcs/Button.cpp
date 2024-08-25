#include <Button.hpp>
#include <Functions.hpp>

//=============================================================================
// Contructors and destructor
//=============================================================================
Button::Button()
{

}

Button::Button(std::string text, int fontSize, draw_pos textPos, sf::Color textColor,
				int x, int y, float w, float h, sprite_name onSprite, sprite_name offSprite)
{
	this->x = x;
	this->y = y;
	this->w = w;
	this->h = h;

	this->text = text;
	this->fontSize = fontSize;
	this->textPos = textPos;
	this->textColor = textColor;

	this->onSprite = onSprite;
	this->offSprite = offSprite;
}


Button::~Button()
{

}

//=============================================================================
//Publics methods
//=============================================================================

void Button::draw(sf::RenderWindow *window, sf::Text *text, TextureManager *textureManager)
{
	if (!this->mouseOver)
		textureManager->drawTexture(window, this->offSprite, this->x, this->y, TOP_LEFT);
	else
		textureManager->drawTexture(window, this->onSprite, this->x, this->y, TOP_LEFT);

	drawText(window, text, this->text,
		this->x + (this->w / 2), this->y + (this->h / 2),
		this->fontSize, this->textColor, this->textPos);
}


void Button::tick(Mouse *mouse)
{
	this->mouseOver = mouse->inRectangle(this->x, this->y, this->w, this->h);
	this->pressed = this->mouseOver && mouse->isPressed(MBUT_LEFT);
}


bool Button::getPressed( void )
{
	return (this->pressed);
}


void	Button::setProperties(std::string text, int fontSize, draw_pos textPos, sf::Color textColor,
						int x, int y, float w, float h, sprite_name onSprite, sprite_name offSprite)
{
	this->x = x;
	this->y = y;
	this->w = w;
	this->h = h;

	this->text = text;
	this->fontSize = fontSize;
	this->textPos = textPos;
	this->textColor = textColor;

	this->onSprite = onSprite;
	this->offSprite = offSprite;
}
