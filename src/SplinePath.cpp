#include "SplinePath.h"



const float SplinePath::Mcat[] = {-.5, 1.5, -1.5, .5, 1, -2.5, 2, -.5, -.5, 0, .5, 0, 0, 1, 0, 0};
const float SplinePath::Mher[] = {2, -2, 1, 1, -3, 3, -2, -1, 0, 0, 1, 0, 1, 0, 0, 0};




double SplinePath::doubleLerp(double input, double minx, double maxx, double miny, double maxy)
{
	return miny + (input-minx) * ((maxy-miny)/(maxx-minx));
}

void SplinePath::deleteHeap()
{
	for(int i = 0; i < paramTable.size(); i++)
	{
		delete paramTable[i];
	}
}

void SplinePath::initSpline(string filename){    //reads .txt, .mat or .csv data sheets
	char end[3] = { filename.at(filename.size()-4),
		filename.at(filename.size()-3),
		filename.at(filename.size()-2) };
	if(end[1] == 't' || end[1] == 'T' ) {
		initSplineZOE(filename);
	}
	else if(end[1] == 'm' || end[1] == 'M' ) {
		initSplineMAT(filename);
	}
	else if (end[1] == 'c' || end[1] == 'C'){
		initSplineEXE(filename);
	}
	else {
		printf("Spline file does not correspond to a supported file type: %c%c%c \n", end[1], end[2], end[3]);
	}
}


/*These functions will initialize the Spline Path given a filename to a file */
void SplinePath::initSplineMAT(string filename)
{
	parseDataFileMAT(filename.c_str());
	gatherDTPoints();
	paramaterizeSpline();
}

void SplinePath::initSplineEXE(string filename)
{
	parseDataFileEXE(filename.c_str());
	gatherEXPoints();
	paramaterizeSpline();
}

void SplinePath::initSplineZOE(string filename)
{
	zoereader = ZOEreader();
	parseDataFileZOE(filename.c_str());
	gatherZOEPoints();
	paramaterizeSpline();
}





void SplinePath::gatherDTPoints()
{
	Vector3f p, q;
	double dt = 0;
	float runningTotts = 0; //running time totals per spot
	float prevRT = 0;
	//printf("data width %d\n", mreader.width());
	for(int id = 0; id < mreader.length(); id++)
	{

		dt += mreader.gElement(0,id);
		runningTotts += dt;
		p.x = mreader.gElement(1,id);
		p.y = .2 * sin(id/8.0);   	//varies the height a little over each point
		p.z = mreader.gElement(2,id);

		if(!(p.x == q.x && p.z == q.z))
		{
			//p.Print();
			isLargerPoint(p);
			isSmallerPoint(p);
			points.push_back(p);
			dts.push_back(dt);
			totts.push_back(prevRT);
			prevRT = runningTotts;
			dt = 0;

		}
		q = p;
	}
	calcRadius();

	initTangents();
}

//csv
void SplinePath::gatherEXPoints()
{
	float runningTotts = 0;  //running time total
	Vector3f sc1 = ereader.gCoordinate(0);
	Vector3f sc2 = ereader.gCoordinate(ereader.size()-1);
	int i = 1;
	while(sc1.fEquals(sc2, 0.001)){  //program will NaN out if these points are the same
		sc2 = ereader.gCoordinate(i);
		i++;	
	}
	double scale = GPSconverter().findScaleDifference(sc1, sc2);
	for(int id = 0; id < ereader.size(); id++)
	{
		Vector3f p = ereader.gCoordinate(id);
		p = p * scale ;
		//p.x = doubleLerp(ereader.gCoordinate(id).x, ereader.gMinLatitude(), ereader.gMaxLatitude(), 0, 100);
		//p.y = doubleLerp(ereader.gCoordinate(id).y, ereader.gMinLongitude(), ereader.gMinLongitude(), 0 , 100) ;
		//p.z = ereader.gCoordinate(id).z;
		//p.Print();
		//printf("\n");

		if(p.fEquals(points[points.size()-1], .00001 * scale)) {
			isLargerPoint(p);
			isSmallerPoint(p);
			points.push_back(p);
			dts.push_back(ereader.gDTS(id));
			totts.push_back(runningTotts+ereader.gDTS(id));
			runningTotts += ereader.gDTS(id);
		}

		//TODO throw out duplicate points
	}
	//TODO clarify EXEreader input
	calcRadius();

	initTangents();
}

