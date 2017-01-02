#pragma once

#include <string>
#include <fstream>

using namespace std;

class Logger
{
public:
	static Logger* getLogger(void);

	void setLogLevel(int LogLevel);

	void setDumpToFile(bool DumpToFile, 
					   string FileName = "");


	void LogTrace(string Message);

	void LogDebug(string Message);

	void LogWarning(string Message);

	void LogError(string Message);
	

	void printAnalysis(void);

	//IMPORTANT!!!!
	void deleteLogger(void);

private:
	Logger(void);

	int WarnCount;

	int ErrorCount;

	static Logger* m_pInstance;

	static int mLogLevel;

	static bool mDumpToFile;

	static ofstream m_OutputFile;
};