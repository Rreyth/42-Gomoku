#ifndef SETTINGS_HPP
# define SETTINGS_HPP

# include <define.hpp>
# include <ui/Button.hpp>
# include <ui/Select.hpp>
# include <ui/ToggleButton.hpp>

class Settings
{
public:
	Settings(void);
	~Settings();

	void	tick(display_state *displayState, float delta, Mouse *mouse, sf::RenderWindow *window, sf::View *view);
	void	draw(sf::RenderWindow *window, sf::Text *text, TextureManager *textureManager);

private:
	std::string		title;
	Button			back;
	ToggleButton	fullscreen;
	Select			resolution;
	int				currentResolutionId;
};

#endif
