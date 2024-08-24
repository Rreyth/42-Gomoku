#include <Mouse.hpp>

////////////////////////////////////////////////////////////////////////////////
// Constructors and destructor
////////////////////////////////////////////////////////////////////////////////

Mouse::Mouse(void)
{
	this->x = 0;
	this->y = 0;
	for (int i = 0; i < 3; i++)
	{
		this->pressed[i] = false;
		this->down[i] = false;
	}
}


Mouse::~Mouse()
{
}

////////////////////////////////////////////////////////////////////////////////
// Getters and setters
////////////////////////////////////////////////////////////////////////////////

int	Mouse::getX(void) const
{
	return (this->x);
}


int	Mouse::getY(void) const
{
	return (this->y);
}


bool	Mouse::isPressed(mouse_button mbut) const
{
	return (this->pressed[mbut]);
}


bool	Mouse::isDown(mouse_button mbut) const
{
	return (this->down[mbut]);
}

////////////////////////////////////////////////////////////////////////////////
// Public methods
////////////////////////////////////////////////////////////////////////////////

void	Mouse::updatePosition(void)
{
	this->x = this->SFML_mouse.getPosition().x;
	this->y = this->SFML_mouse.getPosition().y;
	for (int i = 0; i < 3; i++)
		this->pressed[i] = false;
}


void	Mouse::updateMbutton(mouse_button mbut, bool state)
{
	if (state)
	{
		this->pressed[mbut] = true;
		this->down[mbut] = true;
	}
	else
		this->down[mbut] = false;
}


bool	Mouse::inRectangle(int x, int y, int w, int h) const
{
	return (this->x <= x + w && this->x >= x &&
			this->y <= y + h && this->y >= y);
}
