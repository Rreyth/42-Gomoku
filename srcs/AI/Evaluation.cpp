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

	// Create verify for a left full align
	this->verifyAlignFullL[0] = 0b00000;
	this->verifyAlignFullL[1] = 0b10000;
	this->verifyAlignFullL[2] = 0b11000;
	this->verifyAlignFullL[3] = 0b11100;
	this->verifyAlignFullL[4] = 0b11110;
	this->verifyAlignFullL[5] = 0b11111;

	// Create verify for a right full align
	this->verifyAlignFullR[0] = 0b00000;
	this->verifyAlignFullR[1] = 0b00001;
	this->verifyAlignFullR[2] = 0b00011;
	this->verifyAlignFullR[3] = 0b00111;
	this->verifyAlignFullR[4] = 0b01111;
	this->verifyAlignFullR[5] = 0b11111;
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
		plCaptureMult, opCaptureMult,
		plLine, opLine,
		evalutation;

	// If interstion is already taken, return 0
	if (plBitBoard->get(x, y) || opBitBoard->get(x, y))
		return (0);

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

	// Create multiplier for capture
	plCaptureMult = plCapture / 2 + 1;
	opCaptureMult = opCapture / 2 + 1;

	evalutation = 0;

	// Check for horizontal
	plLine = plBitBoard->getLine(BBH, x, y);
	opLine = opBitBoard->getLine(BBH, x, y);
	evalutation += this->evaluatePositionOnAxis(
					plLine, opLine,
					checkL, checkR, shiftL, shiftR,
					plCapture, plCaptureMult, opCapture, opCaptureMult);
	if (evalutation >= CASE_WIN_POINT)
		return (evalutation);

	// Check for vertical
	plLine = plBitBoard->getLine(BBV, x, y);
	opLine = opBitBoard->getLine(BBV, x, y);
	evalutation += this->evaluatePositionOnAxis(
					plLine, opLine,
					checkVL, checkVR, shiftVL, shiftVR,
					plCapture, plCaptureMult, opCapture, opCaptureMult);
	if (evalutation >= CASE_WIN_POINT)
		return (evalutation);

	// Check for diagonal
	plLine = plBitBoard->getLine(BBD, x, y);
	opLine = opBitBoard->getLine(BBD, x, y);
	evalutation += this->evaluatePositionOnAxis(
					plLine, opLine,
					checkL, checkR, shiftL, shiftR,
					plCapture, plCaptureMult, opCapture, opCaptureMult);
	if (evalutation >= CASE_WIN_POINT)
		return (evalutation);

	// Check for anti diagonal
	plLine = plBitBoard->getLine(BBA, x, y);
	opLine = opBitBoard->getLine(BBA, x, y);
	evalutation += this->evaluatePositionOnAxis(
					plLine, opLine,
					checkL, checkR, shiftL, shiftR,
					plCapture, plCaptureMult, opCapture, opCaptureMult);

	return (evalutation);
}


int	Evaluation::evaluateGrid(
					BitBoard *plBitBoard, BitBoard *opBitBoard,
					int plCapture, int opCapture)
{
	int	shiftL, shiftR, checkL, checkR,
		shiftVL, shiftVR, checkVL, checkVR,
		plCaptureMult, opCaptureMult,
		plLineH, opLineH,
		plLineV, opLineV,
		plLineD, opLineD,
		plLineA, opLineA,
		evalutation;

	evalutation = 0;

	// Create multiplier for capture
	plCaptureMult = (plCapture * plCapture * plCapture) + 1;
	opCaptureMult = (opCapture * opCapture * opCapture) + 1;

	// For each intersection
	for (int y = 0; y < GRID_W_INTER - 1; y++)
	{
		// Create shitf and check for vertical
		shiftVR = y;
		shiftVL = shiftVR - 4;
		checkVL = 0b11111 << shiftVL;
		checkVR = 0b11111 << shiftVR;
		plLineH = plBitBoard->getLine(BBH, 0, y);
		opLineH = opBitBoard->getLine(BBH, 0, y);

		for (int x = 0; x < GRID_W_INTER - 1; x++)
		{
			// If it's empty, skip it
			if (!plBitBoard->get(x, y) && !opBitBoard->get(x, y))
				continue;

			// Create shitf and check for horizontal, diagonal and anti diagonal
			shiftR = x;
			shiftL = shiftR - 4;
			checkL = 0b11111 << shiftL;
			checkR = 0b11111 << shiftR;

			// Else, evaluate it's position
			plLineV = plBitBoard->getLine(BBV, x, y);
			opLineV = opBitBoard->getLine(BBV, x, y);
			plLineD = plBitBoard->getLine(BBD, x, y);
			opLineD = opBitBoard->getLine(BBD, x, y);
			plLineA = plBitBoard->getLine(BBA, x, y);
			opLineA = opBitBoard->getLine(BBA, x, y);

			// Evaluate position for horizontal axis
			evalutation += this->evaluateGridOnAxis(
									plLineH, opLineH,
									checkL, checkR, shiftL, shiftR,
									plCapture, plCaptureMult,
									opCapture, opCaptureMult);

			// Evaluate position for vertical axis
			evalutation += this->evaluateGridOnAxis(
									plLineV, opLineV,
									checkVL, checkVR, shiftVL, shiftVR,
									plCapture, plCaptureMult,
									opCapture, opCaptureMult);

			// Evaluate position for diagonal axis
			evalutation += this->evaluateGridOnAxis(
									plLineD, opLineD,
									checkL, checkR, shiftL, shiftR,
									plCapture, plCaptureMult,
									opCapture, opCaptureMult);

			// Evaluate position for anti diagonal axis
			evalutation += this->evaluateGridOnAxis(
									plLineA, opLineA,
									checkL, checkR, shiftL, shiftR,
									plCapture, plCaptureMult,
									opCapture, opCaptureMult);
		}
	}

	return (evalutation);
}

