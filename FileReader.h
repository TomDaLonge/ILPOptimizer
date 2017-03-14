#pragma once

#include <list>
#include <string>

class Logger;

using namespace std;

class FileReader {
public:
	FileReader(void);

	virtual ~FileReader();

	bool getLines(const char*	FileName,
				  list<string>& rLines);

private:
	Logger* m_pLogger;
};