void SplinePath::gatherZOEPoints()
{
	sGhostPoints(true);
	float runningTotts = 0;  //running time total
	for(int id = 0; id < zoereader.size(); id++)
	{
		Vector3f p = zoereader.gCoordinate(id);
		//p.x = doubleLerp(ereader.gCoordinate(id).x, ereader.gMinLatitude(), ereader.gMaxLatitude(), 0, 100);
		//p.y = doubleLerp(ereader.gCoordinate(id).y, ereader.gMinLongitude(), ereader.gMinLongitude(), 0 , 100) ;
		//p.z = ereader.gCoordinate(id).z;

		isLargerPoint(p);
		isSmallerPoint(p);
		points.push_back(p);
		dts.push_back(zoereader.gDTS(id));
		totts.push_back(runningTotts+zoereader.gDTS(id));
		runningTotts += zoereader.gDTS(id);
	}
	calcRadius();


	initTangents();
}

/*notes upper corner of world while parsing */
void SplinePath::isLargerPoint(Vector3f cur)
{
	if(cur.x > maxPt.x)
	{
		maxPt.x = cur.x;
	}
	if(cur.y > maxPt.y)
	{
		maxPt.y = cur.y;
	}
	if(cur.z > maxPt.z)
	{
		maxPt.z = cur.z;
	}
}

/*Notes lower corner of world while parsin */
void SplinePath::isSmallerPoint(Vector3f cur)
{
	if(cur.x < minPt.x)
	{
		minPt.x = cur.x;
	}
	if(cur.y < minPt.y)
	{
		minPt.y = cur.y;
	}
	if(cur.z < minPt.z)
	{
		minPt.z = cur.z;
	}
}

/*Used to calculate the size of the whole world */
void SplinePath::calcRadius()
{
	midPt = maxPt - minPt;
	radius = (maxPt-midPt).Magnitude();
}

