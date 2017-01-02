#include "LinearEquationSystem.h"

#include <vector>
#include <sstream>

#include "Logger.h"

using namespace std;

LinearEquationSystem::LinearEquationSystem(void)
{
	m_pLogger = Logger::getLogger();
};

LinearEquationSystem::~LinearEquationSystem(void)
{
	//Clear maps
	size_t mainSize = mValueMatrix.size();

	for (int mainCnt = 0; mainCnt < mainSize; mainCnt++)
	{
		mValueMatrix[mainCnt].clear();
	}

	mValueMatrix.clear();
}

void 
	LinearEquationSystem::setIsMin(bool isMin)
{
	mIsMin = isMin;
}

void 
	LinearEquationSystem::setObjFunctRow(int OFRow)
{
	mObjFunctRow = OFRow;
}

void 
	LinearEquationSystem::setObjFuncParams(map<int, double>& rMulitplicators,
											int ConstrCount)
{
	//Add a zero for the right hand side (not included in input format)
	int MultiCount = static_cast<int>(rMulitplicators.size());
	rMulitplicators[MultiCount] = 0;
	
	//Is there already an objective function?
	bool ObjFunctEmpty = mValueMatrix[ConstrCount].empty();

	mObjFunctRow = ConstrCount;

	if (!ObjFunctEmpty)
	{
		mValueMatrix[ConstrCount].clear();
		m_pLogger->LogWarning("LES: Had to delete Values while setting ObjFuncParams.");
	}

	mValueMatrix[ConstrCount] = rMulitplicators;
}

bool
	LinearEquationSystem::addConstrParams(map<int, double>& rMultiplicators)
{
	bool Success = true;

	int width = getWidth();
	int height = getHeight();

	//First check if the dimension is right
	int MultiWidth = static_cast<int>(rMultiplicators.size());
	
	if (MultiWidth == width)
	{
		mValueMatrix[height] = rMultiplicators;
	}
	else
	{
		m_pLogger->LogError("LES: Tried addConstrParams() but but some arguments did not fit.");
		Success &= false;
	}

	return Success;
}

bool 
	LinearEquationSystem::setConstrRow(map<int, double>& rowValues,
									   int RowNr)
{
	bool Success = true;

	int width = getWidth();

	//Check whether line exists and remove values
	bool isLineEmpty = mValueMatrix[RowNr].empty();

	if (!isLineEmpty)
	{
		mValueMatrix[RowNr].clear();
		m_pLogger->LogWarning("LES: Had to clear line before setting it. If you want to change a row, use changeRow() instead of setRow().");
	}

	//Do not just change the objective function
	int rowValueWidth = static_cast<int>(rowValues.size());
	if (rowValueWidth == width && !isObjFunctRow(RowNr))
	{
		mValueMatrix[RowNr] = rowValues;
	}
	else
	{
		m_pLogger->LogError("LES: Tried setConstrRow() but some arguments did not fit or you wanted to change the objective function.");
		Success &= false;
	}

	return Success;
}

bool
	LinearEquationSystem::changeRow(map<int, double>& rowValues,
									int	RowNr)
{
	bool Success = true;

	int height = getHeight();


	if (RowNr < height)
	{
		mValueMatrix[RowNr].clear();
		mValueMatrix[RowNr] = rowValues;
	}
	else
	{
		m_pLogger->LogError("LES: Tried changeRow() but some arguments did not fit or you wanted to change the objective function.");
		Success &= false;
	}

	return Success;
}

bool 
	LinearEquationSystem::isObjFunctRow(int RowNr)
{
	return (RowNr == mObjFunctRow);
}

void
	LinearEquationSystem::setColumn(map<int, double>& columnValues,
									int ColNr)
{
	int size = static_cast<int>(columnValues.size());

	for (int i = 0; i < size; i++)
	{
		mValueMatrix[i][ColNr] = columnValues[i];
	}
}

bool 
	LinearEquationSystem::changeColumn(map<int, double>& columnValues,
								       int ColNr)
{
	bool Success = true;

	int height = getHeight();
	int width = getWidth();
	int newColHeight = static_cast<int>(columnValues.size());

	if (ColNr < width && newColHeight == height)
	{
		for (int i = 0; i < newColHeight; i++)
		{
			changeValue(columnValues[i], ColNr, i);
		}
	}
	else
	{
		m_pLogger->LogError("LES: Tried to changeColumn() but some arguments did not fit.");
		Success &= false;
	}

	return Success;
}

void
	LinearEquationSystem::setValue(double value, int x, int y)
{
	mValueMatrix[y][x] = value;
}

