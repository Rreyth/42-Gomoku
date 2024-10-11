#ifndef BBOXMANAGER_HPP
# define BBOXMANAGER_HPP

# include <game/Bbox.hpp>

# include <SFML/Graphics.hpp>
# include <vector>

class BboxManager
{
public:
	BboxManager(void);
	BboxManager(const BboxManager &obj);
	~BboxManager();

	std::vector<Bbox>	*getBboxes(void);

	BboxManager	&operator=(const BboxManager &obj);

	void						update(int x, int y);
	std::vector<sf::Vector2i>	getListPosition(void) const;
	void						clear(void);

	// TODO: REMOVE
	void	print(void);

private:
	std::vector<Bbox>	bboxes;
	int					nbBbox;
};

#endif
