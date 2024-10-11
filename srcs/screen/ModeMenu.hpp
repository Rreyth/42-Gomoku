#ifndef MODMENU_HPP
# define MODMENU_HPP

# include <define.hpp>
# include <ui/Button.hpp>
# include <ui/Select.hpp>
# include <screen/Game.hpp>

class ModeMenu
{
public:
	ModeMenu(void);
	~ModeMenu();

	void	setSuggestion(bool suggestion);

	void	tick(display_state *displayState, float delta, Mouse *mouse, Game *game, stone_sprite *sprite);
	void	draw(sf::RenderWindow *window, sf::Text *text, TextureManager *textureManager);

private:
	std::string	title;
	Button		back, play;
	Select		playerLeft, playerRight,
					gamemode, gamerules,
					aiLeft, aiRight,
					starting;
	bool		suggestion;
};

#endif