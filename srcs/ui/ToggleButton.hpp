#ifndef TOGGLEBUTTON_HPP
# define TOGGLEBUTTON_HPP

# include <define.hpp>
# include <utils/Mouse.hpp>
# include <utils/TextureManager.hpp>

# include <SFML/Graphics.hpp>
# include <iostream>

class ToggleButton
{
public:
	ToggleButton(void);
	ToggleButton(std::string text, int fontSize, draw_pos textPos, sf::Color textColor,
				int x, int y, float w, float h,
				sprite_name onSprite, sprite_name offSprite, sprite_name toggleSprite);
	~ToggleButton();

	bool	getPressed(void);
	bool	getToggle(void);
	void	setText(std::string text);
	void	setToggle(bool toggle);

	void	tick(Mouse *mouse);
	void	draw(sf::RenderWindow *window, sf::Text *text, TextureManager *textureManager);

	ToggleButton	&operator=(const ToggleButton &btn);

private:
	int			x, y, w, h, fontSize;
	draw_pos	textPos;
	sprite_name	onSprite, offSprite, toggleSprite;
	bool		mouseOver, pressed, toggle;
	sf::Color	textColor, colorOff, colorOn;
	std::string	text;
};


#endif
