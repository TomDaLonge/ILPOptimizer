#include "FileReader.h"

#include <fstream>
#include <algorithm>
#include <iostream>

#include "Logger.h"

FileReader::FileReader(void)
{
	m_pLogger = Logger::getLogger();
}

FileReader::~FileReader()
{
}

bool 
	FileReader::getLines(const char* FileName, 
						 list<string>& rLines)
{
	bool Success = true;

	//Create and open filestream
	ifstream InputFile(FileName);

	if (InputFile.is_open())
	{
		//Read every line
		while (!InputFile.eof())
		{
			string CurrentString;
			getline(InputFile, CurrentString);
			
			//Only store lines that are not empty or comment
			if (string::npos == CurrentString.find("//") && !CurrentString.empty())
			{
				bool LineFinished = false;

				while (!LineFinished)
				{
					//Remove whitespaces
					string::iterator EndWOSpaces = remove_if(CurrentString.begin(), CurrentString.end(), isspace);
					CurrentString = string(CurrentString.begin(), EndWOSpaces);

					//Check semicolon, if it is not there, also read the next line
					//Remove it
					size_t NumberOfSemic = count(CurrentString.begin(), CurrentString.end(), ';');
					if (1 == NumberOfSemic)
					{
						CurrentString = string(CurrentString.begin(), (CurrentString.end() - 1));
						LineFinished = true;
					}
					else if (0 == NumberOfSemic)
					{
						string nextLine;
						getline(InputFile, nextLine);
						CurrentString += nextLine;
					}
					else
					{
						m_pLogger->LogError("FileReader: Found a number of semicolons that were not zweo or one. Check the format of your file.");
					}
				}

				//Add to line list
				rLines.push_back(CurrentString);

				m_pLogger->LogDebug("FileReader: " + CurrentString);
			}	
		}
	}
	else
	{
		m_pLogger->LogError("FileReader: Failed to open File: '" + string(FileName) + "'");
		Success &= false;
	}

	InputFile.close();
	return Success;
}