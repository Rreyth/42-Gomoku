#ifndef EVALUATION_HPP
# define EVALUATION_HPP

# include <define.hpp>
# include <game/Grid.hpp>
# include <game/BitBoard.hpp>

# define CASE_WIN_POINT 1000000000
# define CASE_LOOSE_POINT -1000000000

class Evaluation
{
public:
	Evaluation(void);
	~Evaluation();

	int	evaluationPosition(BitBoard *plBitBoard, BitBoard *opBitBoard,
							int plCapture, int opCapture, int x, int y);
	int	evaluateGrid(BitBoard *plBitBoard, BitBoard *opBitBoard,
						int plCapture, int opCapture);

private:
	int				completeLinePoint[6], blockLinePoint[6],
					verifyAlignL[5], verifyAlignR[5],
					verifyInCaptureL, verifyOutCaptureL,
					verifyInCaptureR, verifyOutCaptureR;

	int	evaluatePositionOnAxis(
			int plLine, int opLine,
			int checkL, int checkR, int shiftL, int shiftR,
			int plCapture, int plCaptureMult,
			int opCapture, int opCaptureMult);

	int	evaluateGridOnAxis(
			int plLine, int opLine,
			int checkL, int checkR, int shiftL, int shiftR,
			int plCapture, int plCaptureMult,
			int opCapture, int opCaptureMult);
};

#endif
