#ifndef BUTTON_HPP
# define BUTTON_HPP

# include <define.hpp>
# include <Mouse.hpp>
# include <TextureManager.hpp>

# include <SFML/Graphics.hpp>
# include <iostream>

class Button
{
public:
	Button(void);
	Button(std::string text, int fontSize, draw_pos textPos, sf::Color textColor,
			int x, int y, float w, float h, sprite_name onSprite, sprite_name offSprite,
			TextureManager *textureManager);
	~Button();

	void	draw(sf::RenderWindow *window, sf::Text *text, TextureManager *textureManager);
	void	tick(Mouse *mouse);
	bool	getPressed(void);
	void	setProperties(std::string text, int fontSize, draw_pos textPos, sf::Color textColor,
							int x, int y, float w, float h, sprite_name onSprite, sprite_name offSprite,
							TextureManager *textureManager);

private:
	int					x, y, w, h, fontSize;
	draw_pos			textPos;
	sprite_name			onSprite, offSprite;
	bool				mouseOver, pressed;
	sf::Color			textColor, colorOff, colorOn;
	std::string			text;
};


#endif //RTB_BUTTON_H
