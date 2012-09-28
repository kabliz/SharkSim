#ifndef ARC_LENGTH_PARAMAT_GAUSSIAN_INTEGRATION
#define ARC_LENGTH_PARAMAT_GAUSSIAN_INTEGRATION

#include "ParamStructures.h"
#include "SplineTable.h"
#include <math.h>
#include <cstdlib>
#include <cstdio>

//Functions for Adaptive Gaussian Integration. From the Parent Text.
/*
 * These functions will paramaterize a curve on two dimensions. Since y is constant in Shark World, this is OK.
 */
class ParamFunctions
{
	public:
		ParamFunctions(SplineTable *tPtr){ table = tPtr;}
		ParamFunctions(){}
		~ParamFunctions(){}
		void setTable(SplineTable *tPtr){table = tPtr;}
		double adaptiveIntegration(cubicCurveTd *curve, double u1, double u2, double tolerance);
		double subdivide (intervalTd *fullInterval, polynomialTd *func, double totalLength, double tolerance);
		void addTableEntry(double u, double length);
		double integrateFunc(polynomialTd *func, intervalTd *interval);
		double evaluatePolynomial(polynomialTd *poly, double u);

	private:
		SplineTable *table; 

};

#endif
