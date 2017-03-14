#include <cstdlib>
#include <ctime>
#include <string>

#include "ILPControl.h"
#include "Logger.h"

using namespace std;

int main(int argc, char** argv) 
{
	bool Success = true;
	
	//First set standard values, if input arguments exist, change to them
	//InputFileName is demanded, others are optional and have standard values in case they are not given by user
	const char* InputFileName = "";
	int LogLevel = 2;
	bool DumpToFile = false;
	string OutputFileName = "";

	//Check input parameters
	if (1 < argc)
	{
		InputFileName = argv[1];

		if (2 < argc)
		{
			//Returns zero also if there is no convertible Integer,
			//so a wrong input leads to LogLevel Debug, which is not a beautiful implementation but it works
			LogLevel = atoi(argv[2]);

			if (3 < argc)
			{
				string BoolString = argv[3];
				if (BoolString == "true")
				{
					DumpToFile = true;
				}

				if (4 < argc)
				{
					OutputFileName = argv[4];
				}
			}
		}
	}
	else 
	{
		//Do not know where to read from, InputFileName is demanded
		Success &= false;
	}

	//Get Logger instance
	Logger* iLogger = Logger::getLogger();
	iLogger->setLogLevel(LogLevel);
	iLogger->setDumpToFile(DumpToFile, OutputFileName);

	//Header
	iLogger->LogTrace("#################################################################");
	iLogger->LogTrace("##               ILPOptimizer | Tom Daniek | 2016              ##");
	iLogger->LogTrace("## Artificial Intelligence | Intelligent Systems Design | HSHL ##");
	iLogger->LogTrace("#################################################################");
	iLogger->LogTrace("");

	//Only do it if there is a name given for the InputFile
	if (Success)
	{
		//Start timestamp
		time_t StartingTime;
		time(&StartingTime);

		ILPControl* MainControl = new ILPControl();

		//Start ILPOptimizer
		if (MainControl->StartILP(InputFileName))
		{
			iLogger->LogTrace("");
			iLogger->LogTrace("## Everything was successful ##");
		}
		else
		{
			iLogger->LogTrace("");
			iLogger->LogTrace("## Seems that something went wrong ##");
		}

		//How many warnings/errors were found?
		iLogger->LogTrace("");
		iLogger->printAnalysis();
		iLogger->LogTrace("");

		//End timestamp
		time_t EndingTime;
		time(&EndingTime);

		//Calculate the time needed
		double seconds = difftime(EndingTime, StartingTime);
		iLogger->LogTrace("Needed " + to_string(seconds) + " seconds for whole computation.");
		iLogger->LogTrace("");

		delete MainControl;
	}
	else
	{
		iLogger->LogError("ILPOptimizer crashed at the beginning. Please give at least the name of the file you want to read from.");
	}

	//CleanUp
	iLogger->deleteLogger();

	//Wait for any key
	system("pause");

    return 0;
}