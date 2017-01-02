#pragma once

class LinearEquationSystem;
class Logger;

class Simplex
{
public:
	Simplex(LinearEquationSystem* pLES);
	~Simplex(void);

	bool doOneSimplexIteration(void);

private:
	LinearEquationSystem* m_pEquationSystem;

	Logger* m_pLogger;
};

