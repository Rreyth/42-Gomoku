#include <End.hpp>
#include <Functions.hpp>

////////////////////////////////////////////////////////////////////////////////
// Constructors and destructor
////////////////////////////////////////////////////////////////////////////////

End::End(void)
{
	this->title = "END";

	this->back = Button("BACK TO MENU", 25, MID_CENTER, sf::Color::White,
						20, WIN_H - 60, 190, 48,
						SPRITE_SQUARE_BUTTON_ON, SPRITE_SQUARE_BUTTON_OFF);
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


void	End::draw(sf::RenderWindow *window, sf::Text *text, TextureManager *textureManager)
{
	drawText(window, text, this->title, WIN_W / 2, 20, 100, sf::Color::White, TOP_CENTER);
	this->back.draw(window, text, textureManager);

	drawText(window, text, "Player 1", WIN_W * 0.25, WIN_H * 0.40, 50, sf::Color::White, MID_CENTER);
	drawText(window, text, "WIN/LOSE", WIN_W * 0.25, WIN_H * 0.50, 50, sf::Color::White, MID_CENTER);
	drawText(window, text, "TIMER", WIN_W * 0.25, WIN_H * 0.60, 50, sf::Color::White, MID_CENTER);

	drawText(window, text, "WINNER", WIN_W * 0.50, WIN_H * 0.47, 50, sf::Color::White, MID_CENTER);
	drawText(window, text, "SPRITE", WIN_W * 0.50, WIN_H * 0.53, 50, sf::Color::White, MID_CENTER);

	drawText(window, text, "Player 2", WIN_W * 0.75, WIN_H * 0.40, 50, sf::Color::White, MID_CENTER);
	drawText(window, text, "WIN/LOSE", WIN_W * 0.75, WIN_H * 0.50, 50, sf::Color::White, MID_CENTER);
	drawText(window, text, "TIMER", WIN_W * 0.75, WIN_H * 0.60, 50, sf::Color::White, MID_CENTER);
}