void SplinePath::initTangents()
{
	tTangents = vector<Vector3f>();
	tangents = vector<Vector3f>();

	if(!isCatmullMode)
	{
		//first value needs to be initilized 
		Vector3f tan = (points[1]-(points[2]-points[1]) - points[0]) *.9;
		tangents.push_back(tan);

		//loop over each of the middle points
		for(int i = 1; i < points.size()-1; i++)
		{
			//first create the direction of the tangent. then scale it to the needed magnitude
			Vector3f postPt = points[i-1];  //posterior
			Vector3f antPt = points[i+1];   //anterior
			Vector3f tarPt = points[i];     //center target
			Vector3f before = tarPt - postPt;
			before.y = 0;
			Vector3f after = tarPt - antPt;
			after.y = 0;
			before /= before.Magnitude();
			after /= after.Magnitude();
			Vector3f edgeDiff;	

			if(before.Dot(after) >= .8 )
			{
				//case: need perpendicular tangent
				//180 degree turn case is discontinious. This is a fix
				//correct 180 case before cross multiplying
				int j = 3;
				while((before.Dot(after) >= .9999)){// || (points[i].fEquals(points[i+j], 0.01))) {
					if(j >= points.size())
					{
						after = Vector3f(0,0,1);  //patch for broken spline case 
						break;
					}
					antPt = points[i+j];
					after = tarPt - antPt;
					after.y = 0;
					j++;
				}
				Vector3f axis;
				axis = before.Cross(after);   //axis of rotate
				Vector3f perpen = before.Cross(axis);  //right angle turn.	
				if(perpen.Dot(after) < 0 )
				{
					perpen *= -1;  //perpen should point in same general direction as the next point
				}
				edgeDiff = perpen;
				edgeDiff /= edgeDiff.Magnitude();
			}
			else{  //case: need a tangent that points to the next location
				edgeDiff = antPt-postPt;  //chord
				edgeDiff /= edgeDiff.Magnitude();	
			}	

			float rotationAngle = tarPt.AngleCos(before, after);  

			//scale magnitude
			float postMag = (tarPt-postPt).Magnitude(); 
			float antMag = (tarPt-antPt).Magnitude() ;
			float Mag = postMag < antMag? postMag : antMag;  //magnitude of shortest distance to point
			Mag /= 2.0;
			float minMag = .2;   //minimum magnitude for the thing.
			float scale = minMag + ((Mag-minMag) * (((3.14159265-rotationAngle)*(3.14159265-rotationAngle)) / 3.14159265 )); //Lerp ease scale

			//tangent calculation
			tan = edgeDiff * scale;
			if(!tan.fEquals(tan, 0.1))  //NaN bandAid TODO
			{
				tan = Vector3f(0,0,2);
			}
			//tan.Print(); 
			tangents.push_back(tan);
		}

		//init the last point
		tan = points[points.size()-1] - points[points.size()-2];
		tangents.push_back(tan);
	}

	//Fritsch-Carlson
	float hm1 = 0.0;  //d_{i-1}
	float hm1x = 0.0;  //d_{i-1}
	float dm1 = 0.0; //h_{i-1}
	float dm1x = 0.0; //h_{i-1}

	Vector3f endTangent = Vector3f(0, 0, 0);

	tTangents.push_back( endTangent );  //tangent at first point
	//Time-space tangent generation
	for(int i = 1; i < points.size()-1; i++)
	{
		Vector3f ttan;
		//first create the direction of the tangent. then scale it to the needed magnitude
		Vector3f antPt = Vector3f(totts[i+1], 0, i+1);   //anterior
		Vector3f tarPt = Vector3f(totts[i],   0, i);   //center target

		float hi = antPt.x - tarPt.x;
		float di = (antPt.z - tarPt.z)/hi;

		float yTan;
		if((dm1 > 0 && di > 0) || (dm1 < 0 && di < 0) || (dm1 == 0 && dm1 == di) )  //same sign
		{
			yTan = 3.0*(hm1 + hi);
			yTan *= (1.0 / (((2.0*hi+hm1)/dm1) + ((2.0*hm1+hi)/di) ));
		}
		else
		{
			yTan = 0;
		}

		float hix = antPt.z - tarPt.z;
		float dix = (antPt.x - tarPt.x)/hix;

		float xTan;
		if((dm1x > 0 && dix > 0) || (dm1x < 0 && dix < 0) || (dm1x == 0 && dm1x == dix) )  //same sign
		{
			xTan = 3.0*(hm1x + hix);
			xTan *= (1.0 / (((2.0*hix+hm1x)/dm1x) + ((2.0*hm1x+hix)/dix) ));
		}
		else
		{
			xTan = 0;
		}

		ttan = Vector3f(xTan, 0, yTan);
		//ttan.Print(); 
		tTangents.push_back(ttan);
		hm1 = hi;
		hm1x = hix;
		dm1 = di;
		dm1x = dix;
	}

	//init the last point
	tTangents.push_back(endTangent);


}


/*Interpolation helper function. Computs the matrix multiplcation between a u matrix, M (spline matrix) and B (points)
 * returns a single result. Run three times to get x, y, and z (full vertex) */
double SplinePath::HmInt(float U[4], const float M[16], float B[4])
{
	float UM[4];
	UM[0] = U[0]*M[0] + U[1]*M[4] + U[2]*M[8] + U[3]*M[12];
	UM[1] = U[0]*M[1] + U[1]*M[5] + U[2]*M[9] + U[3]*M[13];
	UM[2] = U[0]*M[2] + U[1]*M[6] + U[2]*M[10] + U[3]*M[14];
	UM[3] = U[0]*M[3] + U[1]*M[7] + U[2]*M[11] + U[3]*M[15];

	return UM[0]*B[0] + UM[1]*B[1] + UM[2]*B[2] + UM[3]*B[3];
}



/*matrix spline interpolation
 * * returns a vector location 
 * * It is a wrapper around catmullMatrix()
 * * The arc length u is converted into the spline's true u while interpolating*/
Vector3f SplinePath::splineLocation(float curLocation, int startPoint)
{
	if(tabSet) {  //the very first runs of this function creates the table. So the u should not be converted
		curLocation = StoU(curLocation, startPoint); //convert arcLength based u into the spline's true u.
	}
	if(isCatmullMode)
	{
		return catmullMatrix(curLocation, startPoint);
	}
	else
	{
		int endPoint = startPoint + 1;   //TODO found this line. dangerous. set endPoint properly
		return hermiteMatrix(curLocation, points[startPoint], points[endPoint],
				tangents[startPoint], tangents[endPoint]);
	}
}