bool 
	LinearEquationSystem::changeValue(double value, 
									  int x, 
									  int y)
{
	bool Success = true;

	int width = getWidth();
	int height = getHeight();

	if (x < width && y < height)
	{
		mValueMatrix[y][x] = value;
	}
	else
	{
		m_pLogger->LogError("LES: Tried to changeValue() but some arguments did not fit.");
		Success &= false;
	}
	
	return Success;
}

bool 
	LinearEquationSystem::getIsMin(void)
{
	return mIsMin;
}

int 
	LinearEquationSystem::getObjFunctRow(void)
{
	return mObjFunctRow;
}

int
	LinearEquationSystem::getNumberOfConstrs(void)
{
	return static_cast<int>(mValueMatrix.size()-1);
}

bool
	LinearEquationSystem::getRow(int rowNr,
								 map<int, double>& rRow)
{
	bool Success = true;

	int height = getHeight();

	if (rowNr < height)
	{
		rRow.clear();
		rRow = mValueMatrix[rowNr];
	}
	else
	{
		m_pLogger->LogError("LES: Tried to getRow() but some arguments did not fit.");
		Success &= false;
	}

	return Success;
}

bool
	LinearEquationSystem::getColumn(int columnNr,
									map<int, double>& rColumn)
{
	bool Success = true;

	int height = getHeight();
	int width = getWidth();

	if (columnNr < width)
	{
		for (int i = 0; i < height; i++)
		{
			rColumn[i] = mValueMatrix[i][columnNr];
		}
	}
	else
	{
		m_pLogger->LogError("LES: Tried to getColumn() but some arguments did not fit.");
		Success &= false;
	}

	return Success;
}

double
	LinearEquationSystem::getValue(int Row,
								   int Column)
{
	//TODO: Make more safe, now the program would probably just hard crash because it can't find a value
	//or even worse, goes on with doing the simplex with a random value from memory.
	//idea: return bool, double value as reference
	int height = getHeight();
	int width = getWidth();

	if (Row >= height || Column >= width)
	{
		m_pLogger->LogError("LES: Tried to get value from position that is out of range. You should really search for Errors, because the simplex might be going on with a wrong value.");
	}

	return mValueMatrix[Row][Column];
}

int
	LinearEquationSystem::getMaxPosFromRow(int row)
{
	map<int, double> rowValues = mValueMatrix[row];

	int maxValuePos = -1;
	//Because DBL_MIN is the minimum of a positive double value
	double maxValue = -DBL_MAX;

	int rowWidth = static_cast<int>(rowValues.size());

	for (int i = 0; i < rowWidth; i++)
	{
		if (rowValues[i] > maxValue)
		{
			maxValue = rowValues[i];
			maxValuePos = i;
		}
	}

	return maxValuePos;
}

int
	LinearEquationSystem::getMinPosFromConstrCol(int col)
{
	int height = getHeight();

	map<int, double> ColumnValues;

	int minValuePos = 0;
	double minValue = DBL_MAX;

	if (getColumn(col, ColumnValues))
	{
		for (int i = 0; i < height - 1; i++)
		{
			double tmpValue = ColumnValues[i];
			if (tmpValue < minValue)
			{
				minValue = ColumnValues[i];
				minValuePos = i;
			}
		}
	}

	return minValuePos;
}

int
	LinearEquationSystem::getMinPosWPositiveValueFromConstrCol(map<int, double>& rColumn)
{
	int height = static_cast<int>(rColumn.size());

	int minValuePos = -1;
	double minValue = DBL_MAX;

	for (int i = 0; i < height - 1; i++)
	{
		double tmpValue = rColumn[i];
		if (tmpValue < minValue && tmpValue > 0)
		{
			minValue = rColumn[i];
			minValuePos = i;
		}
	}

	return minValuePos;
}

int
	LinearEquationSystem::getHeight(void)
{
	return static_cast<int>(mValueMatrix.size());
}

int 
	LinearEquationSystem::getWidth(void)
{
	return static_cast<int>(mValueMatrix[getObjFunctRow()].size());
}

bool 
	LinearEquationSystem::checkDimensions(void)
{
	bool Success = true;

	int height = getHeight();
	int width;

	if (0 < height)
	{
		width = getWidth();
	}
	else
	{
		m_pLogger->LogError("LES: The dimensions do not fit.");
		Success &= false;
	}

	if (1 <= height)
	{
		for (int i = 1; i < height; i++)
		{
			if (mValueMatrix[i].size() != width)
			{
				m_pLogger->LogError("LES: The dimensions do not fit.");
				Success &= false;
			}
		}
	}

	return Success;
}

