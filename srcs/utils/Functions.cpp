#include <utils/Functions.hpp>

#include <cstdlib>
#include <cmath>
#include <random>
#include <ctime>

void	drawText(sf::RenderWindow *window, sf::Text *text, std::string str,
					int x, int y, int fontSize, sf::Color color, draw_pos pos)
{
	float w, h;

	text->setString(str);
	text->setCharacterSize(fontSize);
	w = text->getLocalBounds().width;
	h = text->getLocalBounds().height + fontSize / 2;
	if (pos == TOP_LEFT)
		text->setPosition(x, y);
	else if (pos == TOP_CENTER)
		text->setPosition(x - w / 2, y);
	else if (pos == TOP_RIGHT)
		text->setPosition(x - w, y);
	else if (pos == MID_LEFT)
		text->setPosition(x, y - h / 2);
	else if (pos == MID_CENTER)
		text->setPosition(x - w / 2, y - h / 2);
	else if (pos == MID_RIGHT)
		text->setPosition(x - w, y - h / 2);
	else if (pos == BOT_LEFT)
		text->setPosition(x, y - h);
	else if (pos == BOT_CENTER)
		text->setPosition(x - w / 2, y - h);
	else // BOT_RIGHT
		text->setPosition(x - w, y - h);
	text->setFillColor(color);
	window->draw(*text);
}


void	initRandom(void)
{
	srand(time(NULL));
}

int		rand_int(int start, int end)
{
	return (rand() % end + start);
}


int	min(int a, int b)
{
	if (a > b)
		return (b);
	return (a);
}


int	max(int a, int b)
{
	if (a < b)
		return (b);
	return (a);
}


std::string	format_time(const std::string &str)
{
	if (str.length() <= 3)
		return (str);
	
	std::string	formated_res = str;
	int			size = str.length();

	for (int i = size - 3; i > 0; i-= 3)
		formated_res.insert(i, "'");

	return (formated_res);
}