// interpolation for the time of movement (time-space curve, rather than the space curve of the other function)
// Returns a double value representing the new U value to input into the space curve
// takes in the amount of time (seconds) since program start, and the current Knot.
double SplinePath::convertTimestampToU(float timer, int curKnot )
{
	int endMark = totts.size();
	int endLocNum; //index to the end point;
	Vector3f startTan;
	Vector3f startTime;
	Vector3f endTime;
	Vector3f endTan;

	startTime = Vector3f(totts[curKnot], 0, curKnot);
	startTan = tTangents[curKnot];
	if(curKnot+1 >= endMark) {
		endLocNum = 0;
	}
	else {
		endLocNum = curKnot+1;
	}

	endTime = Vector3f(totts[endLocNum], 0, endLocNum); //provided it's not at the end of the spline, endLocation is start+1
	endTan = tTangents[endLocNum];

	
	float timeU = doubleLerp(timer, totts[curKnot], totts[endLocNum], 0.0, 1.0);
	float interpolated =  hermiteMatrix(timeU, startTime, endTime, startTan, endTan).z;
	return doubleLerp(interpolated, curKnot, endLocNum, 0.0, 1.0);  //convert interpolated between zero and one.
	//return doubleLerp(res, startTime, endTime, 0.0, 1.0);

}

/*igeneralized catmull-rom matrix multiplcation for complex interpolations
 * returns a vector
 * curLocation is a number between zero and one, and represents percentage of progress
 * See a math reference for how this works. */
Vector3f SplinePath::hermiteMatrix(float curLocation, Vector3f startLocation, Vector3f endLocation,
		Vector3f startTangent, Vector3f endTangent)
{
	Vector3f res = Vector3f();

	float dU[4] = {curLocation*curLocation*curLocation, curLocation*curLocation, curLocation, 1 };

	float Bx[4] = {startLocation.x, endLocation.x, startTangent.x, endTangent.x};
	float By[4] = {startLocation.y, endLocation.y, startTangent.y, endTangent.y};
	float Bz[4] = {startLocation.z, endLocation.z, startTangent.z, endTangent.z};

	res.x =  HmInt(dU, Mher, Bx);
	res.y =  HmInt(dU, Mher, By);
	res.z =  HmInt(dU, Mher, Bz);
	return res;
}

/*Given a curLocation (u) and a point to start at, find a catmull interpolation of a point's actual location past the point (knot) */
Vector3f SplinePath::catmullMatrix(float curLocation, int curKnot) 
{
	int endMark = points.size();
	int endLocNum; //index to the end point;
	Vector3f historyLocation;
	Vector3f startLocation;
	Vector3f endLocation;
	Vector3f futureLocation;

	//Need to derive three nighboring points next to the current point (one behind, two ahead). 
	//Array bounds need to be checked, and they wrap.   
	if(curKnot > 0) {

		historyLocation = points[curKnot-1];  //not at beginning of spline
	}
	else {
		historyLocation = (points[curKnot+1]*.5) - (points[curKnot+2]-points[curKnot+1]) - points[curKnot]; //initial tangent at beginning of spline
	}

	startLocation = points[curKnot];

	if(curKnot+1 >= endMark) {
		endLocNum = 0;
	}
	else {
		endLocNum = curKnot+1;
	}

	endLocation = points[endLocNum]; //provided it's not at the end of the spline, endLocation is start+1
	if(endLocNum+1 >= endMark) {
		futureLocation = points[0];
	}
	else {
		futureLocation = points[endLocNum+1];
	}


	//Prepare the matrices used for catmull interpolation
	Vector3f res = Vector3f();
	float dU[4] = {curLocation*curLocation*curLocation, curLocation*curLocation, curLocation, 1};

	float Bx[4] = {historyLocation.x, startLocation.x, endLocation.x, futureLocation.x};
	float By[4] = {historyLocation.y, startLocation.y, endLocation.y, futureLocation.y};
	float Bz[4] = {historyLocation.z, startLocation.z, endLocation.z, futureLocation.z};

	res.x =  HmInt(dU, Mcat, Bx);
	res.y =  HmInt(dU, Mcat, By); //this spline does not reflect y axis 
	res.z =  HmInt(dU, Mcat, Bz);
	return res;
}




