#include "LESCreator.h"

#include <iostream>
#include <vector>
#include <sstream>

#include "Logger.h"

LESCreator::LESCreator()
{
	m_pEquationSystem = new LinearEquationSystem;
	m_pLogger = Logger::getLogger();
}

LESCreator::~LESCreator()
{
	delete m_pEquationSystem;
}

bool
LESCreator::fillData(list<string>& rLines)
{
	bool Success = true;

	int ConstrCount = static_cast<int>(rLines.size() - 1);
	int ConstrLineIterator = 0;

	//Convert every line from string to LinearEquationSystem format
	while (!rLines.empty() && Success)
	{
		string currentLine = rLines.front();

		size_t Pos = 0;

		//Distinguish between line types

		//Constraints
		if (string::npos != (Pos = currentLine.find("<=")) || string::npos != (Pos = currentLine.find(">=")))
		{
			m_pLogger->LogDebug("LESCreator: Found constraint");

			//Divide left and right part, string currentLine from now on is the right side
			string leftSideString = currentLine.substr(0, Pos);
			currentLine.erase(0, Pos + 2);

			//Find multiplicators of the left side
			map<int, double> multiplicators;
			Success &= findParamMultiplicators(leftSideString, multiplicators);
			int ParamCount = static_cast<int>(multiplicators.size());

			//Find right side parameter and add it to matrix
			int rightSideParam = atoi(currentLine.c_str());
			multiplicators[ParamCount] = rightSideParam;
			m_pLogger->LogDebug("LESCreator: [" + to_string(rightSideParam) + "]");

			//Add to matrix
			Success &= m_pEquationSystem->setConstrRow(multiplicators, ConstrLineIterator);
			ConstrLineIterator++;
		}
		//Objective Function
		else if (string::npos != currentLine.find("max:") || string::npos != currentLine.find("min:"))
		{
			m_pLogger->LogDebug("LESCreator: Found Objective Function");

			//Distinguish between min and max
			size_t Pos = 0;
			if (string::npos != (Pos = currentLine.find("max")))
			{
				m_pEquationSystem->setIsMin(false);
				m_pLogger->LogDebug("LESCreator: Max");
			}
			else if (string::npos != (Pos = currentLine.find("min")))
			{
				m_pEquationSystem->setIsMin(true);
				m_pLogger->LogDebug("LESCreator: Min");
			}

			//Cut string to leave only the objective function remaining
			currentLine = currentLine.substr(currentLine.find(":") + 1);

			//Find Multiplicators
			map<int, double> ObjFuncParams;
			Success &= findParamMultiplicators(currentLine, ObjFuncParams);

			m_pEquationSystem->setObjFuncParams(ObjFuncParams, ConstrCount);
		}
		else
		{
			m_pLogger->LogError("LESCreator: Found line that is neither constraint nor objective Function.");
			Success &= false;
		}

		//Remove line from list
		rLines.pop_front();
	}

	return Success;
}

LinearEquationSystem* 
	LESCreator::getLES(void)
{
	return m_pEquationSystem;
}

bool
	LESCreator::findParamMultiplicators(string ParamString,
										map<int, double>& rMulitplicators)
{
	bool Success = true;

	int i = 0;

	string multDelimiter = "*";
	string addDelimiter = "+";
	string subDelimiter = "-";

	size_t Pos = 0;
	stringstream OutputString;
	OutputString.precision(3);
	OutputString << "LESCreator: [";

	while (string::npos != (Pos = ParamString.find(multDelimiter)))
	{
		string currentMultiplierStr = ParamString.substr(0, Pos);
		double currentMultiplierDBL;
		size_t subPos = 0;

		//check whether the mulitplicator is positive or negative
		bool isNegative;
		if (string::npos != (subPos = currentMultiplierStr.find(addDelimiter)))
		{
			isNegative = false;
		}
		else if (string::npos != (subPos = currentMultiplierStr.find(subDelimiter)))
		{
			isNegative = true;
		}
		else if (0 == i)
		{
			isNegative = false;
		}
		else
		{
			m_pLogger->LogError("LESCreator: Found multiplicator without algebraic sign: " + currentMultiplierStr);
			Success &= false;
			break;
		}

		//Reduce currentMultiplierStr to only the mulitplier itself (Remove former variable)
		if (0 < i)
		{
			currentMultiplierStr.erase(0, subPos);
			OutputString << ", ";
		}

		//Convert to double
		currentMultiplierDBL = atoi(currentMultiplierStr.c_str());

		//Add current value to map
		OutputString << currentMultiplierDBL;
		int size = static_cast<int>(rMulitplicators.size());
		rMulitplicators[size] = currentMultiplierDBL;
		i++;

		//Reduce ParamString to remaining params
		ParamString.erase(0, Pos + multDelimiter.length());
	}
	OutputString << "]";
	m_pLogger->LogDebug(OutputString.str());

	return Success;
}