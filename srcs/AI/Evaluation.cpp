#include <AI/Evaluation.hpp>

////////////////////////////////////////////////////////////////////////////////
// Constructors and destructor
////////////////////////////////////////////////////////////////////////////////

Evaluation::Evaluation(void)
{
	// Create points for complete a line
	this->completeLinePoint[0] = 0; // Line of 0 stone
	this->completeLinePoint[1] = 10; // Line of 1 stone
	this->completeLinePoint[2] = 100; // Line of 2 stones
	this->completeLinePoint[3] = 1000; // Line of 3 stones
	this->completeLinePoint[4] = 100000; // Line of 4 stones
	this->completeLinePoint[5] = 10000000; // Line of 5 stones

	// Create points for block a line
	this->blockLinePoint[0] = 0; // Line of 0 stone
	this->blockLinePoint[1] = 1; // Line of 1 stone
	this->blockLinePoint[2] = 10; // Line of 2 stones
	this->blockLinePoint[3] = 100; // Line of 3 stones
	this->blockLinePoint[4] = 10000; // Line of 4 stones
	this->blockLinePoint[5] = 999000; // Line of 5 stones

	// Create verify for a left align (order is inversed in binary !)
	this->verifyAlignL[0] = 0b0000;
	this->verifyAlignL[1] = 0b1000;
	this->verifyAlignL[2] = 0b1100;
	this->verifyAlignL[3] = 0b1110;
	this->verifyAlignL[4] = 0b1111;

	// Create verify for a right align
	this->verifyAlignR[0] = 0b0000;
	this->verifyAlignR[1] = 0b0001;
	this->verifyAlignR[2] = 0b0011;
	this->verifyAlignR[3] = 0b0111;
	this->verifyAlignR[4] = 0b1111;

	// Create verify for a left capture
	this->verifyInCaptureL = 0b110;
	this->verifyOutCaptureL = 0b001;

	// Create verify for a right capture
	this->verifyInCaptureR = 0b011;
	this->verifyOutCaptureR = 0b100;
}


Evaluation::~Evaluation()
{

}

////////////////////////////////////////////////////////////////////////////////
// Public methods
////////////////////////////////////////////////////////////////////////////////

int	Evaluation::evaluationPosition(BitBoard *plBitBoard, BitBoard *opBitBoard,
									int plCapture, int opCapture, int x, int y)
{
	int	shiftL, shiftR, checkL, checkR,
		shiftVL, shiftVR, checkVL, checkVR,
		yD, yA,
		plCaptureMult, opCaptureMult,
		evalutation;

	// Create shitf and check for horizontal, diagonal and anti diagonal
	shiftR = x + 1;
	shiftL = shiftR - 5;
	checkL = 0b1111 << shiftL;
	checkR = 0b1111 << shiftR;

	// Create shitf and check for vertical
	shiftVR = y + 1;
	shiftVL = shiftVR - 5;
	checkVL = 0b1111 << shiftVL;
	checkVR = 0b1111 << shiftVR;

	// Create y for diagonal
	yD = (y + x) % GRID_W_INTER;

	// Create y for anti diagonal
	yA = y - x;
	if (yA < 0)
		yA = GRID_W_INTER + yA;

	// Create multiplier for capture
	plCaptureMult = plCapture / 2 + 1;
	opCaptureMult = opCapture / 2 + 1;

	evalutation = 0;

	// Check for horizontal
	evalutation += this->evaluatePositionOnAxis(
					plBitBoard->bbH, opBitBoard->bbH, y,
					checkL, checkR, shiftL, shiftR,
					plCapture, plCaptureMult, opCapture, opCaptureMult);
	if (evalutation >= CASE_WIN_POINT)
		return (evalutation);

	// Check for vertical
	evalutation += this->evaluatePositionOnAxis(
					plBitBoard->bbV, opBitBoard->bbV, x,
					checkVL, checkVR, shiftVL, shiftVR,
					plCapture, plCaptureMult, opCapture, opCaptureMult);
	if (evalutation >= CASE_WIN_POINT)
		return (evalutation);

	// Check for diagonal
	evalutation += this->evaluatePositionOnAxis(
					plBitBoard->bbD, opBitBoard->bbD, yD,
					checkL, checkR, shiftL, shiftR,
					plCapture, plCaptureMult, opCapture, opCaptureMult);
	if (evalutation >= CASE_WIN_POINT)
		return (evalutation);

	// Check for anti diagonal
	evalutation += this->evaluatePositionOnAxis(
					plBitBoard->bbA, opBitBoard->bbA, yA,
					checkL, checkR, shiftL, shiftR,
					plCapture, plCaptureMult, opCapture, opCaptureMult);

	return (evalutation);
}


