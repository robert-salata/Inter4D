#ifndef Algebra_master
#define Algebra_master

#include "real_number.h"
#include "common.h"

class AlgebraMaster
{
public:
	static RealNumber* solveEquationSystem(int size, RealNumber** matrix, const RealNumber* vector);
};

#endif
