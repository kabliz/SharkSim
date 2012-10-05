#ifndef CAL_POLY_COMPUTER_SCIENCE_3D_READER
#define CAL_POLY_COMPUTER_SCIENCE_3D_READER

#include <float.h>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include "Vector.h"

/*reads cal poly 3d SPLINE DATA. Assumes all points are one second apart.
 *
 * File format read has three whitespace or comma separated values (representing one point) per line. 
 * It has no time data, so it is inferred to be one second. */
class ZOEreader
{
	public:
		ZOEreader(){minlat = DBL_MAX; minlong = DBL_MAX; maxlat = -DBL_MAX; maxlong = -DBL_MAX; minalt = DBL_MAX; maxalt = -DBL_MAX;} 
		~ZOEreader(){}
		void parseFile(const char* argc);
		void parseFile(){parseFile("CPInput.txt");}
		Vector3f gCoordinate(int i1){return latLong.at(i1);}
		size_t size(){return latLong.size();}
		double gDTS(int index){return dts[index];}
		double gMinLatitude(){return minlat;}
		double gMaxLatitude(){return maxlat;}
		double gMinLongitude(){return minlong;}
		double gMaxLongitude(){return maxlong;}
		double gMinAltitude(){return minalt;}
		double gMaxAltitude(){return maxalt;}


	private:
		FILE* readFile;
		vector<Vector3f> latLong;
		vector<double> dts;
		double minlat;
		double minlong;
		double maxlat;
		double maxlong;
		double minalt;
		double maxalt;

};

#endif
