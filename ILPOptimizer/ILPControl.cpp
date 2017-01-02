#include "ILPControl.h"

#include <list>
#include <sstream>

#include "Logger.h"
#include "FileReader.h"
#include "LESCreator.h"
#include "Simplex.h"
#include "LinearEquationSystem.h"

ILPControl::ILPControl()
{
	m_pFileReader = new FileReader();
	m_pCreator = new LESCreator();
	m_pLogger = Logger::getLogger();
}

ILPControl::~ILPControl()
{
	delete m_pFileReader;
	delete m_pCreator;
}

bool ILPControl::StartILP(const char* InputFileName)
{
	bool Success = true;

	m_pLogger->LogTrace("=================================================================");
	m_pLogger->LogTrace("==                    Booting ILPOptimizer..                   ==");
	m_pLogger->LogTrace("=================================================================");
	m_pLogger->LogTrace("");

	m_pLogger->LogTrace("## Reading and Converting Document.. ##");
	
	list<string> Lines;
	if (m_pFileReader->getLines(InputFileName, Lines))
	{
		m_pLogger->LogTrace("");
		m_pLogger->LogTrace("## Feeding information to Equation System.. ##");

		//Interprets the information contained in the lines list and puts them into an equation system (Matrix)
		Success &= m_pCreator->fillData(Lines);

		LinearEquationSystem* pLES = m_pCreator->getLES();

		m_pLogger->LogTrace("");
		m_pLogger->LogTrace("## Creating Matrix.. ##");
		pLES->printMatrix();

		//Simplex can just find a maximum, in order to find a minimum, you have to translate the matrix first
		if (pLES->getIsMin())
		{
			m_pLogger->LogTrace("");
			m_pLogger->LogTrace("## Transposing Matrix.. ##");
			if (Success &= pLES->TransposeMatrix())
			{
				pLES->printMatrix();
			}
		}

		//Add Slack Variables to matrix
		if (Success)
		{
			m_pLogger->LogTrace("");
			m_pLogger->LogTrace("## Adding Slack Variables.. ##");
			Success &= pLES->addSlackVariables();
		}

		//Main Part
		int counter = 0;
		if (Success)
		{
			m_pLogger->LogTrace("");
			m_pLogger->LogTrace("=================================================================");
			m_pLogger->LogTrace("==                      Doing the Simplex..                    ==");
			m_pLogger->LogTrace("=================================================================");

			Simplex* pSimplexElement = new Simplex(pLES);
			
			//Abort criterion -> every parameter in objective function is less or equal 0
			while (!pLES->IsObjFunctLessOrEqualZero() && true == Success)
			{
				counter++;

				m_pLogger->LogTrace("");
				m_pLogger->LogTrace("## " + to_string(counter) + ". Simplex Iteration: ##");

				Success &= pSimplexElement->doOneSimplexIteration();
			}
		}
		
		if (true == Success)
		{
			m_pLogger->LogTrace("=================================================================");
			m_pLogger->LogTrace("==                      Found a Solution!                      ==");
			m_pLogger->LogTrace("=================================================================");
			m_pLogger->LogTrace("");

			m_pLogger->LogTrace("Solution: Needed " + to_string(counter) + " Iterations!");
			m_pLogger->LogTrace("");

			//Print solution value (min or max)
			int height = pLES->getHeight();
			int width = pLES->getWidth();
			//used stringstream to set print precision of double values
			stringstream solutionValue;
			solutionValue.precision(15);
			solutionValue << pLES->getValue(height - 1, (width - 1)) * -1;
			if (pLES->getIsMin())
			{
				m_pLogger->LogTrace("Solution: Min = " + solutionValue.str());
			}
			else
			{
				m_pLogger->LogTrace("Solution: Max = " + solutionValue.str());
			}
			
			m_pLogger->LogTrace("");
			
			//Print values for other variables
			for (int i = 0; i < height - 1; i++)
			{
				stringstream currentValueStream;
				currentValueStream.precision(15);
				currentValueStream << pLES->getValue(i, (width - 1));
				m_pLogger->LogTrace("Solution: x" + to_string(i) + "= " + currentValueStream.str());
			}
		}
	}
	else
	{
		Success &= false;
	}

	return Success;
}