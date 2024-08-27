#ifndef GAME_HPP
# define GAME_HPP

# include <define.hpp>
# include <ui/Button.hpp>
# include <game/Grid.hpp>
# include <game/Player.hpp>

class Game
{
public:
	Game(void);
	~Game();

	void		tick(display_state *displayState, float delta, Mouse *mouse);
	void		draw(sf::RenderWindow *window, sf::Text *text, TextureManager *textureManager);
	void		setGame(player_type playerLeft, player_type playerRight, game_mode mode);

	Player		*getLeftPlayer(void);
	Player		*getRightPlayer(void);
	Grid		*getGrid(void);

private:
	Button		leave;
	Grid		grid;
	sf::RectangleShape	rect;
	Player 		playerLeft, playerRight;
	game_mode	mode;

	void		drawLeftSide(sf::RenderWindow *window, sf::Text *text, TextureManager *textureManager);
	void		drawRightSide(sf::RenderWindow *window, sf::Text *text, TextureManager *textureManager);
	void		drawBottom(sf::RenderWindow *window, sf::Text *text, TextureManager *textureManager);
};

#endif