//Reparamaterizes the spline so that the u value used for matrix interpolation represents distance over the xyz coordinate units.
//Both the original u, the reparamaterized u (u prime), and the coordinate distance is stored in a table.
//The way RParent talks about length is confusing. In adaptive gaussian integration, the u prime value is referred to as length. It is not the coordinate distance length. 
void SplinePath::paramaterizeSpline()
{
	//for each point (except the last) on the original path, record the table inbetween each set of knots.
	for(int i = 0; i < points.size()-1; i++)
	{
		//create temp curve structure, feed it into the algorithm
		cubicCurveTd curArc;	

		//Abandoning adaptive gaussian integration, it is buggy and I am trying something simpler. AGI returns values greater than 1.
		//It is not supposed to. 
		//solving for space curve. reference Christopher Twigg paper
		/*double x1, z1, x2, z2, x3, z3, x4, z4;
		  Vector3f one, two, three, four;
		  double tau = .5000000;

		  if(isCatmullMode)
		  {
		  Vector3f initPt = (points[i+1]*.5) - (points[i+2]-points[i+1]) - points[i];
		  int endPt = i+1;
		  int futPt = i+2;

		  if (i == points.size()-1) {
		  endPt = 0;
		  }
		  if(i == points.size() -2) {
		  futPt = 0;
		  }
		  else {
		  futPt = endPt + 1;
		  }

		  one = i > 0 ? points[i-1]: initPt;
		  two = points[i];
		  three = points[endPt]; //i > 1 ? points[i-2].z: points[points.size()-2].z;
		  four = points[futPt]; 
		  Vector3f tan1 = (three - one)/2;
		  Vector3f tan2 = (four - two)/2;
		  Vector3f res1 = (three-two)*3 - tan1*2 - tan2;
		  Vector3f res2 = (two-three)*2 + tan1 + tan2;

		  curArc.dx = res2.x;
		  curArc.dy = res2.z;
		  curArc.cx = res1.x;
		  curArc.cy = res1.z; 
		  curArc.bx = tan1.x;
		  curArc.by = tan1.z; 
		  curArc.ax = two.x;
		  curArc.ay = two.z;
		  }
		  else //hermite mode?
		  {
		  one = points[i];
		  two = points[i+1]; //i > 0 ? points[i-1].z: points[points.size()-1].z;
		  three = tangents[i];
		  four = tangents[i+1]; 

		  curArc.dx = one.x*Mher[0] + two.x*Mher[1] + three.x*Mher[2] + four.x*Mher[3];
		  curArc.dy = one.z*Mher[0] + two.z*Mher[1] + three.z*Mher[2] + four.z*Mher[3];
		  curArc.cx = one.x*Mher[4] + two.x*Mher[5] + three.x*Mher[6] + four.x*Mher[7];
		  curArc.cy = one.z*Mher[4] + two.z*Mher[5] + three.z*Mher[6] + four.z*Mher[7];
		  curArc.bx = one.x*Mher[8] + two.x*Mher[9] + three.x*Mher[10] + four.x*Mher[11];
		  curArc.by = one.z*Mher[8] + two.z*Mher[9] + three.z*Mher[10] + four.z*Mher[11];
		  curArc.ax = one.x*Mher[12] + two.x*Mher[13] + three.x*Mher[14] + four.x*Mher[15];
		  curArc.ay = one.z*Mher[12] + two.z*Mher[13] + three.z*Mher[14] + four.z*Mher[15];
		  }*/


		//create next table to fill
		SplineTable *curTab = new SplineTable();
		paramfuncs = ParamFunctions(curTab);
		//Abandoning AGI. Now calculating U' after coordinate based distances are mapped to U
		//double oldk = 0;
		for(int kount = 0; kount < totalSlices; kount++) //divide the curve into 10 parts between each knot. 
			//Store their arc lengths (in u, not coords) in table
		{
			double kk = kount*(1.0/totalSlices);
			//double arclen = paramfuncs.adaptiveIntegration(&curArc, oldk, kk, Tolerance); //arclen supposed to be [0,1]. It is not
			//oldk = kk;
			curTab->addTableEntry(kk, -1);
		}

		double size;
		Vector3f lastPoint = points[i]; //= splineLocation(curTab->getU(0), i);	
		double totalDistance = 0; //the total coord distance to do linear interpolation over, to get u'
		//Setting distances in unit coordinates, in addition to saving the u values -li-ke-- ab-o-ve- like below.
		for(int r =0; r < totalSlices; r++){
			if(!r){
				size = 0;
			}
			else{
				Vector3f thisPoint = splineLocation(curTab->getU(r-1), i);
				size = (thisPoint - lastPoint).Magnitude(); 
				totalDistance += size;
				lastPoint = thisPoint;
			}
			curTab->setDist(size, r); //place coordinate distance into tables.
		}

		//and now to finally calculate u prime, the paramaterized version of u.
		size = 0;
		lastPoint = points[i];
		curTab->setLength(0, 0); //first value is trivial.
		for(int r = 1; r < totalSlices; r++) {
			//do this again to get the current size
			Vector3f thisPoint = splineLocation(curTab->getU(r-1), i);
			size += (thisPoint - lastPoint).Magnitude();

			//this time, interpolate linearly to totalDistance. start interval [0, totalDistance]. end interval [0,1] 
			double result = doubleLerp(size, 0, totalDistance, 0.0, 1.0);
			curTab->setLength(result, r); //the way RParent talks about length makes it confusing. setLength is setting u prime, not the coordinate length.
			//printf("%f %f \n", curTab->getU(r), curTab->getDist(r), result);
			lastPoint = thisPoint;
		}

		paramTable.push_back(curTab);

	}
	tabSet = true;
}

