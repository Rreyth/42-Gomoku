#include <screen/End.hpp>
#include <utils/Functions.hpp>
#include <screen/Game.hpp>

static void	drawLeft(sf::RenderWindow *window, sf::Text *text, Player *leftPlayer, std::string leftWin);
static void drawRight(sf::RenderWindow *window, sf::Text *text, Player *rightPlayer, std::string rightWin);

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

	this->replay = Button("REPLAY", 25, MID_CENTER, sf::Color::White,
						WIN_W * 0.875, WIN_H - 60, 190, 48,
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
void	End::tick(display_state *displayState, float delta, Mouse *mouse, Game *game)
{
	this->back.tick(mouse);
	this->firstMove.tick(mouse);
	this->previousMove.tick(mouse);
	this->nextMove.tick(mouse);
	this->lastMove.tick(mouse);
	this->replay.tick(mouse);

	if (this->back.getPressed())
		*displayState = DISPLAY_MENU;
	else if (this->firstMove.getPressed())
		game->getGrid()->goToHistoryStart();
	else if (this->previousMove.getPressed())
		game->getGrid()->goToHistoryPrevious();
	else if (this->nextMove.getPressed())
		game->getGrid()->goToHistoryNext();
	else if (this->lastMove.getPressed())
		game->getGrid()->goToHistoryEnd();
	else if (this->replay.getPressed())
		game->replay(displayState);
}


void	End::draw(sf::RenderWindow *window, sf::Text *text, TextureManager *textureManager,
					Player *leftPlayer, Player *rightPlayer, Grid *grid)
{
	std::string	timer;

	// Display grid
	grid->draw(window, text, textureManager);

	// Display victory line if win by align
	if (grid->isHistoryEnd()
				&& (leftPlayer->getWinState() == WIN_STATE_ALIGN
				|| rightPlayer->getWinState() == WIN_STATE_ALIGN))
	{
		WinLine		*winline = grid->getWinLine();
		sprite_name line_sprite;
		draw_pos	pos;
		int			drawX, drawY;

		drawX = grid->getX() + winline->x * GRID_SQUARE_SIZE + 23;
		drawY = grid->getY() + winline->y * GRID_SQUARE_SIZE + 10;

		if (winline->axis == BBH)
		{
			line_sprite = LINE_H;
			pos = MID_RIGHT;
		}
		else if (winline->axis == BBV)
		{
			drawY = grid->getY() + winline->y * GRID_SQUARE_SIZE + 22;
			line_sprite = LINE_V;
			pos = BOT_RIGHT;
		}
		else if (winline->axis == BBD)
		{
			drawY = grid->getY() + winline->y * GRID_SQUARE_SIZE - 5;
			line_sprite = LINE_D;
			pos = TOP_RIGHT;
		}
		else
		{
			drawY = grid->getY() + winline->y * GRID_SQUARE_SIZE + 22;
			line_sprite = LINE_A;
			pos = BOT_RIGHT;
		}

		textureManager->drawTexture(window, line_sprite, drawX, drawY, pos);
	}

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
		drawText(window, text, "WIN BY", WIN_W * 0.875, 40, 70, sf::Color::White, MID_CENTER);

		std::string winStates[] = {"NONE", "ALIGN", "CAPTURE", "AUTO CAPTURE", "TIMEOUT"};

		if (leftPlayer->getWinState() != WIN_STATE_NONE)
			drawText(window, text, winStates[leftPlayer->getWinState()], WIN_W * 0.875, 110, 70, sf::Color::White, MID_CENTER);
		else
			drawText(window, text, winStates[rightPlayer->getWinState()], WIN_W * 0.875, 110, 70, sf::Color::White, MID_CENTER);

		leftWin = (leftPlayer->getWinState() != WIN_STATE_NONE) ? "WIN" : "LOSE";
		rightWin = (rightPlayer->getWinState() != WIN_STATE_NONE) ? "WIN" : "LOSE";
	}

	this->back.draw(window, text, textureManager);

	// Left player info
	drawLeft(window, text, leftPlayer, leftWin);

	// Right player info
	drawRight(window, text, rightPlayer, rightWin);

	// Moves control
	drawText(window, text, "MOVES", WIN_W / 2, WIN_H * 0.875, 35, sf::Color::White, MID_CENTER);
	this->firstMove.draw(window, text, textureManager);
	this->previousMove.draw(window, text, textureManager);
	drawText(window, text, grid->getHistoryIdString(), WIN_W / 2, WIN_H * 0.93, 35, sf::Color::White, MID_CENTER);
	this->nextMove.draw(window, text, textureManager);
	this->lastMove.draw(window, text, textureManager);
	this->replay.draw(window, text, textureManager);
}


