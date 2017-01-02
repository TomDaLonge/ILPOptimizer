#include "Simplex.h"

#include "LinearEquationSystem.h"
#include "Logger.h"

using namespace std;

Simplex::Simplex(LinearEquationSystem* pLES)
{
	m_pEquationSystem = pLES;
	m_pLogger = Logger::getLogger();
}


Simplex::~Simplex(void)
{
}

bool 
	Simplex::doOneSimplexIteration(void)
{
	bool Success = true;

	m_pLogger->LogTrace("");
	m_pLogger->LogTrace("## Searching Pivot Values.. ##");

	//Search pivot column
	int ObjFunctRow = m_pEquationSystem->getObjFunctRow();
	int PivotColumn = m_pEquationSystem->getMaxPosFromRow(ObjFunctRow);
	if (-1 == PivotColumn)
	{
		Success &= false;
	}
	m_pLogger->LogDebug("Pivot: PivotColumn = " + to_string(PivotColumn));

	//Divide right side arguments by pivot column but DO NOT replace them!!!
	//Needed to find PivotRow in the next step
	int width = m_pEquationSystem->getWidth();
	map<int, double> ResultColumn;
	Success &= m_pEquationSystem->divideColumns(width - 1, PivotColumn, ResultColumn);

	//Find PivotRow
	int PivotRow = m_pEquationSystem->getMinPosWPositiveValueFromConstrCol(ResultColumn);
	if (-1 == PivotRow)
	{
		Success &= false;
	}
	m_pLogger->LogDebug("Pivot: PivotRow = " + to_string(PivotRow));

	//Find PivotElementValue
	double PivotElementValue = m_pEquationSystem->getValue(PivotRow, PivotColumn);
	m_pLogger->LogDebug("Pivot: PivotElementValue = " + to_string(PivotElementValue));

	//Make pivot element equal one
	m_pLogger->LogTrace("");
	m_pLogger->LogTrace("## Dividing PivotRow by PivotElement Value.. ##");
	Success &= m_pEquationSystem->divideRowByValue(PivotRow, PivotElementValue);
	m_pEquationSystem->printMatrix();

	//Make other Values of PivotColumn equal zero by subtracting multiples of PivotElement
	m_pLogger->LogTrace("");
	m_pLogger->LogTrace("## Making every Element in PivotColumn equal zero.. ##");
	int height = m_pEquationSystem->getHeight();
	for (int i = 0; i < height; i++)
	{
		if (PivotRow == i)
		{
			continue;
		}

		m_pEquationSystem->subtractRowByMultOfAnother(i, PivotRow, PivotColumn);
	}
	m_pEquationSystem->printMatrix();
	m_pLogger->LogTrace("");

	double currentResult = m_pEquationSystem->getValue(height - 1, width - 1);
	m_pLogger->LogTrace("Simplex: The result after this iteration equals " + to_string(currentResult) + ".");
	m_pLogger->LogTrace("");

	return Success;
}