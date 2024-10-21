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


	// Mask for align stone left
	this->checkNbStoneL[0] = 0b000001000;
	this->checkNbStoneL[1] = 0b000001100;
	this->checkNbStoneL[2] = 0b000001110;
	this->checkNbStoneL[3] = 0b000001111;

	// Mask for align stone right
	this->checkNbStoneR[0] = 0b000100000;
	this->checkNbStoneR[1] = 0b001100000;
	this->checkNbStoneR[2] = 0b011100000;
	this->checkNbStoneR[3] = 0b111100000;

	// Score for align stone
	this->alignScore[0] = 10;
	this->alignScore[1] = 100;
	this->alignScore[2] = 1000;
	this->alignScore[3] = 100000;
	this->alignScore[4] = 10000000;

	// Score for block stone align
	this->blockScore[0] = 0;
	this->blockScore[1] = 10;
	this->blockScore[2] = 100;
	this->blockScore[3] = 10000;
	this->blockScore[4] = 1000000;

	this->addStepOnAxis[0] = sf::Vector2i(1, 0); // BBH
	this->addStepOnAxis[1] = sf::Vector2i(0, 1); // BBV
	this->addStepOnAxis[2] = sf::Vector2i(1, -1); // BBD
	this->addStepOnAxis[3] = sf::Vector2i(1, 1); // BBA
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


