#ifndef GAME_HPP
# define GAME_HPP

# include <define.hpp>
# include <Button.hpp>
# include <Grid.hpp>

class Game
{
public:
	Game(void);
	~Game();

	void		tick(display_state *displayState, float delta, Mouse *mouse);
	void		draw(sf::RenderWindow *window, sf::Text *text, TextureManager *textureManager);
	void		setPlayers(player_type playerLeft, player_type playerRight);

private:
	Button		leave;
	Grid		grid;
	std::string	turn;
	sf::RectangleShape	rect;
	// Player 		playerLeft, playerRight;

	void		drawLeftSide(sf::RenderWindow *window, sf::Text *text, TextureManager *textureManager);
	void		drawRightSide(sf::RenderWindow *window, sf::Text *text, TextureManager *textureManager);
	void		drawBottom(sf::RenderWindow *window, sf::Text *text, TextureManager *textureManager);
};

#endif