bool
	LinearEquationSystem::addSlackVariables(void)
{
	bool Success = true;

	//Objective Function
	Success &= addSlackVarsToObjFunct();

	//Constraints
	Success &= addSlackVarsToConstraints();

	//Check Dimensions
	Success &= checkDimensions();

	if (Success)
	{
		printMatrix();
	}

	return Success;
}

bool
	LinearEquationSystem::addSlackVarsToObjFunct(void)
{
	bool Success = true;

	int numberOfConstraints = getNumberOfConstrs();
	int width = getWidth();

	if (!mValueMatrix.empty() && 0 < numberOfConstraints && 1 < width && isObjFunctRow(numberOfConstraints))
	{
		//Move right side argument
		double newValue = getValue(0, width - 1);
		setValue(newValue, (width - 2 + numberOfConstraints), numberOfConstraints);

		//Add zeros
		for (int i = 0; i <= numberOfConstraints; i++)
		{
			setValue(0.0, (width + i - 1), numberOfConstraints);
		}
	}
	else
	{
		m_pLogger->LogError("LES: Tried to addSlackVarsToObjFunct() but some arguments did not fit.");
		Success &= false;
	}

	return Success;
}

bool
	LinearEquationSystem::addSlackVarsToConstraints(void)
{
	bool Success = true;

	int numberOfConstraints = getNumberOfConstrs();

	if (0 < numberOfConstraints)
	{
		for (int i = 0; i < numberOfConstraints; i++)
		{
			map<int, double> currentConstraint;
			Success &= getRow(i, currentConstraint);

			int size = static_cast<int>(currentConstraint.size());

			//Move right side argument
			currentConstraint[size + numberOfConstraints - 1] = currentConstraint[size - 1];

			//Add zeros
			for (int u = 0; u < numberOfConstraints; u++)
			{
				if (u == i)
				{
					currentConstraint[size + u - 1] = 1;
				}
				else
				{
					currentConstraint[size + u - 1] = 0;
				}

			}
			Success &= changeRow(currentConstraint, i);
		}
	}
	else
	{
		m_pLogger->LogError("LES: Tried to addSlackVarsToConstrs() but there are no Constraints.");
		Success &= false;
	}

	return Success;
}

//This function DOES NOT change the CounterColumn but changes the given rResultColumn
bool 
	LinearEquationSystem::divideColumns(int counterRow, 
									    int denomRow,
										map<int, double>& rResultColumn)
{
	bool Success = true;

	Success &= checkDimensions();

	map<int, double> counterColumn;
	Success &= getColumn(counterRow, counterColumn);

	map<int, double> denomColumn;
	Success &= getColumn(denomRow, denomColumn);

	if (Success)
	{
		int size = static_cast<int>(counterColumn.size());
		for (int i = 0; i < size; i++)
		{
			if (0.0 >= denomColumn[i])
			{
				rResultColumn[i] = counterColumn[i];
				continue;
			}

			rResultColumn[i] = counterColumn[i] / denomColumn[i];
		}
	}

	return Success;
}

bool 
	LinearEquationSystem::divideRowByValue(int rowNr, 
										   double value)
{
	bool Success = true;

	int width = getWidth();

	if (0.0 != value && 1.0 != value && rowNr < width)
	{
		map<int, double> CurrentRow;
		Success &= getRow(rowNr, CurrentRow);

		int size = static_cast<int>(CurrentRow.size());
		for (int i = 0; i < size; i++)
		{
			if (0.0 == CurrentRow[i])
			{
				continue;
			}

			CurrentRow[i] = CurrentRow[i] / value;
		}

		changeRow(CurrentRow, rowNr);
	}
	else if (0.0 == value || rowNr >= width)
	{
		m_pLogger->LogError("LES: Tried divideRowByValue() but some arguments did not fit.");
		Success &= false;
	}

	return Success;
}

