#include <ui/Button.hpp>
#include <utils/Functions.hpp>

////////////////////////////////////////////////////////////////////////////////
// Constructors and destructor
////////////////////////////////////////////////////////////////////////////////

Button::Button()
{
	this->x = 0;
	this->y = 0;
	this->w = 0;
	this->h = 0;

	this->text = "";
	this->fontSize = 0;
	this->textPos = TOP_LEFT;
	this->textColor = sf::Color::Black;

	this->mouseOver = false;
	this->pressed = false;

	this->onSprite = SPRITE_SQUARE_BUTTON_ON;
	this->offSprite = SPRITE_SQUARE_BUTTON_OFF;
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

	this->mouseOver = false;
	this->pressed = false;

	this->onSprite = onSprite;
	this->offSprite = offSprite;
}


Button::~Button()
{

}

////////////////////////////////////////////////////////////////////////////////
// Getters and setters
////////////////////////////////////////////////////////////////////////////////

bool Button::getPressed( void )
{
	return (this->pressed);
}


void	Button::setText(std::string text)
{
	this->text = text;
}

////////////////////////////////////////////////////////////////////////////////
// Public methods
////////////////////////////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////////////////////////////
// Operator
////////////////////////////////////////////////////////////////////////////////

Button	&Button::operator=(const Button &btn)
{
	this->x = btn.x;
	this->y = btn.y;
	this->w = btn.w;
	this->h = btn.h;

	this->text = btn.text;
	this->fontSize = btn.fontSize;
	this->textPos = btn.textPos;
	this->textColor = btn.textColor;

	this->mouseOver = btn.mouseOver;
	this->pressed = btn.pressed;

	this->onSprite = btn.onSprite;
	this->offSprite = btn.offSprite;

	return (*this);
}
