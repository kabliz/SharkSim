#ifndef SHARK_EXCEL_FILE_READER_IMPORTER
#define SHARK_EXCEL_FILE_READER_IMPORTER

#include "Vector.h"
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <float.h>

class EXEreader
{
	public:
		EXEreader(){minlat = DBL_MAX; minlong = DBL_MAX; maxlat = -DBL_MAX; maxlong = -DBL_MAX;}
		~EXEreader(){}
		void parseFile(const char* argv);

		Vector3f gCoordinate(int index){return latLong[index];}
		size_t size(){return latLong.size();}
		double gDTS(int index){return dts[index];}
		double gMinLatitude(){return minlat;}
		double gMaxLatitude(){return maxlat;}
		double gMinLongitude(){return minlong;}
		double gMaxLongitude(){return maxlong;}

	private:
		FILE* readFile;
		vector<Vector3f> latLong;
		vector<double> dts;
		double prevTime; //to calculate dt with
		double minlat;
		double minlong;
		double maxlat;
		double maxlong;
};

#endif