int	Evaluation::evaluateGrid(
					BitBoard *plBitBoard, BitBoard *opBitBoard,
					int plCapture, int opCapture)
{
	return (0);
	// int				value, opValue, tmp;
	// intersection	*inter;

	// value = 0;
	// for (int y = 0; y < GRID_W_INTER; y++)
	// {
	// 	for (int x = 0; x < GRID_W_INTER; x++)
	// 	{
	// 		// inter = grid->getIntersection(x, y);
	// 		// if (inter->type == plType)
	// 		// {
	// 		// 	for (int i = 0; i < 8; i++)
	// 		// 	{
	// 		// 		tmp = inter->neighbor[i] + 1;
	// 		// 		if (tmp > 5)
	// 		// 			tmp = 5;
	// 		// 		value += this->completeLinePoint[tmp];
	// 		// 	}
	// 		// }
	// 		// else if (inter->type == opType)
	// 		// {
	// 		// 	for (int i = 0; i < 8; i++)
	// 		// 	{
	// 		// 		tmp = inter->neighbor[i] + 1;
	// 		// 		if (tmp > 5)
	// 		// 			tmp = 5;
	// 		// 		value -= this->completeLinePoint[tmp];
	// 		// 	}
	// 		// }
	// 		// Skip it tkt
	// 		// value += this->evaluationPosition(grid, plType, opType, plCapture, opCapture, x, y) / 2;
	// 		// value -= this->evaluationPosition(grid, opType, plType, opCapture, plCapture, x, y);
	// 	}
	// }

	// return (value);
}

////////////////////////////////////////////////////////////////////////////////
// Private methods
////////////////////////////////////////////////////////////////////////////////

int	Evaluation::evaluatePositionOnAxis(
					int *plBb, int *opBb, int y,
					int checkL, int checkR, int shiftL, int shiftR,
					int plCapture, int plCaptureMult,
					int opCapture, int opCaptureMult)
{
	int	plTmpL, opTmpL, plTmpR, opTmpR,
		plIsEvalL, opIsEvalL, plIsEvalR, opIsEvalR,
		tmpCompleteLine, tmpBlockLine,
		evalutation;

	evalutation = 0;

	// Check for horizontal
	plTmpL = (plBb[y] & checkL) >> shiftL;
	opTmpL = (opBb[y] & checkL) >> shiftL;
	plTmpR = (plBb[y] & checkR) >> shiftR;
	opTmpR = (opBb[y] & checkR) >> shiftR;

	plIsEvalL = false;
	opIsEvalL = false;
	plIsEvalR = false;
	opIsEvalR = false;
	tmpCompleteLine = 0;
	tmpBlockLine = 0;
	for (int i = 0; i < 5; i++)
	{
		// Check for line complete on left
		if (plTmpL == this->verifyAlignL[i])
		{
			plIsEvalL = true;
			tmpCompleteLine += i;
		}

		// Check for line complete on right
		if (plTmpR == this->verifyAlignR[i])
		{
			plIsEvalR = true;
			tmpCompleteLine += i;
		}

		// Check for line block on left
		if (opTmpL == this->verifyAlignL[i])
		{
			opIsEvalL = true;
			tmpBlockLine += i;
		}

		// Check for line block on right
		if (opTmpR == this->verifyAlignR[i])
		{
			opIsEvalR = true;
			tmpBlockLine += i;
		}

		// If all case are already count, break
		if (plIsEvalL && opIsEvalL && plIsEvalR && opIsEvalR)
			break;
	}

	if (tmpCompleteLine > 4)
		tmpCompleteLine = 4;
	if (tmpBlockLine > 4)
		tmpCompleteLine = 4;

	evalutation += this->completeLinePoint[tmpCompleteLine + 1];
	evalutation += this->blockLinePoint[tmpBlockLine + 1];

	// Check capture
	plTmpL = (plTmpL & 0b1110) >> 1;
	opTmpL = (opTmpL & 0b1110) >> 1;
	plTmpR = (plTmpR & 0b0111);
	opTmpR = (opTmpR & 0b0111);

	// Capture opponent's stone
	if ((plTmpL == this->verifyOutCaptureL && opTmpL == this->verifyInCaptureL)
		|| (plTmpR == this->verifyOutCaptureR && opTmpR == this->verifyInCaptureR))
	{
		// If this capture will make player win, make it an insane good move
		if (plCapture == 8)
			return (CASE_WIN_POINT);
		// Else count it like complete a 2 stone line
		// Multiply the point for increase reward as long as number of capture
		evalutation += this->completeLinePoint[2] * plCaptureMult;
	}

	// Block opponent's capture
	if ((plTmpL == this->verifyInCaptureL && opTmpL == this->verifyOutCaptureL)
		|| (plTmpR == this->verifyInCaptureR && opTmpR == this->verifyOutCaptureR))
	{
		// If this capture will make oppennent win, block it at if we cannot win !
		if (opCapture == 8)
			return (this->completeLinePoint[5] - 1);
		// Else count it like block a 2 stone line
		// Multiply the point for increase reward as long as number of capture
		evalutation += this->blockLinePoint[2] * opCaptureMult;
	}

	return (evalutation);
}


