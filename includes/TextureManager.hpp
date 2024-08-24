#ifndef TEXTUREMANAGER_HPP
# define TEXTUREMANAGER_HPP

# include <define.hpp>

# include <SFML/Graphics.hpp>
# include <string>

# define NB_SPRITES 1

typedef enum e_sprite_name
{
	SPRITE_GRID = 0
}	sprite_name;

class TextureManager
{
public:
	TextureManager(void);
	~TextureManager();

	void	loadTextures(void);
	void	drawTexture(sf::RenderWindow *window, sprite_name name, int x, int y, draw_pos pos);

private:
	sf::Texture	textures[NB_SPRITES];
	sf::Sprite	sprites[NB_SPRITES];

	void loadTexture(sprite_name name, std::string filename);
};

#endif
