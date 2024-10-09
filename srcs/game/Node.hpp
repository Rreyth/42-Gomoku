#ifndef NODE_HPP
# define NODE_HPP

# include <define.hpp>
# include <game/BitBoard.hpp>
# include <game/PlayerInfo.hpp>
# include <game/Grid.hpp>
# include <AI/Evaluation.hpp>

# include <vector>

class Node
{
public:
	Node(void);
	Node(const Node &obj);
	~Node();

	Node	&operator=(const Node &obj);

	// TODO: REMOVE
	void	printGrid(void);

private:

};

#endif
