#include <screen/End.hpp>
#include <utils/Functions.hpp>

////////////////////////////////////////////////////////////////////////////////
// Constructors and destructor
////////////////////////////////////////////////////////////////////////////////

End::End(void)
{
	this->title = "END";

	this->back = Button("BACK TO MENU", 25, MID_CENTER, sf::Color::White,
						20, WIN_H - 60, 190, 48,
						SPRITE_SQUARE_BUTTON_ON, SPRITE_SQUARE_BUTTON_OFF);

	this->rect.setPosition(0, 0);
	this->rect.setSize(sf::Vector2f(WIN_W, WIN_H));
	this->rect.setFillColor(sf::Color(0, 0, 0, 75));
}

End::~End()
{
}

////////////////////////////////////////////////////////////////////////////////
// Public methods
////////////////////////////////////////////////////////////////////////////////
void	End::tick(display_state *displayState, float delta, Mouse *mouse)
{
	this->back.tick(mouse);

	if (this->back.getPressed())
		*displayState = DISPLAY_MENU;
}


void	End::draw(sf::RenderWindow *window, sf::Text *text, TextureManager *textureManager,
					Player *leftPlayer, Player *rightPlayer, Grid *grid)
{
	grid->draw(window, text, textureManager);

	window->draw(this->rect);

	drawText(window, text, this->title, 20, -20, 200, sf::Color::White, TOP_LEFT);
	this->back.draw(window, text, textureManager);

	drawText(window, text, leftPlayer->getName(), WIN_W * 0.20, WIN_H * 0.40, 50, sf::Color::White, MID_CENTER);
	drawText(window, text, (leftPlayer->isWinner()) ? "WIN" : "LOSE", WIN_W * 0.20, WIN_H * 0.50, 50, sf::Color::White, MID_CENTER);
	drawText(window, text, std::to_string((int)leftPlayer->getTimer()) + " s", WIN_W * 0.20, WIN_H * 0.60, 50, sf::Color::White, MID_CENTER);

	drawText(window, text, rightPlayer->getName(), WIN_W * 0.80, WIN_H * 0.40, 50, sf::Color::White, MID_CENTER);
	drawText(window, text, (rightPlayer->isWinner()) ? "WIN" : "LOSE", WIN_W * 0.80, WIN_H * 0.50, 50, sf::Color::White, MID_CENTER);
	drawText(window, text, std::to_string((int)rightPlayer->getTimer()) + " s", WIN_W * 0.80, WIN_H * 0.60, 50, sf::Color::White, MID_CENTER);
}
