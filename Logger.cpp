#include "Logger.h"

#include <iostream>
#include <ctime>


Logger* Logger::m_pInstance = nullptr;
int Logger::mLogLevel = 2;
bool Logger::mDumpToFile = false;
ofstream Logger::m_OutputFile;

Logger::Logger(void) 
{
	WarnCount = 0;
	ErrorCount = 0;
}

Logger* 
	Logger::getLogger(void)
{
	if (nullptr == m_pInstance)
	{
		m_pInstance = new Logger();
	}
	return m_pInstance;
}

void
	Logger::setLogLevel(int LogLevel)
{
	//0 = Debug, 1 = Warning, 2 = Error

	if (0 <= LogLevel && LogLevel <= 2)
	{
		mLogLevel = LogLevel;
	}
	else
	{
		string WarnString = "Tried to change LogLevel to" + to_string(LogLevel) +
			". Logger only knows Debug = 0, Warning = 1 and Error = 2. Current LogLevel is " +
			to_string(mLogLevel) + ".";

		LogWarning(WarnString);
	}
}

void 
	Logger::setDumpToFile(bool DumpToFile, 
						  string FileName)
{
	bool Success = true;

	if (!mDumpToFile && DumpToFile)
	{
		
		
		if ("" != FileName)
		{
			if (FileName.find(".txt") == std::string::npos)
			{
				FileName += ".txt";
			}

			//Try to open file
			m_OutputFile = ofstream(FileName);
			Success &= m_OutputFile.is_open();
		}
		
		if ("" == FileName || !Success)
		{
			time_t currentTime;
			struct tm localTime;

			time(&currentTime);
			localtime_s(&localTime, &currentTime);

			string Year = to_string(localTime.tm_year + 1900);
			string Month = to_string(localTime.tm_mon + 1);
			string Day = to_string(localTime.tm_mday);
			
			string Hour = to_string(localTime.tm_hour);
			string Min = to_string(localTime.tm_min);
			string Sec = to_string(localTime.tm_sec);

			FileName = Year + "-" + Month + "-" + Day + "_" +
				Hour + "-" + Min + "-" + Sec + +"- LoggerDump.txt";

			m_OutputFile = ofstream(FileName);
			Success &= m_OutputFile.is_open();
		}
	}
	else if (mDumpToFile && !DumpToFile)
	{
		//Close Stream
		m_OutputFile.close();
	}
	
	if (Success)
	{
		mDumpToFile = DumpToFile;
	}
	else
	{
		LogWarning("Tried to open FileStream in order to dump the LoggerOutput but something went wrong. Check the Inputs.");
	}
	
}


void 
	Logger::LogTrace(string Message)
{
	cout << Message << endl;
	if (mDumpToFile)
	{
		m_OutputFile << Message << endl;
	}
}

void 
	Logger::LogDebug(string Message)
{
	if (0 == mLogLevel)
	{
		cout << "Debug: " << Message << endl;
		if (mDumpToFile)
		{
			m_OutputFile << "Debug: " << Message << endl;
		}
	}
}

void 
	Logger::LogWarning(string Message)
{
	if (1 >= mLogLevel)
	{
		cout << "Warning: " << Message << endl;
		if (mDumpToFile)
		{
			m_OutputFile << "Warning: " << Message << endl;
		}
	}
	WarnCount++;
}

void 
	Logger::LogError(string Message)
{
	if (2 >= mLogLevel)
	{
		cout << "Error: " << Message << endl;
		if (mDumpToFile)
		{
			m_OutputFile << "Error: " <<Message << endl;
		}
	}
	ErrorCount++;
}


void 
	Logger::printAnalysis(void)
{
	LogTrace("Logger: Found " + to_string(WarnCount) + " Warnings and " + to_string(ErrorCount) + " Errors.");
}

void 
	Logger::deleteLogger(void)
{
	m_OutputFile.close();
	delete m_pInstance;
	m_pInstance = nullptr;
}