/*gets a point a certain (positive) distance away (in unit coordinate distance), 
 * it needs teh distance, the starting point and the current u position after the point  */
Vector3f SplinePath::getNearbyPoint(double distanceAhead, int startPoint, double startU)
{
	double curU = startU;
	double result = -1;
	int curPoint = startPoint;
	Vector3f nearbyPoint = points[startPoint];


	if(distanceAhead > 0) { //looking forward
		int i = 0;
		for(i = startPoint; i < paramTable.size(); i++) {
			result = paramTable[curPoint]->getAheadU(curU, distanceAhead);  
			if(result <= 0) {  //didn't find point yet, keep going
				curU = 0; //start at the beginning of the point from now on.
				distanceAhead = distanceAhead + result;  //this subtracts(!) the distance already traveled
				nearbyPoint = splineLocation(1,curPoint);
				curPoint++; 
			}
			else if(result > 0) {  // found the point, or as far as this can move
				nearbyPoint = splineLocation(result, curPoint);
				break;
			}
			//printf(" %f %f \n", result, distanceAhead);
		}
		if(i == paramTable.size()) { //point wasn't located. so it's at the end.
			nearbyPoint = splineLocation(1, i-1);
		}
	}
	//TODO investigate backwards anomoly when hitting the 1.000 table index on a point
	else if(distanceAhead < 0) { //looking backward 
		int i = 0;
		for(i = startPoint; i > 0 ; i--) {
			result = paramTable[curPoint]->getBehindU(curU, -distanceAhead);
			//printf("backwards %f %f (%f)\n", curU, result, distanceAhead);
			if(result <= 0) {  //didn't find point yet, keep going
				curU = 1; //start at the end of the point from now on.
				distanceAhead = distanceAhead - result;  //this subtracts(!) the distance already traveled. both nums negative
				nearbyPoint = splineLocation(0, curPoint);
				curPoint--; 
			}
			else if(result > 0) {  // found the point, or as far as this can move
				nearbyPoint = splineLocation(result, curPoint);
				break;
			}
		}
		if(i == 0) { //reached the very start without finding the point
			nearbyPoint = splineLocation(0,0);
		}
	}


	return nearbyPoint;
}

/*S to U: Find the value for the space curve u given a distance value between zero and one, between the current point and the next point */
double SplinePath::StoU(double sDist, int curPoint)
{
	//Pthis = Plast - f(Plast)/f'(plast);
	//f = s - LENGTH(u1, U(plast)) = 0;
	//U = parametric value of point on paramaterized curve

	//find the first arclength value higher than the target distance, then try and linearlly interpolate between them
	//really this s to U conversion is an approximation

	int i = -1;
	double aDist = 0;
	while((i<totalSlices-1) && (aDist <= sDist + .000001))	
	{
		i++;
		aDist = paramTable[curPoint]->getLength(i);
		if(aDist >= sDist-.0000001 && aDist <= sDist+.0000001)
		{
			return paramTable[curPoint]->getU(i); //exact match case
		}
	}
	int h = i-1;  //h is the last relevant distance, i is the next one.
	double simpleU = 0;
	if(h >= 0)
	{
		//interpolate between values in the table, since sDist is probably between table points more oftern than not. 
		simpleU = doubleLerp(sDist, paramTable[curPoint]->getLength(h), paramTable[curPoint]->getLength(i),
				paramTable[curPoint]->getU(h), paramTable[curPoint]->getU(i));
	}
	else //first entry on table being interpolated
	{
		simpleU = doubleLerp(sDist, 0, paramTable[curPoint]->getLength(i),
				0, paramTable[curPoint]->getU(i));
	}
	return simpleU;
}

