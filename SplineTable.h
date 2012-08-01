#ifndef SPLINE_PATH_TABLE_FOR_SHARK_SIM
#define SPLINE_PATH_TABLE_FOR_SHARK_SIM

#include <vector>
#include <cstdio>
using namespace std;

class splineTableEntry
{
	public:
	double u;  // the original u valueon the spline
	double dist; //the approximate distance down the spline, from the last sample point in the table. 
	double length; //u prime, the new u
};

class SplineTable
{
	public:
		SplineTable(){}
		~SplineTable(){}
		void addTableEntry(double u, double len){
			splineTableEntry g; 
			g.u = u; 
			g.length = len;
			bool success = false;
			//enforce that stable is ordered by u.
			for(vector<splineTableEntry>::iterator iv = sTable.begin(); iv != sTable.end(); iv++) {
				if(u < iv->u){
					sTable.insert(iv, g);
					success = true;
				}
			}
			if(!success){	
				sTable.push_back(g);
			}
		}

		splineTableEntry get(int index){return sTable.at(index);}
		double getU(int index){return sTable.at(index).u;}
		double getDist(int index){return sTable.at(index).dist;}
		double getLength(int index){return sTable.at(index).length;}
		void setDist(double ns, int index){sTable[index].dist = ns;}
		void setU(double newu, int index){sTable[index].u = newu;}
		void setLength(double uprime, int index){ sTable[index].length = uprime;}
		int getSize(){ return sTable.size();}

		/*return teh table index that is closest to a s value */
		int getIndexFromS(double s){
			double target = 0;
			int i = 0;
			while(target < s){
				target += getDist(i);
				i++;
				if(i >= sTable.size() && target < s){
					return -1;
				}
			}
			return i;
		}

		double calcDistance(double s1, double s2){ //TODO wrong
			int start =  getIndexFromS(s2);
		       	int end = getIndexFromS(s1);
			double dist = 0;
			while (start < end)
			{
				dist += getDist(start);
				start++;
			}
			return dist;
		}

		/*Given a u starting point, find a u' down the path a set distance away 
		 * return the (negative) distance travelled if the end point is reached before the goal is found,
		 * otherwise, return the (positive) u' value
		 * start1 needs to be >= 0. targetCoordDist must be positive */
		double getAheadU(double start1, double targetCoordDist)
		{
			double distanceTraveled = 0;
			int i = 0;
			int foundIndex = -1;
			bool startFound = false;

			for(i = 0; i < sTable.size(); i++) {
				if(!startFound && start1 < getLength(i) ) {
					startFound = true;
				}
				else if(startFound && distanceTraveled < targetCoordDist) {
					distanceTraveled += getDist(i);		
				}
				else if(startFound && distanceTraveled >= targetCoordDist) {
					foundIndex = i;
				        break;	
				}
			}
			//check to make sure foundIndex was really found. If it hasn't, that means that it is on the next point
			if(foundIndex < 0) {
				return -distanceTraveled; 
			}
			else
			{
				return getLength(foundIndex);
			}

		}

		/*Like getCloseU, but searches backwards on the spline.
		 * Still returns (negative) distanceTraveled if the spot isn't found
		 * return (positive) u' if it is found */
		double getBehindU(double start1, double targetCoordDist)
		{
			double distanceTraveled = 0;
			int i;  
			int foundIndex = -1;
			bool startFound = false;

			for(i = sTable.size()-1; i >= 0; i--) {
				if(!startFound && start1 > getLength(i) ) {
					startFound = true;
				}
				else if(startFound && distanceTraveled < targetCoordDist) {
					distanceTraveled += getDist(i);		
				}
				else if(startFound && distanceTraveled >= targetCoordDist) {
					foundIndex = i;
				        break;	
				}
			}
			//check to make sure foundIndex was really found. If it hasn't, that means that it is on the next point
			if(foundIndex < 0) {
				return -distanceTraveled; 
			}
			else
			{
				return getLength(foundIndex);
			}
			
		}

	private:
		vector<splineTableEntry> sTable;
};

#endif
