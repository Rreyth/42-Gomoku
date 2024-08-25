#ifndef GAME_HPP
# define GAME_HPP

# include <define.hpp>
# include <Button.hpp>
# include <Grid.hpp>
# include <Player.hpp>

class Game
{
public:
	Game(void);
	~Game();

	void		tick(display_state *displayState, float delta, Mouse *mouse);
	void		draw(sf::RenderWindow *window, sf::Text *text, TextureManager *textureManager);
	void		setGame(player_type playerLeft, player_type playerRight, game_mode mode);

private:
	Button		leave;
	Grid		grid;
	std::string	turn;
	sf::RectangleShape	rect;
	Player 		playerLeft, playerRight;
	game_mode	mode;

	void		drawLeftSide(sf::RenderWindow *window, sf::Text *text, TextureManager *textureManager);
	void		drawRightSide(sf::RenderWindow *window, sf::Text *text, TextureManager *textureManager);
	void		drawBottom(sf::RenderWindow *window, sf::Text *text, TextureManager *textureManager);
};

#endif
