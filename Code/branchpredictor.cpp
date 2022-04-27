#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

enum SingleRegister
{
	NOTTAKEN,
	TAKEN
};

enum DoubleRegister
{
	STRONGLYTAKEN,
	WEAKLYTAKEN,
	WEAKLYNOTTAKEN,
	STRONGLYNOTTAKEN
};

struct Global
{
	unsigned int b : 6;
};

int k = 256;


void doublebit_h(string line, int table_size, DoubleRegister *table, long &correct)
{
	long addr;
	string line_long = line.substr(0, 7);
	addr = stoul(line_long, nullptr, 16);
	addr = addr % table_size;
	DoubleRegister *h = &(table[addr]);

	if (*h == STRONGLYTAKEN)
	{
		if (line[7] == 'T')
			correct++;
		else
		{
			*h = WEAKLYTAKEN;
		}
	}
	else if (*h == WEAKLYTAKEN)
	{
		if (line[7] == 'T')
		{
			correct++;
			*h = STRONGLYTAKEN;
		}
		else
			*h = WEAKLYNOTTAKEN;
	}
	else if (*h == WEAKLYNOTTAKEN)
	{
		if (line[7] == 'N')
		{
			correct++;
			*h = STRONGLYNOTTAKEN;
		}
		else
			*h = WEAKLYTAKEN;
	}
	else
	{
		if (line[7] == 'N')
			correct++;
		else
			*h = WEAKLYNOTTAKEN;
	}
}

void doublebit(vector<string> &list, fstream &outFile)
{
	long correct, total;
	int i = 256;
	vector<string>::iterator it;

	DoubleRegister table[i];
	for (int j = 0; j < i; j++)
		table[j] = STRONGLYNOTTAKEN;

	total = correct = 0;
	for (it = list.begin(); it != list.end(); it++)
	{
		doublebit_h(*it, i, table, correct);
		total++;
	}

	float notcorrect = (total-correct);
	cout << "Misprediction rate double bit predictor:" << (notcorrect/total)*100 <<"%";
	outFile << "Misprediction rate double bit predictor:" << (notcorrect/total)*100 <<"%";

	cout << endl;
	outFile << endl;
}

void corrolatingdoublebit_h(string line, Global &globalHistory, DoubleRegister *table, long &correct)
{
	string line_long = line.substr(0, 7);
	long addr, index;
	unsigned short ghr;
	DoubleRegister *h;

	addr = stoul(line_long, nullptr, 16);
	ghr = globalHistory.b;
	index = addr ^ ghr;
	index = index % (k*64);
	h = &(table[index]);

	if (*h == STRONGLYTAKEN)
	{
		if (line[7] == 'T')
		{
			correct++;
			globalHistory.b = globalHistory.b << 1;
			globalHistory.b = globalHistory.b | (1UL << 0);
		}
		else
		{
			*h = WEAKLYTAKEN;
			globalHistory.b = globalHistory.b << 1;
			globalHistory.b = globalHistory.b & ~(1UL << 0);
		}
	}
	else if (*h == WEAKLYTAKEN)
	{
		if (line[7] == 'T')
		{
			correct++;
			*h = STRONGLYTAKEN;
			globalHistory.b = globalHistory.b << 1;
			globalHistory.b = globalHistory.b | (1UL << 0);
		}
		else
		{
			*h = WEAKLYNOTTAKEN;
			globalHistory.b = globalHistory.b << 1;
			globalHistory.b = globalHistory.b & ~(1UL << 0);
		}
	}
	else if (*h == WEAKLYNOTTAKEN)
	{
		if (line[7] == 'N')
		{
			correct++;
			*h = STRONGLYNOTTAKEN;
			globalHistory.b = globalHistory.b << 1;
			globalHistory.b = globalHistory.b & ~(1UL << 0);
		}
		else
		{
			*h = WEAKLYTAKEN;
			globalHistory.b = globalHistory.b << 1;
			globalHistory.b = globalHistory.b | (1UL << 0);
		}
	}
	else
	{
		if (line[7] == 'N')
		{
			correct++;
			globalHistory.b = globalHistory.b << 1;
			globalHistory.b = globalHistory.b & ~(1UL << 0);
		}
		else
		{
			*h = WEAKLYNOTTAKEN;
			globalHistory.b = globalHistory.b << 1;
			globalHistory.b = globalHistory.b | (1UL << 0);
		}
	}
}

