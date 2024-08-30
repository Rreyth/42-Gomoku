#ifndef END_HPP
# define END_HPP

# include <define.hpp>
# include <ui/Button.hpp>
# include <game/Player.hpp>
# include <game/Grid.hpp>

class End
{
public:
	End(void);
	~End();

	void	tick(display_state *displayState, float delta, Mouse *mouse, Grid *grid);
	void	draw(sf::RenderWindow *window, sf::Text *text, TextureManager *textureManager,
				Player *leftPlayer, Player *rightPlayer, Grid *grid);

private:
	std::string			title;
	Button				back, firstMove, previousMove, nextMove, lastMove;
	sf::RectangleShape	rect;
};

#endif
