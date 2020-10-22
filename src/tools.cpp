#include "tools.h"
#include "R.h"
#include <Rinternals.h>
#include <cstdarg>
using namespace std;

const string OS_ADDRESS_SIZE = "X" + to_string(sizeof(void*) * 8);


size_t getTypeSize(int type)
{
	size_t elt_size = 0;
	switch (type)
	{
	case INTSXP:
		elt_size = 4;
		break;
	case LGLSXP:
		elt_size = 4;
		break;
	case REALSXP:
		elt_size = 8;
		break;
	case RAWSXP:
		elt_size = 1;
		break;
	case CPLXSXP:
		elt_size = 16;
		break;
	//case STRSXP:
	// unsigned64_t
	default:
		Rf_error("Unknown type: %d\n",type);
	}
	return elt_size;
}

uint64_t getObjectSize(SEXP x)
{
	size_t elt_size = getTypeSize(TYPEOF(x));
	return elt_size * XLENGTH(x);
}
