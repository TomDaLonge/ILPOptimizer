#pragma once

#include <string>

class FileReader;
class LESCreator;
class Logger;

using namespace std;

class ILPControl
{
public:
	ILPControl();
	~ILPControl();

	bool StartILP(const char* InputFileName);

private:
	FileReader* m_pFileReader;
	LESCreator* m_pCreator;

	Logger* m_pLogger;
};