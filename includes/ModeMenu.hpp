#ifndef MODMENU_HPP
# define MODMENU_HPP

# include <define.hpp>
# include <Button.hpp>

class ModeMenu
{
public:
	ModeMenu(void);
	~ModeMenu();

	void	tick(display_state *displayState, float delta, Mouse *mouse);
	void	draw(sf::RenderWindow *window, sf::Text *text, TextureManager *textureManager);

private:
	std::string	title;
	Button		back;
};

#endif
