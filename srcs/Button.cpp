#include <Button.hpp>
#include <Functions.hpp>

//=============================================================================
// Contructors and destructor
//=============================================================================
Button::Button()
{

}

Button::Button(std::string text, int fontSize, draw_pos textPos, sf::Color textColor,
				int x, int y, int w, int h, sf::Color colorOff,
				sf::Color colorOn, int borderSize, sf::Color borderColor)
{
	this->x = x;
	this->y = y;
	this->w = w;
	this->h = h;

	this->text = text;
	this->fontSize = fontSize;
	this->textPos = textPos;
	this->textColor = textColor;

	this->shape = sf::RectangleShape();
	this->shape.setPosition(this->x, this->y);
	this->shape.setSize(sf::Vector2f(this->w, this->h));
	this->shape.setOutlineThickness(borderSize);
	this->shape.setOutlineColor(borderColor);

	this->colorOff = colorOff;
	this->colorOn = colorOn;
}


Button::~Button()
{

}


//=============================================================================
//Publics methods
//=============================================================================
void Button::draw(sf::RenderWindow *window, sf::Text *text)
{
	if (!this->mouseOver)
		this->shape.setFillColor(this->colorOff);
	else
		this->shape.setFillColor(this->colorOn);
	window->draw(this->shape);

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
