#include <string>
#include "Rcpp.h"
#include "altrep.h"
#include "altrepMacro.h"
#include "utils.h"
#include "sharedMemory.h"
using std::string;
using namespace Rcpp;

Rboolean sharedVector_Inspect(SEXP x, int pre, int deep, int pvec,
							  void (*inspect_subtree)(SEXP, int, int, int))
{
	Rprintf(" (len=%llu, type=%d, COW=%d, SC=%d, SS=%d) shared object ID:%s\n", Rf_xlength(x), TYPEOF(x),
			Rcpp::as<int>(GET_ALT_SLOT(x, INFO_COPYONWRITE)),
			Rcpp::as<int>(GET_ALT_SLOT(x, INFO_SHAREDCOPY)),
			Rcpp::as<int>(GET_ALT_SLOT(x, INFO_SHAREDSUBSET)),
			Rcpp::as<string>(GET_ALT_SLOT(x, INFO_DATAID)).c_str());
	return TRUE;
}

R_xlen_t sharedVector_length(SEXP x)
{
	R_xlen_t size = Rcpp::as<R_xlen_t>(GET_ALT_SLOT(x, INFO_LENGTH));
	altrepPrint("accessing length:%llu\n", size);
	return size;
}

void *sharedVector_dataptr(SEXP x, Rboolean writeable)
{
	altrepPrint("accessing data pointer\n");
	return ALT_EXTPTR(x);
}
const void *sharedVector_dataptr_or_null(SEXP x)
{
	altrepPrint("accessing data pointer or null\n");
	return sharedVector_dataptr(x, Rboolean::TRUE);
}

SEXP sharedVector_duplicate(SEXP x, Rboolean deep)
{
	bool copyOnWrite = as<bool>(GET_ALT_SLOT(x, INFO_COPYONWRITE));
	bool sharedSubset = as<bool>(GET_ALT_SLOT(x, INFO_SHAREDSUBSET));
	bool sharedCopy = as<bool>(GET_ALT_SLOT(x, INFO_SHAREDCOPY));
	altrepPrint("Duplicating data, deep: %d, copy on write: %d, shared copy %d\n",
				deep, copyOnWrite, sharedCopy);
	if (copyOnWrite)
	{
		if (sharedCopy)
		{
			List newDataInfo = Rf_duplicate(ALT_DATAINFO(x));
			SEXP result = createSharedObjectFromSource(x,
														 copyOnWrite,
														 sharedSubset,
														 sharedCopy);
			return (result);
		}
		else
		{
			SEXP result = PROTECT(Rf_allocVector(TYPEOF(x), XLENGTH(x)));
			memcpy(DATAPTR(result), DATAPTR(x), as<R_xlen_t>(GET_ALT_SLOT(x, INFO_TOTALSIZE)));
			UNPROTECT(1);
			return result;
		}
	}
	else
	{
		List newDataInfo = Rf_duplicate(ALT_DATAINFO(x));
		SEXP result = readSharedObject(newDataInfo);
		return (result);
	}
}

SEXP sharedVector_serialized_state(SEXP x)
{
	altrepPrint("serialize state\n");
	//We check the memory before serialize the object
	std::string id = Rcpp::as<std::string>(GET_ALT_SLOT(x, INFO_DATAID));
	if (!hasSharedMemory(id))
	{
		Rf_warning("The shared memory does not exist(id: %s), the unshared data will be exported instead\n", id.c_str());
		SEXP unsharedData = PROTECT(Rf_allocVector(TYPEOF(x), XLENGTH(x)));
		memcpy(DATAPTR(unsharedData), DATAPTR(x), getObjectSize(x));
		UNPROTECT(1);
		return unsharedData;
	}else{
		return (ALT_DATAINFO(x));
	}
}

SEXP sharedVector_unserialize(SEXP R_class, SEXP dataInfo)
{
	altrepPrint("unserializing data\n");
	if(TYPEOF(dataInfo) != VECSXP){
		Rf_warning("The shared memory has been released! Receiving unshared object.\n");
		return dataInfo;
	}else{
		SEXP result;
		try
		{
			//loadLibrary();
			altrepPrint("Library loaded\n");
			result = readSharedObject(dataInfo);
		}
		catch (const std::exception & ex){
			Rf_warning("Error in unserializing an altrep\n%s", ex.what());
			result = R_NilValue;
		}
		return result;
	}
}

SEXP sharedVector_subset(SEXP x, SEXP indx, SEXP call)
{
	bool copyOnWrite = as<bool>(GET_ALT_SLOT(x, INFO_COPYONWRITE));
	bool sharedSubset = as<bool>(GET_ALT_SLOT(x, INFO_SHAREDSUBSET));
	bool sharedCopy = as<bool>(GET_ALT_SLOT(x, INFO_SHAREDCOPY));
	altrepPrint("Accessing subset, sharedSubset: %d\n", sharedSubset);

	//Allocate the subset vector and assign values
	uint64_t length = Rf_xlength(indx);
	SEXP subVector;
	if (sharedSubset)
	{
		subVector = createEmptySharedObject(TYPEOF(x), length,
											  copyOnWrite, sharedSubset, sharedCopy);
		PROTECT(subVector);
	}
	else
	{
		subVector = PROTECT(Rf_allocVector(TYPEOF(x), length));
	}
	// 1-based index
	uint8_t typeSize = getTypeSize(TYPEOF(x));
	void *indx_ptr = DATAPTR(indx);
	char *src_ptr = (char *)DATAPTR(x) - typeSize;
	char *dest_ptr = (char *)DATAPTR(subVector);
	for (uint64_t i = 0; i < length; i++)
	{
		switch (TYPEOF(indx))
		{
		case INTSXP:
			memcpy(dest_ptr + i * typeSize, src_ptr + ((int *)indx_ptr)[i] * typeSize, typeSize);
			break;
		case REALSXP:
			memcpy(dest_ptr + i * typeSize, src_ptr + ((size_t)((double *)indx_ptr)[i]) * typeSize, typeSize);
			break;
		}
	}
	UNPROTECT(1);
	return subVector;
}
