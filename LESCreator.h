#pragma once

#include <list>
#include <string>

#include "LinearEquationSystem.h"

class Logger;

using namespace std;

class LESCreator
{
public:
	LESCreator();
	~LESCreator();

	bool fillData(list<string>& rLines);

	LinearEquationSystem* getLES(void);

private:
	bool findParamMultiplicators(string ParamString,
								 map<int, double>& rMulitplicators);

	LinearEquationSystem* m_pEquationSystem;

	Logger* m_pLogger;
};

