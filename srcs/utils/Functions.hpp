#ifndef FUNCTIONS_HPP
# define FUNCTIONS_HPP

# include <define.hpp>

# include <SFML/Graphics.hpp>
# include <iostream>

void		drawText(sf::RenderWindow *window, sf::Text *text, std::string str,
					int x, int y, int fontSize, sf::Color color, draw_pos pos);

void		initRandom(void);

int			rand_int(int start, int end);

int			min(int a, int b);
int			max(int a, int b);

std::string	format_time(const std::string &str);

#endif
