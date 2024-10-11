#ifndef SETTINGS_HPP
# define SETTINGS_HPP

# include <define.hpp>
# include <ui/Button.hpp>
# include <ui/Select.hpp>
# include <ui/ToggleButton.hpp>
# include <screen/ModeMenu.hpp>

class Settings
{
public:
	Settings(void);
	~Settings();

	void	tick(ModeMenu *menu, display_state *displayState, float delta, Mouse *mouse, sf::RenderWindow *window, sf::View *view, sf::Text *text, sf::Font *font, stone_sprite *sprite);
	void	draw(sf::RenderWindow *window, sf::Text *text, TextureManager *textureManager);

private:
	std::string		title;
	Button			back;
	ToggleButton	fullscreen, suggestion;
	Select			resolution, stones, fonts;
	int				currentResolutionId, currentFontId, currentStoneId;

	void	updateWindow(sf::RenderWindow *window, sf::View *view);
	void	updateFont(sf::Text *text, sf::Font *font);
	void	updateStone(stone_sprite *sprite);
	void	updateSuggestion(ModeMenu *menu);
};

#endif
