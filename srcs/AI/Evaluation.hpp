#ifndef EVALUATION_HPP
# define EVALUATION_HPP

# include <define.hpp>
# include <game/Grid.hpp>


class Evaluation
{
public:
	Evaluation(void);
	~Evaluation();

	int	evaluationPosition(Grid *grid, inter_type plType, inter_type opType,
							int plCapture, int opCapture, int x, int y);
	int	evaluateGrid(Grid *grid, inter_type plType, inter_type opType,
							int plCapture, int opCapture);

private:
	int				completeLinePoint[6], blockLinePoint[6];
	sf::Vector2i	dirs[8];

	void	updateCompleteAndBlockLine(
				Grid *grid, inter_type plType, inter_type opType,
				int plCapture, int opCapture,
				int x, int y, int axis,
				int *completeLine, bool *isCompleteLine,
				int *blockLine, bool *isBlockLine, int *result);
};

#endif
