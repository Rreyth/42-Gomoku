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
	int	evalPosition(BitBoard *plBitBoard, BitBoard *opBitBoard,
							int plCapture, int opCapture, int x, int y);

private:
	int		completeLinePoint[6], blockLinePoint[6],
			verifyAlignL[5], verifyAlignR[5],
			verifyAlignFullL[6], verifyAlignFullR[6],
			verifyInCaptureL, verifyOutCaptureL,
			verifyInCaptureR, verifyOutCaptureR;

	int				checkNbStoneL[4], checkNbStoneR[4],
					alignScore[5], blockScore[5];
	sf::Vector2i	addStepOnAxis[4];

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


	int		evalPosAxis(
				BitBoard *plBitBoard, BitBoard *opBitBoard, bitboardAxis axis,
				int *plCapture, int opCapture, int x, int y);
	int		getLinePart(
				BitBoard *plBitBoard, BitBoard *opBitBoard,
				int x, int y, bitboardAxis axis,
				int *plLinePart, int *opLinePart);
	int		computeAlignScore(
				BitBoard *plBitBoard, BitBoard *opBitBoard,
				int x, int y, bitboardAxis axis,
				int plLine, int opLine, int bitX,
				int opCapture);
	bool	isLineWinnable(
				BitBoard *plBitBoard, BitBoard *opBitBoard,
				int x, int y, bitboardAxis axis,
				int plLine, int opLine, int bitX,
				int nbStoneL, int nbStoneR, int nbStone);
	bool	isCapturable(
				BitBoard *plBitBoard, BitBoard *opBitBoard,
				int x, int y, bitboardAxis axis,
				int plLine, int opLine, int bitX,
				int nbStoneL, int nbStoneR, int nbStone);
	int		computeBlockScore(int opLine);
	int		computeCaptureScore(
				BitBoard *plBitBoard, BitBoard *opBitBoard, bitboardAxis axis,
				int *plCapture, int opCapture, int x, int y,
				int plLine, int opLine);
	int		getLineOfPos(
				BitBoard *bitboard,
				int x, int y, bitboardAxis axis);
};

#endif
