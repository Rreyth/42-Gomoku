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

int		rand_int(int start, int end)
{
	srand(time(NULL));
	return (rand() % end + start);
}
