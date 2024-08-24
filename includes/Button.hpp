#ifndef BUTTON_HPP
# define BUTTON_HPP

# include <define.hpp>
# include <Mouse.hpp>

# include <SFML/Graphics.hpp>
# include <iostream>

class Button
{
public:
	Button(void);
	Button(std::string text, int fontSize, draw_pos textPos, sf::Color textColor,
			int x, int y, int w, int h, sf::Color colorOff,
			sf::Color colorOn, int borderSize, sf::Color borderColor);
	~Button();

	void draw(sf::RenderWindow *window, sf::Text *text);
	void tick(Mouse *mouse);
	bool getPressed(void);

private:
	int					x, y, w, h, fontSize;
	draw_pos			textPos;
	bool				mouseOver, pressed;
	sf::Color			textColor, colorOff, colorOn;
	std::string			text;
	sf::RectangleShape	shape;
};


#endif //RTB_BUTTON_H