int	Evaluation::evalPosition(
					BitBoard *plBitBoard, BitBoard *opBitBoard,
					int plCapture, int opCapture, int x, int y)
{
	int	score = 0;

	// Return 0 if there already have a stone on this pos
	if (plBitBoard->get(x, y) || opBitBoard->get(x, y))
		return (0);

	// Evaluate on horizontal axis
	score = this->evalPosAxis(
					plBitBoard, opBitBoard, BBH,
					&plCapture, opCapture, x, y);
	if (score >= CASE_WIN_POINT || score <= CASE_LOOSE_POINT)
		return (score);

	// Evaluate on vertical axis
	score += this->evalPosAxis(
					plBitBoard, opBitBoard, BBV,
					&plCapture, opCapture, x, y);
	if (score >= CASE_WIN_POINT || score <= CASE_LOOSE_POINT)
		return (score);

	// Evaluate on diagonal axis
	score += this->evalPosAxis(
					plBitBoard, opBitBoard, BBD,
					&plCapture, opCapture, x, y);
	if (score >= CASE_WIN_POINT || score <= CASE_LOOSE_POINT)
		return (score);

	// Evaluate on anti diagonal axis
	score += this->evalPosAxis(
					plBitBoard, opBitBoard, BBA,
					&plCapture, opCapture, x, y);

	return (score);
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
			plTmpCompleteLine += i;
		}

		// Check for line complete on right
		if (plTmpR == this->verifyAlignFullR[i])
		{
			plIsEvalR = true;
			plTmpCompleteLine += i;
		}

		// Check for line block on left
		if (opTmpL == this->verifyAlignFullL[i])
		{
			opIsEvalL = true;
			opTmpCompleteLine += i;
		}

		// Check for line block on right
		if (opTmpR == this->verifyAlignFullR[i])
		{
			opIsEvalR = true;
			opTmpCompleteLine += i;
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


int	Evaluation::evalPosAxis(
					BitBoard *plBitBoard, BitBoard *opBitBoard, bitboardAxis axis,
					int *plCapture, int opCapture, int x, int y)
{
	int	score,
		bitX, plLine, opLine,
		nbStoneBlock;

	score = 0;

	// Get line part
	bitX = this->getLinePart(plBitBoard, opBitBoard, x, y, axis, &plLine, &opLine);

	// Get make line point
	score += this->computeAlignScore(
					plBitBoard, opBitBoard, x, y, axis,
					plLine, opLine, bitX, opCapture);

	// Get block line point
	score += this->computeBlockScore(opLine);

	// Get capture point
	score += this->computeCaptureScore(
					plBitBoard, opBitBoard, axis,
					plCapture, opCapture, x, y,
					plLine, opLine);

	return (score);
}


int	Evaluation::getLinePart(
					BitBoard *plBitBoard, BitBoard *opBitBoard,
					int x, int y, bitboardAxis axis,
					int *plLinePart, int *opLinePart)
{
	int	plLine, opLine,
		shift, lineMask,
		bitX;

	// Get the bitX
	if (axis == BBV)
		bitX = y;
	else
		bitX = x;

	// Get line by axis
	plLine = plBitBoard->getLine(axis, x, y);
	opLine = opBitBoard->getLine(axis, x, y);

	// Compute shift
	shift = bitX - 4;
	if (shift < 0)
		lineMask = 0b111111111 >> -shift;
	else
		lineMask = 0b111111111 << shift;

	// Compute line part by apply the mask and shift it
	if (shift < 0)
	{
		*plLinePart = (plLine & lineMask) << -shift;
		*opLinePart = (opLine & lineMask) << -shift;
	}
	else
	{
		*plLinePart = (plLine & lineMask) >> shift;
		*opLinePart = (opLine & lineMask) >> shift;
	}

	return (bitX);
}


int	Evaluation::computeAlignScore(
					BitBoard *plBitBoard, BitBoard *opBitBoard,
					int x, int y, bitboardAxis axis,
					int plLine, int opLine, int bitX,
					int opCapture)
{
	int		nbStoneL, nbStoneR, nbStone,
			nbOpStoneL, nbOpStoneR, nbOpStone,
			nbFreeL, nbFreeR,
			score;

	// Get the number of stone at the left (for H)
	nbStoneL = 0;
	for (int i = 0; i < 4; i++)
	{
		if ((plLine & this->checkNbStoneL[i]) == this->checkNbStoneL[i])
			nbStoneL = i + 1;
		else
			break;
	}
	// Get the number of stone at the right (for H)
	nbStoneR = 0;
	for (int i = 0; i < 4; i++)
	{
		if ((plLine & this->checkNbStoneR[i]) == this->checkNbStoneR[i])
			nbStoneR = i + 1;
		else
			break;
	}

	// Get the number of space before the closest ennemy stone on the left
	nbFreeL = 5;
	for (int i = 3; i >= 0; i--)
	{
		if (opLine & this->checkNbStoneL[i])
			nbFreeL = i;
		else
			break;
	}
	// Correct the free space in accordance with the border
	if (bitX - nbFreeL < 0)
		nbFreeL += bitX - nbFreeL;

	// Get the number of space before the closest ennemy stone on the right
	nbFreeR = 5;
	for (int i = 3; i >= 0; i--)
	{
		if (opLine & this->checkNbStoneR[i])
			nbFreeR = i;
		else
			break;
	}
	// Correct the free space in accordance with the border
	if (bitX + nbFreeR >= GRID_W_INTER)
		nbFreeR -= (bitX + nbFreeR) - GRID_W_INTER + 1;

	nbStone = nbStoneL + nbStoneR + 1;

	// If line have 5 or more stones, check if this is a victory
	if (nbStone >= 5)
	{
		if (this->isLineWinnable(
					plBitBoard, opBitBoard, x, y, axis,
					plLine, opLine, bitX,
					nbStoneL, nbStoneR, nbStone))
			return (CASE_WIN_POINT); // If yes, return the max score

		else if (opCapture == 8)
			return (CASE_LOOSE_POINT); // Auto capture win for opponent
	}

	// Get score of line
	if (nbStone > 5)
		nbStone = 5;
	score = this->alignScore[nbStone - 1];

	// If threre is no enough space for a 5 stone ligne, it's a 0 score
	if (nbFreeL + nbFreeR + 1 < 5)
		return (0);
	// If there is an opponent stone on the left on player line stone
	if (nbFreeL == nbStoneL)
		score /= 2;
	// If there is an opponent stone on the right on player line stone
	if (nbFreeR == nbStoneR)
		score /= 2;

	return (score);
}


bool	Evaluation::isLineWinnable(
						BitBoard *plBitBoard, BitBoard *opBitBoard,
						int x, int y, bitboardAxis axis,
						int plLine, int opLine, int bitX,
						int nbStoneL, int nbStoneR, int nbStone)
{
	// Check killer move
	int 	checkX, checkY,
			nbStoneOk, checkPlLine, checkOpLine;
	bool	capturable;

	// Get start position and increments depends on axis
	if (axis == BBH)
	{
		checkX = x - nbStoneL;
		checkY = y;
	}
	else if (axis == BBV)
	{
		checkX = x;
		checkY = y - nbStoneL;
	}
	else if (axis == BBD)
	{
		checkX = x - nbStoneL;
		checkY = y + nbStoneL;
	}
	else // BBA
	{
		checkX = x - nbStoneL;
		checkY = y - nbStoneL;
	}

	// For each stone
	nbStoneOk = 0;
	for (int i = 0; i < nbStone; i++)
	{
		capturable = false;

		// If the stone is capturable, break the winning line
		if (this->isCapturable(
					plBitBoard, opBitBoard, checkX, checkY, axis,
					plLine, opLine, bitX,
					nbStoneL, nbStoneR, nbStone))
		{
			nbStoneOk = 0;
			// If we know there is not enough stone left to win, stop here
			if (nbStone - i - 1 < 5)
				break;
		}
		// Else continue it
		else
		{
			nbStoneOk++;
			// If we have already 5 stone ok, stop here
			if (nbStoneOk == 5)
				break;
		}
		checkX += this->addStepOnAxis[axis].x;
		checkY += this->addStepOnAxis[axis].y;
	}

	// If there is 5 stones ok, it's a victory !
	if (nbStoneOk == 5)
		return (true);
	return (false);
}


bool	Evaluation::isCapturable(
						BitBoard *plBitBoard, BitBoard *opBitBoard,
						int x, int y, bitboardAxis axis,
						int plLine, int opLine, int bitX,
						int nbStoneL, int nbStoneR, int nbStone)
{
	bool	skipCapture1, skipCapture2;
	int		shift,
			lineMasks[2],
			plTmpLine[2], opTmpLine[2];

	for (int checkAxis = 0; checkAxis < 4; checkAxis++)
	{
		if (checkAxis == axis) // Skip the check of our axis
			continue;

		if (checkAxis == BBH)
		{
			skipCapture1 = (x <= 0 || x >= GRID_W_INTER - 2);
			skipCapture2 = (x <= 1 || x >= GRID_W_INTER - 1);
		}
		else if (checkAxis == BBV)
		{
			skipCapture1 = (y <= 0 || y >= GRID_W_INTER - 2);
			skipCapture2 = (y <= 1 || y >= GRID_W_INTER - 1);
		}
		else if (checkAxis == BBD)
		{
			skipCapture1 = (x <= 0 || x >= GRID_W_INTER - 2
							|| y <= 1 || y >= GRID_W_INTER - 1);
			skipCapture2 = (x <= 1 || x >= GRID_W_INTER - 1
							|| y <= 0 || y >= GRID_W_INTER - 2);
		}
		else // BBA
		{
			skipCapture1 = (x <= 0 || x >= GRID_W_INTER - 2
							|| y <= 0 || y >= GRID_W_INTER - 2);
			skipCapture2 = (x <= 1 || x >= GRID_W_INTER - 1
							|| y <= 1 || y >= GRID_W_INTER - 1);
		}

		// If both capture case are impossible, skip stone check
		if (skipCapture1 && skipCapture2)
			continue;

		if (checkAxis == BBV)
			shift = y - 1;
		else
			shift = x - 1;

		if (shift < 0)
		{
			lineMasks[0] = 0b01011 >> -shift;
			lineMasks[1] = 0b11010 >> -shift;
		}
		else
		{
			lineMasks[0] = 0b11010 << shift;
			lineMasks[1] = 0b01011 << shift;
		}

		plLine = plBitBoard->getLine((bitboardAxis)checkAxis, x, y) << 1;
		opLine = opBitBoard->getLine((bitboardAxis)checkAxis, x, y) << 1;

		if (shift < 0)
		{
			plTmpLine[0] = (plLine & lineMasks[0]) << -shift;
			plTmpLine[1] = (plLine & lineMasks[1]) << (-shift - 1);
			opTmpLine[0] = (opLine & lineMasks[0]) << -shift;
			opTmpLine[1] = (opLine & lineMasks[1]) << (-shift - 1);
		}
		else
		{
			plTmpLine[0] = (plLine & lineMasks[0]) >> (shift + 1);
			plTmpLine[1] = (plLine & lineMasks[1]) >> shift;
			opTmpLine[0] = (opLine & lineMasks[0]) >> (shift + 1);
			opTmpLine[1] = (opLine & lineMasks[1]) >> shift;
		}

		if (plTmpLine[0] == 0b0100 && !skipCapture1)
		{
			if (opTmpLine[0] == 0b1000 || opTmpLine[0] == 0b0001)
				return (true);
		}
		if (plTmpLine[1] == 0b0010 && !skipCapture2)
		{
			if (opTmpLine[1] == 0b1000 || opTmpLine[1] == 0b0001)
				return (true);
		}
	}

	return (false);
}


int	Evaluation::computeBlockScore(int opLine)
{
	int	nbOpStoneL, nbOpStoneR, nbOpStone,
		score;

	nbOpStoneL = 0;
	for (int i = 0; i < 4; i++)
	{
		if ((opLine & this->checkNbStoneL[i]) == this->checkNbStoneL[i])
			nbOpStoneL = i + 1;
		else
			break;
	}
	nbOpStoneR = 0;
	for (int i = 0; i < 4; i++)
	{
		if ((opLine & this->checkNbStoneR[i]) == this->checkNbStoneR[i])
			nbOpStoneR = i + 1;
		else
			break;
	}

	nbOpStone = nbOpStoneL + nbOpStoneR;

	// Get score of block line
	score = this->blockScore[nbOpStone];

	return (score);
}


int	Evaluation::computeCaptureScore(
					BitBoard *plBitBoard, BitBoard *opBitBoard, bitboardAxis axis,
					int *plCapture, int opCapture, int x, int y,
					int plLine, int opLine)
{
	int		plCheckL, plCheckR,
			opCheckL, opCheckR,
			check1X, check1Y,
			check2X, check2Y,
			nbStone1, nbStone2,
			score;

	score = 0;
	plCheckL = (plLine & 0b000001110) >> 1;
	plCheckR = (plLine & 0b011100000) >> 4;
	opCheckL = (opLine & 0b000001110) >> 1;
	opCheckR = (opLine & 0b011100000) >> 4;

	// Player capture left
	if (opCheckL == 0b0110 && plCheckL == 0b0001)
	{
		// Get addition score for capture
		if (*plCapture == 8)
			return (CASE_WIN_POINT);
		else if (*plCapture == 6)
			score += 5000;
		else
			score += 500;

		// Update number of capture for further check
		*plCapture += 2;

		check1X = x - this->addStepOnAxis[axis].x;
		check1Y = y - this->addStepOnAxis[axis].y;
		check2X = check1X - this->addStepOnAxis[axis].x;
		check2Y = check1Y - this->addStepOnAxis[axis].y;

		for (int checkAxis = 0; checkAxis < 4; checkAxis++)
		{
			if (checkAxis == axis)
				continue;

			nbStone1 = this->getLineOfPos(
								opBitBoard, check1X, check1Y,
								(bitboardAxis)checkAxis);
			if (nbStone1 != 5)
				score += this->blockScore[nbStone1 - 1];
			else
				score -= 1000000;

			nbStone2 = this->getLineOfPos(
								opBitBoard, check2X, check2Y,
								(bitboardAxis)checkAxis);
			if (nbStone2 != 5)
				score += this->blockScore[nbStone2 - 1];
			else
				score -= 1000000;
		}
		return (score);
	}

	// Player capture right
	if (opCheckR == 0b0110 && plCheckR == 0b1000)
	{
		// Get addition score for capture
		if (*plCapture == 8)
			return (CASE_WIN_POINT);
		else if (*plCapture == 6)
			score += 5000;
		else
			score += 500;

		// Update number of capture for further check
		*plCapture += 2;

		check1X = x + this->addStepOnAxis[axis].x;
		check1Y = y + this->addStepOnAxis[axis].y;
		check2X = check1X + this->addStepOnAxis[axis].x;
		check2Y = check1Y + this->addStepOnAxis[axis].y;

		for (int checkAxis = 0; checkAxis < 4; checkAxis++)
		{
			if (checkAxis == axis)
				continue;

			nbStone1 = this->getLineOfPos(
								opBitBoard, check1X, check1Y,
								(bitboardAxis)checkAxis);
			if (nbStone1 != 5)
				score += this->blockScore[nbStone1 - 1];
			else
				score -= 1000000;

			nbStone2 = this->getLineOfPos(
								opBitBoard, check2X, check2Y,
								(bitboardAxis)checkAxis);
			if (nbStone2 != 5)
				score += this->blockScore[nbStone2 - 1];
			else
				score -= 1000000;
		}
		return (score);
	}

	// Player block capture left
	if (plCheckL == 0b0110 && opCheckL == 0b0001)
	{
		// Get addition score for block capture
		if (opCapture == 8)
			score += 50000000;
		else if (opCapture == 6)
			score += 2500;
		else
			score += 250;

		check1X = x - this->addStepOnAxis[axis].x;
		check1Y = y - this->addStepOnAxis[axis].y;
		check2X = check1X - this->addStepOnAxis[axis].x;
		check2Y = check1Y - this->addStepOnAxis[axis].y;

		for (int checkAxis = 0; checkAxis < 4; checkAxis++)
		{
			if (checkAxis == axis)
				continue;

			nbStone1 = this->getLineOfPos(
								plBitBoard, check1X, check1Y,
								(bitboardAxis)checkAxis);
			if (nbStone1 == 5)
				score += this->alignScore[nbStone1 - 1];
			else if (nbStone1 > 0)
				score += this->alignScore[nbStone1 - 2];

			nbStone2 = this->getLineOfPos(
								plBitBoard, check2X, check2Y,
								(bitboardAxis)checkAxis);
			if (nbStone2 == 5)
				score += this->alignScore[nbStone2 - 1];
			else if (nbStone2 > 0)
				score += this->alignScore[nbStone2 - 2];
		}
		return (score);
	}

	// Player block capture right
	if (plCheckR == 0b0110 && opCheckR == 0b1000)
	{
		// Get addition score for block capture
		if (opCapture == 8)
			score += 50000000;
		else if (opCapture == 6)
			score += 2500;
		else
			score += 250;

		check1X = x + this->addStepOnAxis[axis].x;
		check1Y = y + this->addStepOnAxis[axis].y;
		check2X = check1X + this->addStepOnAxis[axis].x;
		check2Y = check1Y + this->addStepOnAxis[axis].y;

		for (int checkAxis = 0; checkAxis < 4; checkAxis++)
		{
			if (checkAxis == axis)
				continue;

			nbStone1 = this->getLineOfPos(
								plBitBoard, check1X, check1Y,
								(bitboardAxis)checkAxis);
			if (nbStone1 == 5)
				score += this->alignScore[nbStone1 - 1];
			else if (nbStone1 > 0)
				score += this->alignScore[nbStone1 - 2];

			nbStone2 = this->getLineOfPos(
								plBitBoard, check2X, check2Y,
								(bitboardAxis)checkAxis);
			if (nbStone2 == 5)
				score += this->alignScore[nbStone2 - 1];
			else if (nbStone2 > 0)
				score += this->alignScore[nbStone2 - 2];
		}
	}

	return (score);
}


int	Evaluation::getLineOfPos(
					BitBoard *bitboard,
					int x, int y, bitboardAxis axis)
{
	int	line, linePart, shift, lineMask,
		nbStoneL, nbStoneR, nbStone;

	// Get line by axis
	line = bitboard->getLine(axis, x, y);

	// Compute shift
	if (axis == BBV)
		shift = y - 4;
	else
		shift = x - 4;

	if (shift < 0)
		lineMask = 0b111111111 >> -shift;
	else
		lineMask = 0b111111111 << shift;

	// Compute line part by apply the mask and shift it
	if (shift < 0)
		linePart = (line & lineMask) << -shift;
	else
		linePart = (line & lineMask) >> shift;

	// Get the number of stone at the left (for H)
	nbStoneL = 0;
	for (int i = 0; i < 4; i++)
	{
		if ((linePart & this->checkNbStoneL[i]) == this->checkNbStoneL[i])
			nbStoneL = i + 1;
		else
			break;
	}
	// Get the number of stone at the right (for H)
	nbStoneR = 0;
	for (int i = 0; i < 4; i++)
	{
		if ((linePart & this->checkNbStoneR[i]) == this->checkNbStoneR[i])
			nbStoneR = i + 1;
		else
			break;
	}
	nbStone = nbStoneL + nbStoneR + 1;
	if (nbStone > 5)
		nbStone = 5;
	return (nbStone);
}
