// #ifndef NODE_HPP
// # define NODE_HPP

// # include <define.hpp>
// # include <game/BitBoard.hpp>
// # include <game/PlayerInfo.hpp>

// # include <vector>

// class Node
// {
// public:
// 	int					evaluation;
// 	bool				playerTurn, childrenCompute, evaluationCompute;
// 	BitBoard			playerBitboard, opponentBitboard;
// 	PlayerInfo			player, opponent;
// 	sf::Vector2i		lastMove;
// 	std::vector<Node>	children;

// 	Node(void);
// 	Node(const Node &obj);
// 	~Node();

// 	Node	&operator=(const Node &obj);
// };

// #endif
