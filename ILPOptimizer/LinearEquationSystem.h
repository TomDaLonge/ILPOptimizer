#pragma once

#include <list>
#include <map>

class Logger;

using namespace std;

class LinearEquationSystem {
public:
	LinearEquationSystem(void);

	~LinearEquationSystem(void);


	//Setters & Change Functions
	//Change functions check whether the object to change is inside the dimensions, 
	//which brings additional security. When creating new Values outside the range, use normal setters.

	void setIsMin(bool isMin);

	void setObjFunctRow(int OFRow);

	//Rows
	void setObjFuncParams(map<int, double>& rMulitplicators,
						  int CostrCount);

	bool addConstrParams(map<int, double>& rMulitplicators);

	bool setConstrRow(map<int, double>& rowValues,
				int RowNr);

	bool changeRow(map<int, double>& rowValues,
				   int RowNr);

	bool isObjFunctRow(int RowNr);

	

	//Columns
	void setColumn(map<int, double>& columnValues,
				   int ColNr);

	bool changeColumn(map<int, double>& columnValues, 
				      int ColNr);

	//Values
	void setValue(double value,
				  int x,
				  int y);

	bool changeValue(double value, 
					 int x, 
					 int y);


	//Getter
	bool getIsMin(void);

	int getObjFunctRow(void);

	int getNumberOfConstrs(void);

	bool getRow(int rowNr, 
				map<int, double>& rRow);

	bool getColumn(int columnNr, 
				   map<int, double>& rColumn);

	double getValue(int Row, int Column);

	//Positions
	int getMaxPosFromRow(int row);

	int getMinPosFromConstrCol(int col);

	int getMinPosWPositiveValueFromConstrCol(map<int, double>& rColumn);
	
	//Dimensions
	int getHeight(void);

	int getWidth(void);


	//Actions

	bool checkDimensions(void);

	bool addSlackVariables(void);

	bool addSlackVarsToObjFunct(void);

	bool addSlackVarsToConstraints(void);

	bool divideColumns(int counter,
					   int denominator,
					   map<int, double>& rResultColumn);

	bool divideRowByValue(int row,
						  double value);

	bool subtractRowByMultOfAnother(int ActionRowNr,
									int PivotRowNr,
									int PivotColumnNr);

	bool IsObjFunctLessOrEqualZero(void);

	bool TransposeMatrix(void);


	//Print-Function
	void printMatrix(void);

private:
	bool mIsMin;

	int mObjFunctRow;

	//value in map with key size(list) is right side param,
	map <int, map <int, double> > mValueMatrix;

	Logger* m_pLogger;
};