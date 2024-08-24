#ifndef MOUSE_HPP
# define MOUSE_HPP

# include <SFML/Graphics.hpp>

typedef enum e_mouse_button
{
	MBUT_LEFT,
	MBUT_MIDDLE,
	MBUT_RIGHT
}	mouse_button;

class Mouse
{
public:
	Mouse(void);
	~Mouse();

	int		getX(void) const;
	int		getY(void) const;
	bool	isPressed(mouse_button mbut) const;
	bool	isDown(mouse_button mbut) const;

	void	updatePosition(void);
	void	updateMbutton(mouse_button mbut, bool state);

	bool	inRectangle(int x, int y, int w, int h) const;

private:
	sf::Mouse	SFML_mouse;
	int			x, y;
	bool		pressed[3], down[3];
};

#endif