////////////////////////////////////////////////////////////////////////////////
// Private methods
////////////////////////////////////////////////////////////////////////////////

int	Evaluation::evaluatePositionOnAxis(
					int plLine, int opLine,
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
	plTmpL = (plLine & checkL) >> shiftL;
	opTmpL = (opLine & checkL) >> shiftL;
	plTmpR = (plLine & checkR) >> shiftR;
	opTmpR = (opLine & checkR) >> shiftR;

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
					int plLine, int opLine,
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
	plTmpL = (plLine & checkL) >> shiftL;
	opTmpL = (opLine & checkL) >> shiftL;
	plTmpR = (plLine & checkR) >> shiftR;
	opTmpR = (opLine & checkR) >> shiftR;

	plIsEvalL = false;
	opIsEvalL = false;
	plIsEvalR = false;
	opIsEvalR = false;
	plTmpCompleteLine = 0;
	opTmpCompleteLine = 0;
	for (int i = 0; i < 5; i++)
	{
		// Check for line complete on left
		if (plTmpL == this->verifyAlignFullL[i])
		{
			plIsEvalL = true;
			plTmpCompleteLine += i - 1;
		}

		// Check for line complete on right
		if (plTmpR == this->verifyAlignFullR[i])
		{
			plIsEvalR = true;
			plTmpCompleteLine += i - 1;
		}

		// Check for line block on left
		if (opTmpL == this->verifyAlignFullL[i])
		{
			opIsEvalL = true;
			opTmpCompleteLine += i - 1;
		}

		// Check for line block on right
		if (opTmpR == this->verifyAlignFullR[i])
		{
			opIsEvalR = true;
			opTmpCompleteLine += i - 1;
		}

		// If all case are already count, break
		if (plIsEvalL && opIsEvalL && plIsEvalR && opIsEvalR)
			break;
	}

	plTmpCompleteLine++;
	if (plTmpCompleteLine > 5)
		plTmpCompleteLine = 5;
	opTmpCompleteLine++;
	if (opTmpCompleteLine > 5)
		opTmpCompleteLine = 5;

	evalutation += this->completeLinePoint[plTmpCompleteLine];
	evalutation -= this->completeLinePoint[opTmpCompleteLine];

	// Check capture
	plTmpL = (plTmpL & 0b11100) >> 2;
	opTmpL = (opTmpL & 0b11100) >> 2;
	plTmpR = (plTmpR & 0b00111);
	opTmpR = (opTmpR & 0b00111);

	// Capture opponent's stone
	if ((plTmpL == this->verifyOutCaptureL && opTmpL == this->verifyInCaptureL)
		|| (plTmpR == this->verifyOutCaptureR && opTmpR == this->verifyInCaptureR))
	{
		// If this capture will make player win, make it an insane good move
		if (plCapture == 8)
			return (CASE_WIN_POINT);
		// Else count it like complete a 2 stone line
		// Multiply the point for increase reward as long as number of capture
		evalutation += this->completeLinePoint[3] * plCaptureMult;
	}

	// Block opponent's capture
	if ((plTmpL == this->verifyInCaptureL && opTmpL == this->verifyOutCaptureL)
		|| (plTmpR == this->verifyInCaptureR && opTmpR == this->verifyOutCaptureR))
	{
		// If this capture will make opponnent win, block it at if we cannot win !
		if (opCapture == 8)
			return (this->completeLinePoint[5] - 1);
		// Else count it like block a 2 stone line
		// Multiply the point for increase reward as long as number of capture
		evalutation -= this->completeLinePoint[3] * opCaptureMult;
	}

	return (evalutation);
}
