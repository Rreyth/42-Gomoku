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

	// Move control buttons
	this->firstMove = Button("|<", 25, MID_CENTER, sf::Color::White,
						WIN_W * 0.42, WIN_H * 0.92, 24, 24,
						SPRITE_SMALL_ROUND_BUTTON_ON, SPRITE_SMALL_ROUND_BUTTON_OFF);
	this->previousMove = Button("<", 25, MID_CENTER, sf::Color::White,
						WIN_W * 0.44, WIN_H * 0.92, 20, 20,
						SPRITE_SMALL_ROUND_BUTTON_ON, SPRITE_SMALL_ROUND_BUTTON_OFF);
	this->nextMove = Button(">", 25, MID_CENTER, sf::Color::White,
						WIN_W * 0.545, WIN_H * 0.92, 20, 20,
						SPRITE_SMALL_ROUND_BUTTON_ON, SPRITE_SMALL_ROUND_BUTTON_OFF);
	this->lastMove = Button(">|", 25, MID_CENTER, sf::Color::White,
						WIN_W * 0.565, WIN_H * 0.92, 24, 24,
						SPRITE_SMALL_ROUND_BUTTON_ON, SPRITE_SMALL_ROUND_BUTTON_OFF);

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
void	End::tick(display_state *displayState, float delta, Mouse *mouse, Grid *grid)
{
	this->back.tick(mouse);
	this->firstMove.tick(mouse);
	this->previousMove.tick(mouse);
	this->nextMove.tick(mouse);
	this->lastMove.tick(mouse);

	if (this->back.getPressed())
		*displayState = DISPLAY_MENU;
	else if (this->firstMove.getPressed())
		grid->goToFirstMove();
	else if (this->previousMove.getPressed())
		grid->goToPreviousMove();
	else if (this->nextMove.getPressed())
		grid->goToNextMove();
	else if (this->lastMove.getPressed())
		grid->goToLastMove();
}


void	End::draw(sf::RenderWindow *window, sf::Text *text, TextureManager *textureManager,
					Player *leftPlayer, Player *rightPlayer, Grid *grid)
{
	// Display grid
	grid->draw(window, text, textureManager);

	// Darker screen
	window->draw(this->rect);

	// Titles
	drawText(window, text, this->title, 20, -40, 200, sf::Color::White, TOP_LEFT);

	std::string leftWin, rightWin;
	if (leftPlayer->getWinState() == WIN_STATE_NONE && rightPlayer->getWinState() == WIN_STATE_NONE)
	{
		drawText(window, text, "DRAW", WIN_W * 0.92, 40, 70, sf::Color::White, MID_CENTER);
		leftWin = "DRAW";
		rightWin = "DRAW";
	}
	else
	{
		drawText(window, text, "WIN BY", WIN_W * 0.92, 40, 70, sf::Color::White, MID_CENTER);

		std::string winStates[] = {"NONE", "ALIGN", "CAPTURE", "TIMEOUT"};

		if (leftPlayer->getWinState() != WIN_STATE_NONE)
			drawText(window, text, winStates[leftPlayer->getWinState()], WIN_W * 0.92, 110, 70, sf::Color::White, MID_CENTER);
		else
			drawText(window, text, winStates[rightPlayer->getWinState()], WIN_W * 0.92, 110, 70, sf::Color::White, MID_CENTER);

		leftWin = (leftPlayer->getWinState() != WIN_STATE_NONE) ? "WIN" : "LOSE";
		rightWin = (rightPlayer->getWinState() != WIN_STATE_NONE) ? "WIN" : "LOSE";
	}

	this->back.draw(window, text, textureManager);

	// Left player info
	drawText(window, text, leftPlayer->getName(), WIN_W * 0.15, WIN_H * 0.33, 50, sf::Color::White, MID_CENTER);
	drawText(window, text, leftWin, WIN_W * 0.15, WIN_H * 0.43, 50, sf::Color::White, MID_CENTER);
	drawText(window, text, "TIMER", WIN_W * 0.10, WIN_H * 0.53, 50, sf::Color::White, MID_CENTER);
	drawText(window, text, std::to_string((int)leftPlayer->getTimer()) + " s", WIN_W * 0.20, WIN_H * 0.53, 50, sf::Color::White, MID_CENTER);
	drawText(window, text, "MOVES", WIN_W * 0.10, WIN_H * 0.63, 50, sf::Color::White, MID_CENTER);
	drawText(window, text, std::to_string(leftPlayer->getMoves()), WIN_W * 0.20, WIN_H * 0.63, 50, sf::Color::White, MID_CENTER);
	drawText(window, text, "CAPTURED", WIN_W * 0.10, WIN_H * 0.73, 50, sf::Color::White, MID_CENTER);
	drawText(window, text, std::to_string(leftPlayer->getCaptured()), WIN_W * 0.20, WIN_H * 0.73, 50, sf::Color::White, MID_CENTER);

	// Right player info
	drawText(window, text, rightPlayer->getName(), WIN_W * 0.85, WIN_H * 0.33, 50, sf::Color::White, MID_CENTER);
	drawText(window, text, rightWin, WIN_W * 0.85, WIN_H * 0.43, 50, sf::Color::White, MID_CENTER);
	drawText(window, text, "TIMER", WIN_W * 0.80, WIN_H * 0.53, 50, sf::Color::White, MID_CENTER);
	drawText(window, text, std::to_string((int)rightPlayer->getTimer()) + " s", WIN_W * 0.90, WIN_H * 0.53, 50, sf::Color::White, MID_CENTER);
	drawText(window, text, "MOVES", WIN_W * 0.80, WIN_H * 0.63, 50, sf::Color::White, MID_CENTER);
	drawText(window, text, std::to_string(rightPlayer->getMoves()), WIN_W * 0.90, WIN_H * 0.63, 50, sf::Color::White, MID_CENTER);
	drawText(window, text, "CAPTURED", WIN_W * 0.80, WIN_H * 0.73, 50, sf::Color::White, MID_CENTER);
	drawText(window, text, std::to_string(rightPlayer->getCaptured()), WIN_W * 0.90, WIN_H * 0.73, 50, sf::Color::White, MID_CENTER);

	// Moves control
	drawText(window, text, "MOVES", WIN_W / 2, WIN_H * 0.875, 35, sf::Color::White, MID_CENTER);
	this->firstMove.draw(window, text, textureManager);
	this->previousMove.draw(window, text, textureManager);
	drawText(window, text, grid->getCurrentMove(), WIN_W / 2, WIN_H * 0.93, 35, sf::Color::White, MID_CENTER);
	this->nextMove.draw(window, text, textureManager);
	this->lastMove.draw(window, text, textureManager);
}
