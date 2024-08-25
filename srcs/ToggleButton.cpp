#include <ToggleButton.hpp>
#include <Functions.hpp>

////////////////////////////////////////////////////////////////////////////////
// Constructors and destructor
////////////////////////////////////////////////////////////////////////////////

ToggleButton::ToggleButton()
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
	this->toggle = false;

	this->onSprite = SPRITE_SQUARE_BUTTON_ON;
	this->offSprite = SPRITE_SQUARE_BUTTON_OFF;
}

ToggleButton::ToggleButton(std::string text, int fontSize, draw_pos textPos, sf::Color textColor,
							int x, int y, float w, float h,
							sprite_name onSprite, sprite_name offSprite, sprite_name toggleSprite)
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
	this->toggle = false;

	this->onSprite = onSprite;
	this->offSprite = offSprite;
	this->toggleSprite = toggleSprite;
}


ToggleButton::~ToggleButton()
{

}

////////////////////////////////////////////////////////////////////////////////
// Getters and setters
////////////////////////////////////////////////////////////////////////////////

bool ToggleButton::getPressed( void )
{
	return (this->pressed);
}


bool	ToggleButton::getToggle(void)
{
	return (this->toggle);
}


void	ToggleButton::setText(std::string text)
{
	this->text = text;
}


void	ToggleButton::setToggle(bool toggle)
{
	this->toggle = toggle;
}

////////////////////////////////////////////////////////////////////////////////
// Public methods
////////////////////////////////////////////////////////////////////////////////

void ToggleButton::draw(sf::RenderWindow *window, sf::Text *text, TextureManager *textureManager)
{
	if (!this->mouseOver)
	{
		if (!this->toggle)
			textureManager->drawTexture(window, this->offSprite, this->x, this->y, TOP_LEFT);
		else
			textureManager->drawTexture(window, this->toggleSprite, this->x, this->y, TOP_LEFT);
	}
	else
		textureManager->drawTexture(window, this->onSprite, this->x, this->y, TOP_LEFT);

	drawText(window, text, this->text,
		this->x + (this->w / 2), this->y + (this->h / 2),
		this->fontSize, this->textColor, this->textPos);
}


void ToggleButton::tick(Mouse *mouse)
{
	this->mouseOver = mouse->inRectangle(this->x, this->y, this->w, this->h);
	this->pressed = this->mouseOver && mouse->isPressed(MBUT_LEFT);
}

////////////////////////////////////////////////////////////////////////////////
// Operator
////////////////////////////////////////////////////////////////////////////////

ToggleButton	&ToggleButton::operator=(const ToggleButton &btn)
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
	this->toggle = btn.toggle;

	this->onSprite = btn.onSprite;
	this->offSprite = btn.offSprite;
	this->toggleSprite = btn.toggleSprite;

	return (*this);
}
