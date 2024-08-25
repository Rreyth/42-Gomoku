#ifndef SELECT_HPP
# define SELECT_HPP

# include <define.hpp>
# include <Mouse.hpp>
# include <TextureManager.hpp>
# include <Button.hpp>
# include <ToggleButton.hpp>

# include <SFML/Graphics.hpp>
# include <iostream>
# include <vector>

class Select
{
public:
	Select(void);
	Select(std::vector<std::string> &texts, int fontSize, draw_pos textPos, sf::Color textColor,
			int x, int y, float w, float h,
			sprite_name onSprite, sprite_name offSprite, sprite_name toggleSprite);
	~Select();

	int		getSelected(void);
	void	setSelected(int selected);

	void	tick(Mouse *mouse);
	void	draw(sf::RenderWindow *window, sf::Text *text, TextureManager *textureManager);

	Select	&operator=(const Select &btn);

private:
	int							x, y, w, h, fontSize, selected, mouseOverSelected;
	draw_pos					textPos;
	sprite_name					onSprite, offSprite, toggleSprite;
	bool						mouseOver, toggle;
	sf::Color					textColor, colorOff, colorOn;
	std::vector<std::string>	texts;
};


#endif
