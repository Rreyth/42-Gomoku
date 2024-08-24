#ifndef FUNCTIONS_HPP
# define FUNCTIONS_HPP

# include <define.hpp>

# include <SFML/Graphics.hpp>
# include <iostream>

int		keyToInt(sf::Keyboard::Key key);

void	drawText(sf::RenderWindow *window, sf::Text *text, std::string str,
					int x, int y, int fontSize, sf::Color color, draw_pos pos);

#endif
