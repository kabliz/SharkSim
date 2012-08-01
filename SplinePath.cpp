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

void SplinePath::gatherDTPoints()
{
	Vector3f p, q;
	double dt;
	for(int id = 0; id < mreader.pMatrices[0].size(); id++)
	{

		dt += mreader.pMatrices[0][id];
		p.x = mreader.pMatrices[1][id];
		p.y = 0;
		p.z = mreader.pMatrices[2][id];
		if(!(p.x == q.x && p.z == q.z))
		{
			isLargerPoint(p);
			isSmallerPoint(p);
			points.push_back(p);
			dts.push_back(dt);
			dt = 0;
		}
		q = p;
	}
	calcRadius();

	initTangents();
}

void SplinePath::gatherZPoints()
{
	for(int id = 0; id < ereader.latLong.size(); id++)
	{
		Vector3f p;
		p.x = doubleLerp(ereader.latLong[id].x, ereader.minlat, ereader.maxlat, 0, 100);
		p.y = doubleLerp(ereader.latLong[id].y, ereader.minlong, ereader.maxlong, 0 , 100) ;
		p.z = ereader.latLong[id].z;

		isLargerPoint(p);
		isSmallerPoint(p);
		points.push_back(p);
		dts.push_back(ereader.dts[id]);
		//printf("%f %f \n", p.x, p.y);
		//printf("%f %f \n", ereader.minlong, ereader.maxlong);
	}
	//TODO clarify EXEreader input
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
	//first value needs to be initilized 
	Vector3f tan = (points[1]-(points[2]-points[1]) - points[0]) *.5;
	tangents.push_back(tan);

	//loop over each of the middle points
	for(int i = 1; i < points.size()-1; i++)
	{
		Vector3f edgeDiff = points[i+1]-points[i-1];

		//180 degree turn case is discontinious. This is a fix
		int j = 2;
		while(edgeDiff.Magnitude() == 0)
		{
			edgeDiff = (points[i+j] - points[i-j]) * .5;
			j++;
		}

		//tangent calculation
		tan = (edgeDiff)* (points[i+1]-points[i]).Magnitude() / (edgeDiff).Magnitude();
		tangents.push_back(tan);
	}

	//init the last point
	tan = points[points.size()-1] - points[points.size()-2];
	tangents.push_back(tan);
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
	//TODO renable to convert spline
	if(tabSet) {  //the very first runs of this function creates the table. So the u should not be converted
		curLocation = StoU(curLocation, startPoint); //convert arcLength based u into the spline's true u.
	}
	if(isCatmullMode)
	{
		return catmullMatrix(curLocation, startPoint);
	}
	else
	{
		int endPoint = startPoint + 1;
		return hermiteMatrix(curLocation, points[startPoint], points[endPoint],
				tangents[startPoint], tangents[endPoint]);
	}
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
	Vector3f nearbyPoint = Vector3f(0,0,0);

	
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
	else if(distanceAhead < 0) { //looking backward 
		int i = 0;
		for(i = startPoint; i > 0 ; i--) {
			result = paramTable[curPoint]->getBehindU(curU, distanceAhead);
			if(result <= 0) {  //didn't find point yet, keep going
				curU = 1; //start at the beginning of the point from now on.
				distanceAhead = distanceAhead + result;  //this subtracts(!) the distance already traveled
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


























