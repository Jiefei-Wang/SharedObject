#include "tools.h"
#include "R.h"
#include <Rinternals.h>
#include <cstdarg>

#define BUFFER_SIZE 1024 * 1024

using namespace std;

const string OS_ADDRESS_SIZE = "X" + to_string(sizeof(void*) * 8);
static dispatchLocation errorLocation = R_ERROR;

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

void setErrorDispatch(dispatchLocation location){
		errorLocation = location;
}
void throwError(const char *format, ...){
		char buffer[BUFFER_SIZE];
		va_list args;
		va_start(args, format);
		vsnprintf(buffer, BUFFER_SIZE, format, args);
		switch (errorLocation)
		{
		case R_ERROR:
			Rf_error(buffer);
			break;
		case CPP_EXCEPTION:
			throw(string(buffer));
			break;
		}
}
