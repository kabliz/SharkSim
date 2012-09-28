#ifndef SHARK_3D_POLYNOMIAL_CUBIC_FUNCTION_STRUCTURES
#define SHARK_3D_POLYNOMIAL_CUBIC_FUNCTION_STRUCTURES

//structures from Parent Animation book. These structures are used to calculate Arc-Length

typedef struct tableEntryStructure
{
	double u;
	double length;
}tableEntryTd;


typedef struct intervalStructure
{
	double u1;
	double u2;
	double length;
}intervalTd;

typedef struct cubicCurveStructure
{
	double ax;
	double bx;
	double cx;
	double dx;
	double ay;
	double by;
	double cy;
	double dy;

}cubicCurveTd;

typedef struct polynomialStructure
{
	double *coeff;
	int degree;
}polynomialTd;



#endif