bool 
	LinearEquationSystem::subtractRowByMultOfAnother(int ActionRowNr, 
													 int PivotRowNr, 
													 int PivotColumnNr)
{
	bool Success = true;

	Success &= checkDimensions();

	int width = getWidth();
	int height = getHeight();

	if (Success && ActionRowNr < height && PivotRowNr < height && PivotColumnNr < width)
	{
		//Copy to tmpRows
		map<int, double> ActionRow;
		Success &= getRow(ActionRowNr, ActionRow);
		map<int, double> PivotRow;
		Success &= getRow(PivotRowNr, PivotRow);

		double multiplier = ActionRow[PivotColumnNr];

		int size = static_cast<int>(ActionRow.size());

		for (int i = 0; i < size; i++)
		{
			double ActionValue = ActionRow[i];
			double PivotRowValue = PivotRow[i];

			if (0 == PivotRowValue)
			{
				continue;
			}

			double newValue = ActionValue - (PivotRowValue * multiplier);

			ActionRow[i] = newValue;
		}

		changeRow(ActionRow, ActionRowNr);
	}
	else
	{
		m_pLogger->LogError("LES: Tried subtractRowByMultOfAnother() but some arguments did not fit.");
		Success &= false;
	}

	return Success;
}

bool 
	LinearEquationSystem::IsObjFunctLessOrEqualZero(void)
{
	int ObjFunctRow = getObjFunctRow();
	int MaxPos = getMaxPosFromRow(ObjFunctRow);
	double MaxValue = mValueMatrix[ObjFunctRow][MaxPos];

	bool isLessOrEqualZero = false;

	if (0.0 >= MaxValue)
	{
		isLessOrEqualZero = true;
	}

	return isLessOrEqualZero;
}

bool 
	LinearEquationSystem::TransposeMatrix(void)
{
	bool Success = true;

	if (!mValueMatrix.empty() && checkDimensions())
	{
		int height = getHeight();
		int width = getWidth();

		map<int, map< int, double>> tmpMatrix = mValueMatrix;

		mValueMatrix.clear();

		for (int i = 0; i < height; i++)
		{
			setColumn(tmpMatrix[i], i);
		}

		setObjFunctRow(static_cast<int>(mValueMatrix.size() - 1));
	}
	else
	{
		m_pLogger->LogError("LES: Tried transposeMatrix() but Matrix is empty or the Dimensions do not fit.");
		Success &= false;
	}

	return Success;
}

void
	LinearEquationSystem::printMatrix(void)
{
	int rowCount = getHeight();
	int colCount = getWidth();

	int ObjFunctRow = getObjFunctRow();

	//First line that contains the numbers of the columns
	string firstLineString = "    ";
	for (int col = 0; col < colCount; col++)
	{
		if (0 <= col && 9 >= col)
		{
			firstLineString += "      ";
		}
		else if (10 <= col && 99 >= col)
		{
			firstLineString += "     ";
		}
		else if (100 <= col && 999 >= col)
		{
			firstLineString += "    ";
		}
		firstLineString += to_string(col);
	}
	m_pLogger->LogDebug("");
	m_pLogger->LogDebug(firstLineString);
	
	//Print Rows
	for (int row = 0; row < rowCount; row++)
	{
		if (ObjFunctRow == row)
		{
			m_pLogger->LogDebug("   Objective Function");
		}
		else if (0 == row)
		{
			m_pLogger->LogDebug("   Constraints");
		}

		string currentRowString;
		if (0 <= row && 9 >= row)
		{
			currentRowString += "  ";
		}
		else if (10 <= row && 99 >= row)
		{
			currentRowString += " ";
		}

		currentRowString += to_string(row) + " [";

		int colSize = static_cast<int>(mValueMatrix[row].size());

		for (int col = 0; col < colSize; col++)
		{
			if (col != 0)
			{
				currentRowString += ",";
			}
			double tmpValue = mValueMatrix[row][col];

			//Trying to avoid messing the formatting up completely
			if ((tmpValue - floor(tmpValue) < 0.000000000000000000000000000001) || (tmpValue - floor(tmpValue) > 0.999999999999999999999999999))
			{
				if (0 <= tmpValue && 9 >= tmpValue)
				{
					currentRowString += "     ";
				}
				else if (0 > tmpValue && -9 <= tmpValue || 10 <= tmpValue && 99 >= tmpValue)
				{
					currentRowString += "    ";
				}
				else if (-10 >= tmpValue && -99 <= tmpValue || 100 <= tmpValue && 999 >= tmpValue)
				{
					currentRowString += "   ";
				}
				else if (-100 >= tmpValue && -999 <= tmpValue || 1000 <= tmpValue && 9999 >= tmpValue)
				{
					currentRowString += "  ";
				}
			}
			else if (0 < tmpValue)
			{
				currentRowString += " ";
			}

			stringstream sStr;
			sStr.precision(3);
			sStr << tmpValue;
			currentRowString += sStr.str();
		}

		currentRowString += "]";
		m_pLogger->LogDebug(currentRowString);
	}
}