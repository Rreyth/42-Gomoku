#ifndef GAME_HPP
# define GAME_HPP

# include <define.hpp>
# include <Button.hpp>

class Game
{
public:
	Game(void);
	~Game();

	void	tick(display_state *displayState, float delta, Mouse *mouse);
	void	draw(sf::RenderWindow *window, sf::Text *text, TextureManager *textureManager);

private:
	Button		leave;
};

#endif