void corrolatingdoublebit(vector<string> &list, fstream &outFile)
{
	long correct, total;
	DoubleRegister pTable[k*64];
	vector<string>::iterator it;

	for (int i = 0; i < (k*64); i++)
		pTable[i] = STRONGLYNOTTAKEN;

	Global globalHistory = {0};
	total = correct = 0;
	for (it = list.begin(); it != list.end(); it++)
	{
		corrolatingdoublebit_h(*it, globalHistory, pTable, correct);
		total++;
	}

	float notcorrect = (total-correct);
	cout << "Misprediction rate corrolating predictor (n=2):" << (notcorrect/total)*100 <<"%";
	outFile << "Misprediction rate corrolating predictor (n=2):" << (notcorrect/total)*100 <<"%";

	cout << endl;
	outFile << endl;
}

void corrolatingsinglebit_h(string line, Global &globalHistory, SingleRegister *table, long &correct)
{
	string line_long = line.substr(0, 7);
	long addr, index;
	unsigned short ghr;
	SingleRegister *h = &(table[addr]);

	addr = stoul(line_long, nullptr, 16);
	ghr = globalHistory.b;
	index = addr ^ ghr;
	index = index % (k*64);
	h = &(table[index]);

	if (*h == TAKEN)
	{
		if (line[7] == 'T')
		{
			correct++;
			globalHistory.b = globalHistory.b << 1;
			//globalHistory.b = globalHistory.b | (1UL << 0);
		}
		else
		{
			*h = NOTTAKEN;
			globalHistory.b = globalHistory.b << 1;
			//globalHistory.b = globalHistory.b & ~(1UL << 0);
		}
	}
	else
	{
		if (line[7] == 'N')
		{
			correct++;
			globalHistory.b = globalHistory.b << 1;
			//globalHistory.b = globalHistory.b | (1UL << 0);
		}
		else
		{
			*h = TAKEN;
			globalHistory.b = globalHistory.b << 1;
			//globalHistory.b = globalHistory.b & ~(1UL << 0);
		}
	}
	
}

void corrolatingsinglebit(vector<string> &list, fstream &outFile)
{
	long correct, total;
	SingleRegister pTable[k*64];
	vector<string>::iterator it;

	for (int i = 0; i < (k*64) ; i++)
		pTable[i] = NOTTAKEN;

	Global globalHistory = {0};
	total = correct = 0;
	for (it = list.begin(); it != list.end(); it++)
	{
		corrolatingsinglebit_h(*it, globalHistory, pTable, correct);
		total++;
	}

	float notcorrect = (total-correct);
	cout << "Misprediction rate corrolating predictor (n=1):" << (notcorrect/total)*100 <<"%";
	outFile << "Misprediction rate corrolating predictor (n=1):" << (notcorrect/total)*100 <<"%";

	cout << endl;
	outFile << endl;
}

void Single_h(string line, int table_size, SingleRegister *table, long &correct)
{
	long addr;
	string line_long = line.substr(0, 7);
	addr = stoul(line_long, nullptr, 16);
	addr = addr % table_size;
	SingleRegister *h = &(table[addr]);

	if (*h == TAKEN)
	{
		if (line[7] == 'T')
		{
			correct++;
		}
		else
		{
			*h = NOTTAKEN;
		}
	}
	else
	{
		if (line[7] == 'N')
		{
			correct++;
		}
		else
		{
			*h = TAKEN;
		}
	}
}

void Single(vector<string> &list, fstream &outFile)
{
	long correct, total;
	SingleRegister *h;
	int i = 256;
	bool result = false;

	SingleRegister table[i];
	for (int j = 0; j < i; j++)
		table[j] = NOTTAKEN;
	vector<string>::iterator it;

	correct = total = 0;
	for (it = list.begin(); it != list.end(); it++)
	{
		Single_h(*it, i, table, correct);
		total++;
	}

	float notcorrect = (total-correct);
	cout << "Misprediction rate single bit predictor:" << (notcorrect/total)*100 <<"%";
	outFile << "Misprediction rate single bit predictor:" << (notcorrect/total)*100 <<"%";

	cout << endl;
	outFile << endl;
}


int main(int argc, char const *argv[])
{
	vector<string> list;

	fstream inFile, outFile;
	inFile.open(argv[1], ios::in);
	outFile.open("output.txt", ios::out | ios::app);

	string line;
	while (getline(inFile, line))
	{
		list.push_back(line);
	}
	inFile.close();
	Single(list, outFile);
	doublebit(list, outFile);
	corrolatingsinglebit(list, outFile);
	corrolatingdoublebit(list, outFile);
	outFile.close();

	return 0;
}