static void	drawLeft(sf::RenderWindow *window, sf::Text *text, Player *leftPlayer, std::string leftWin)
{
	std::string	timer;
	int	timePerMove;

	if (leftPlayer->getType() == AI_PLAYER) 
		timePerMove = leftPlayer->getAI()->getTotalTimer() / leftPlayer->getNbMove();
	else
		timePerMove = (leftPlayer->getTimer() * 1000000) / leftPlayer->getNbMove();

	drawText(window, text, leftPlayer->getName(), WIN_W * 0.15, WIN_H * 0.30, 50, sf::Color::White, MID_CENTER);
	drawText(window, text, leftWin, WIN_W * 0.15, WIN_H * 0.41, 50, sf::Color::White, MID_CENTER);

	timer = format_time(std::to_string((int)leftPlayer->getTimer())) + " s";
	drawText(window, text, "TIMER", WIN_W * 0.10, WIN_H * 0.52, 50, sf::Color::White, MID_CENTER);
	drawText(window, text, timer, WIN_W * 0.20, WIN_H * 0.52, 50, sf::Color::White, MID_CENTER);

	timer = format_time(std::to_string(timePerMove)) + " us";
	drawText(window, text, "time/move", WIN_W * 0.10, WIN_H * 0.565, 30, sf::Color(192, 192, 192), MID_CENTER);
	drawText(window, text, timer, WIN_W * 0.20, WIN_H * 0.565, 30, sf::Color(192, 192, 192), MID_CENTER);

	drawText(window, text, "MOVES", WIN_W * 0.10, WIN_H * 0.63, 50, sf::Color::White, MID_CENTER);
	drawText(window, text, std::to_string(leftPlayer->getNbMove()), WIN_W * 0.20, WIN_H * 0.63, 50, sf::Color::White, MID_CENTER);
	drawText(window, text, "CAPTURED", WIN_W * 0.10, WIN_H * 0.74, 50, sf::Color::White, MID_CENTER);
	drawText(window, text, std::to_string(leftPlayer->getNbCapture()), WIN_W * 0.20, WIN_H * 0.74, 50, sf::Color::White, MID_CENTER);
}


static void drawRight(sf::RenderWindow *window, sf::Text *text, Player *rightPlayer, std::string rightWin)
{
	std::string	timer;
	int	timePerMove;

	if (rightPlayer->getType() == AI_PLAYER) 
		timePerMove = rightPlayer->getAI()->getTotalTimer() / rightPlayer->getNbMove();
	else
		timePerMove = (rightPlayer->getTimer() * 1000000) / rightPlayer->getNbMove();

	drawText(window, text, rightPlayer->getName(), WIN_W * 0.85, WIN_H * 0.30, 50, sf::Color::White, MID_CENTER);
	drawText(window, text, rightWin, WIN_W * 0.85, WIN_H * 0.41, 50, sf::Color::White, MID_CENTER);

	timer = format_time(std::to_string((int)rightPlayer->getTimer())) + " s";
	drawText(window, text, "TIMER", WIN_W * 0.80, WIN_H * 0.52, 50, sf::Color::White, MID_CENTER);
	drawText(window, text, timer, WIN_W * 0.90, WIN_H * 0.52, 50, sf::Color::White, MID_CENTER);

	timer = format_time(std::to_string(timePerMove)) + " us";
	drawText(window, text, "time/move", WIN_W * 0.80, WIN_H * 0.565, 30, sf::Color(192, 192, 192), MID_CENTER);
	drawText(window, text, timer, WIN_W * 0.90, WIN_H * 0.565, 30, sf::Color(192, 192, 192), MID_CENTER);

	drawText(window, text, "MOVES", WIN_W * 0.80, WIN_H * 0.63, 50, sf::Color::White, MID_CENTER);
	drawText(window, text, std::to_string(rightPlayer->getNbMove()), WIN_W * 0.90, WIN_H * 0.63, 50, sf::Color::White, MID_CENTER);
	drawText(window, text, "CAPTURED", WIN_W * 0.80, WIN_H * 0.74, 50, sf::Color::White, MID_CENTER);
	drawText(window, text, std::to_string(rightPlayer->getNbCapture()), WIN_W * 0.90, WIN_H * 0.74, 50, sf::Color::White, MID_CENTER);
}