/*Draws line between the previous line and the current one */
void SplinePath::drawPointLine(int i, Frustum* frustum)
{
	glPushMatrix();
	{
		glTranslatef(gPoint(i).x, gPoint(i).y, gPoint(i).z);
		if(frustum->testPoint(gPoint(i)))
		{
			glutSolidSphere(.1, 3, 2);
		}
	}glPopMatrix();
	if(i > 0)
	{
		if(frustum->testPoint(gPoint(i)) || frustum->testPoint(gPoint(i-1)))
		{
			//splined points for more curvature
			Vector3f p1 = splineLocation(.1, i-1);
			Vector3f p2 = splineLocation(.2, i-1);
			Vector3f p3 = splineLocation(.3, i-1);
			Vector3f p4 = splineLocation(.4, i-1);
			Vector3f p5 = splineLocation(.5, i-1);
			Vector3f p6 = splineLocation(.6, i-1);
			Vector3f p7 = splineLocation(.7, i-1);
			Vector3f p8 = splineLocation(.8, i-1);
			Vector3f p9 = splineLocation(.9, i-1);

			glBegin(GL_LINES);
			glVertex3f(gPoint(i-1).x, gPoint(i-1).y, gPoint(i-1).z);
			glVertex3f(p1.x, p1.y, p1.z);

			glVertex3f(p1.x, p1.y, p1.z);
			glVertex3f(p2.x, p2.y, p2.z);

			glVertex3f(p2.x, p2.y, p2.z);
			glVertex3f(p3.x, p3.y, p3.z);

			glVertex3f(p3.x, p3.y, p3.z);
			glVertex3f(p4.x, p4.y, p4.z);

			glVertex3f(p4.x, p4.y, p4.z);
			glVertex3f(p5.x, p5.y, p5.z);

			glVertex3f(p5.x, p5.y, p5.z);
			glVertex3f(p6.x, p6.y, p6.z);

			glVertex3f(p6.x, p6.y, p6.z);
			glVertex3f(p7.x, p7.y, p7.z);

			glVertex3f(p7.x, p7.y, p7.z);
			glVertex3f(p8.x, p8.y, p8.z);

			glVertex3f(p8.x, p8.y, p8.z);
			glVertex3f(p9.x, p9.y, p9.z);

			glVertex3f(p9.x, p9.y, p9.z);
			glVertex3f(gPoint(i).x, gPoint(i).y, gPoint(i).z);
			glEnd();
		}
	}
}



void SplinePath::drawPoints(int curPoint, bool usesGhostPoints, Frustum* frustum)
{
	int step = 1;

	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHTING);
	float blue = 1.0;
	float red = 1.0;
	float green = 1.0;
	int i;
	int chunk = size()*.1 / 4;
	int endRange = limitedDrawing? curPoint + 500 : (curPoint+(size()*.1));
	int pastRange = limitedDrawing? curPoint - 100 : (usesGhostPoints? 1 : 0 );

	frustum->extract();



	for(i = curPoint+1; i < endRange; i += step)
	{
		glColor3f(red,green,blue);
		drawPointLine(i, frustum);
		if(i > curPoint+chunk) red -= .02;
		if(i > curPoint+(3.0*chunk)){ red += .02; green -=.01; }
		if(i < curPoint+(chunk*2)){ blue -= .02; }
		if(i > curPoint+(chunk*2)){ blue += .02; }

	}


	//Past points drawn in red
	glColor3f(1,0,0);
	i = pastRange;
	if(usesGhostPoints && pastRange < 1){
		i = 1;
	}
	else if(pastRange < 0){i = 0;}

	for(/*same val*/ ; i <= curPoint; i++)
	{
		drawPointLine(i, frustum);
	}

	//Far future points drawn in black, if not limiting Drawing
	if(!limitedDrawing)
	{
		glColor3f(0,0,0);
		for(i = (curPoint+(size()*.1)) ;i < (usesGhostPoints?size()-1: size()); i ++)
		{
			drawPointLine(i, frustum);
		}
	}
	glColor3f(1,1,1);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
}




















