#include <Menu.hpp>
#include <Functions.hpp>

////////////////////////////////////////////////////////////////////////////////
// Constructors and destructor
////////////////////////////////////////////////////////////////////////////////

Menu::Menu(void)
{
	this->title = "GOMOCOOL";

	this->play.setProperties("Play", 25, MID_CENTER, sf::Color::Black,
						WIN_W / 2 - 50, WIN_H / 2 - 80, 100, 40,
						sf::Color(100, 100, 100), sf::Color(200, 200, 200), 1, sf::Color::Black);

	this->settings.setProperties("Settings", 25, MID_CENTER, sf::Color::Black,
						WIN_W / 2 - 50, WIN_H / 2 - 20, 100, 40,
						sf::Color(100, 100, 100), sf::Color(200, 200, 200), 1, sf::Color::Black);

	this->quit.setProperties("QUIT", 25, MID_CENTER, sf::Color::Black,
						WIN_W / 2 - 50, WIN_H / 2 + 40, 100, 40,
						sf::Color(100, 100, 100), sf::Color(200, 200, 200), 1, sf::Color::Black);
}


Menu::~Menu()
{
}

////////////////////////////////////////////////////////////////////////////////
// Public methods
////////////////////////////////////////////////////////////////////////////////
void	Menu::tick(display_state *displayState, float delta, Mouse *mouse)
{
	this->play.tick(mouse);
	this->settings.tick(mouse);
	this->quit.tick(mouse);

	if (this->quit.getPressed())
		*displayState = DISPLAY_QUIT;
}


void	Menu::draw(sf::RenderWindow *window, sf::Text *text)
{
	drawText(window, text, this->title, WIN_W / 2, 20, 192, sf::Color(50, 50, 50), TOP_CENTER);
	this->play.draw(window, text);
	this->settings.draw(window, text);
	this->quit.draw(window, text);
}
