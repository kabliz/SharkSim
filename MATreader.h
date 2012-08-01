#ifndef _MATLAB_FILE_READER_FOR_SHARKS_
#define _MATLAB_FILE_READER_FOR_SHARKS_

#include <cstdio>
#include <cstdlib>
#include <zlib.h>
#include <stack>
#include <vector>
using namespace std;

//endian swappers
#define SWAP_2(x) ( (((x) & 0xff) << 8) | ((unsigned short)(x) >> 8) )
#define SWAP_4(x) ( ((x) << 24) | \
         (((x) << 8) & 0x00ff0000) | \
         (((x) >> 8) & 0x0000ff00) | \
         ((x) >> 24) )
#define FIX_SHORT(x) (*(unsigned short *)&(x) = SWAP_2(*(unsigned short *)&(x)))
#define FIX_INT(x)   (*(unsigned int *)&(x)   = SWAP_4(*(unsigned int *)&(x)))
#define FIX_FLOAT(x) FIX_INT(x)


class MATreader
{

	public:
		MATreader(){numMatrix = 0;}
		~MATreader(){}
		void parseFile(const char* argc);		
	
		bool lilEndian;
		bool runninglilEndian;
		FILE* readfile;
		vector<vector<double> > pMatrices;
		int numMatrix;

	private:
		void checkErr(int chk, bool type_isnonzeroerror);
		bool checkNull(char *chk);
		void exitOnNull(char *chk);
		int charToInt(char c[5]);
		double charToDouble(char c[9]);
		Bytef* uncompressMatrix(unsigned int asize);
		void parseComMatrix(Bytef* buffer, int ptr, int size);
};

#endif