int	Evaluation::evaluateGridOnAxis(
					int *plBb, int *opBb, int y,
					int checkL, int checkR, int shiftL, int shiftR,
					int plCapture, int plCaptureMult,
					int opCapture, int opCaptureMult)
{
	int	plTmpL, opTmpL, plTmpR, opTmpR,
		plIsEvalL, opIsEvalL, plIsEvalR, opIsEvalR,
		plTmpCompleteLine, opTmpCompleteLine,
		evalutation;

	evalutation = 0;

	// Check for horizontal
	plTmpL = (plBb[y] & checkL) >> shiftL;
	opTmpL = (opBb[y] & checkL) >> shiftL;
	plTmpR = (plBb[y] & checkR) >> shiftR;
	opTmpR = (opBb[y] & checkR) >> shiftR;

	plIsEvalL = false;
	opIsEvalL = false;
	plIsEvalR = false;
	opIsEvalR = false;
	plTmpCompleteLine = 0;
	opTmpCompleteLine = 0;
	for (int i = 0; i < 5; i++)
	{
		// Check for line complete on left
		if (plTmpL == this->verifyAlignL[i])
		{
			plIsEvalL = true;
			plTmpCompleteLine += i;
		}

		// Check for line complete on right
		if (plTmpR == this->verifyAlignR[i])
		{
			plIsEvalR = true;
			plTmpCompleteLine += i;
		}

		// Check for line block on left
		if (opTmpL == this->verifyAlignL[i])
		{
			opIsEvalL = true;
			opTmpCompleteLine += i;
		}

		// Check for line block on right
		if (opTmpR == this->verifyAlignR[i])
		{
			opIsEvalR = true;
			opTmpCompleteLine += i;
		}

		// If all case are already count, break
		if (plIsEvalL && opIsEvalL && plIsEvalR && opIsEvalR)
			break;
	}

	if (plTmpCompleteLine > 4)
		plTmpCompleteLine = 4;
	if (opTmpCompleteLine > 4)
		plTmpCompleteLine = 4;

	evalutation += this->completeLinePoint[plTmpCompleteLine + 1];
	evalutation -= this->completeLinePoint[opTmpCompleteLine + 1];

	// // Check capture
	// plTmpL = (plTmpL & 0b1110) >> 1;
	// opTmpL = (opTmpL & 0b1110) >> 1;
	// plTmpR = (plTmpR & 0b0111);
	// opTmpR = (opTmpR & 0b0111);

	// // Capture opponent's stone
	// if ((plTmpL == this->verifyOutCaptureL && opTmpL == this->verifyInCaptureL)
	// 	|| (plTmpR == this->verifyOutCaptureR && opTmpR == this->verifyInCaptureR))
	// {
	// 	// If this capture will make player win, make it an insane good move
	// 	if (plCapture == 8)
	// 		return (CASE_WIN_POINT);
	// 	// Else count it like complete a 2 stone line
	// 	// Multiply the point for increase reward as long as number of capture
	// 	evalutation += this->completeLinePoint[2] * plCaptureMult;
	// }

	// // Block opponent's capture
	// if ((plTmpL == this->verifyInCaptureL && opTmpL == this->verifyOutCaptureL)
	// 	|| (plTmpR == this->verifyInCaptureR && opTmpR == this->verifyOutCaptureR))
	// {
	// 	// If this capture will make oppennent win, block it at if we cannot win !
	// 	if (opCapture == 8)
	// 		return (this->completeLinePoint[5] - 1);
	// 	// Else count it like block a 2 stone line
	// 	// Multiply the point for increase reward as long as number of capture
	// 	evalutation -= this->completeLinePoint[2] * opCaptureMult;
	// }

	return (evalutation);
}
