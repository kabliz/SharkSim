#ifndef EARTH_GPS_TO_DISTANCE
#define EARTH_GPS_TO_DISTANCE

#include<cstdlib>
#include<cstdio>
#include<vector>
#include<cmath>
#include"Vector.h"

/*measures the distance in meters between two GPS coordinates */
/*For the shark application, the scaling difference is the ratio between the GPS coordinate and 
 *their distance.
* Only meant to be used for small distances. Assumes the earth is spherical. */
class GPSconverter
{
	public:
		GPSconverter(){}
		~GPSconverter(){}
		double GPSdistance(Vector3f p1, Vector3f p2);  //great-circle distance between two points
		double haversine(double theta);
		double inverseHaversine(double h);
		double findScaleDifference(Vector3f p, Vector3f q);   //return the ratio of greatCircle distance to coordinate distance 
		
		static float const EARTH_RADIUS_KM = 6371.0 ;    //average radius of the earth.
		static float const EARTH_RADIUS_M = 6371000.0 ;
};

#endif

