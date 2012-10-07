#include "GPSconverter.h"

/*assuming that latitude and longitude are stored in the x and z coordinates of a point */
double GPSconverter::GPSdistance(Vector3f p, Vector3f q)
{
	double h = haversine(q.x - p.x) + cos(p.x)*cos(q.x)*haversine(q.z - p.z); 
	return inverseHaversine(h);
}

double GPSconverter::haversine(double theta)
{
	return (1.0 - cos(theta))/2.0;
}

double GPSconverter::inverseHaversine(double h)
{
	return 2.0*EARTH_RADIUS_M * asin(sqrt(h));
}

double GPSconverter::findScaleDifference(Vector3f p, Vector3f q)
{
	double cirDist = GPSdistance(p,q);
	double coorDist = p.EuclDistance(q);
	return cirDist / coorDist;
}
