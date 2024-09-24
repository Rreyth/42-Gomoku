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

	BboxManager	&operator=(const BboxManager &obj);

	void						saveBboxes(std::vector<Bbox> *bboxes);
	void						loadBboxes(std::vector<Bbox> *bboxes);
	void						update(int x, int y);
	std::vector<sf::Vector2i>	getListPosition(void) const;
	void						clear(void);

private:
	std::vector<Bbox>	bboxes;
	int					nbBbox;
};

#endif
