#pragma once
#include "Rcpp.h"
#include "macro.h"
#include "tools.h"
#include "C_interface.h"


Rboolean sharedVector_Inspect(SEXP x, int pre, int deep, int pvec,
	void (*inspect_subtree)(SEXP, int, int, int))
{
	Rprintf(" (len=%llu, type=%d) shared object ID:%d\n", Rf_xlength(x),TYPEOF(x), 
		Rcpp::as<int>(GET_ALT_SLOT(x, INFO_DATAID)));

	return TRUE;
}

R_xlen_t sharedVector_length(SEXP x)
{
	R_xlen_t size = Rcpp::as<R_xlen_t>(GET_ALT_SLOT(x, INFO_LENGTH));
	DEBUG(Rprintf("accessing length:%llu\n", size));
	return size;
}

void* sharedVector_dataptr(SEXP x, Rboolean writeable) {
	DEBUG(Rprintf("accessing data pointer\n"));
	return ALT_EXTPTR(x);
}
const void* sharedVector_dataptr_or_null(SEXP x)
{
	DEBUG(Rprintf("accessing data pointer or null\n"));
	return sharedVector_dataptr(x, Rboolean::TRUE);
}

SEXP sharedVector_duplicate(SEXP x, Rboolean deep) {
	try {
		using namespace Rcpp;
		bool copyOnWrite = as<bool>(GET_ALT_SLOT(x, INFO_COPYONWRITE));
		bool sharedCopy = as<bool>(GET_ALT_SLOT(x, INFO_SHAREDCOPY));
		DEBUG(Rprintf("Duplicating data, deep: %d, copy on write: %d, shared copy %d\n", 
			deep, copyOnWrite, sharedCopy));
		if (copyOnWrite) {
			if (sharedCopy) {
				List newDataInfo = Rf_duplicate(ALT_DATAINFO(x));
				newDataInfo[INFO_OWNDATA] = Rf_ScalarLogical(true);
				SEXP result = C_createSharedMemory(x, wrap(newDataInfo));
				return(result);
			}
			else {
				SEXP result = PROTECT(Rf_allocVector(TYPEOF(x), XLENGTH(x)));
				memcpy(DATAPTR(result), DATAPTR(x), as<R_xlen_t>(GET_ALT_SLOT(x, INFO_TOTALSIZE)));
				UNPROTECT(1);
				return result;
			}
		}
		else {
			List newDataInfo = Rf_duplicate(ALT_DATAINFO(x));
			newDataInfo[INFO_OWNDATA] = Rf_ScalarLogical(false);
			SEXP result = C_readSharedMemory(newDataInfo);
			return(result);
		}
	}
	catch (const std::exception & ex) {
		Rf_error("Error in duplicating an altrep\n%s", ex.what());
	}

	// Just for suppressing the annoying warning, it should never be excuted
	return(NULL);
}

SEXP sharedVector_serialized_state(SEXP x) {
	DEBUG(Rprintf("serialize state\n"));
	SEXP dataInfo = PROTECT(Rf_duplicate(ALT_DATAINFO(x)));
	SET_SLOT(dataInfo, INFO_OWNDATA, Rf_ScalarLogical(0));
	UNPROTECT(1);
	return(dataInfo);
}

void loadLibrary() {
	SEXP e;
	Rf_protect(e = Rf_lang2(Rf_install("library"), Rf_mkString(PACKAGE_NAME)));
	R_tryEval(e, R_GlobalEnv, NULL);
	Rf_unprotect(1);
}

SEXP sharedVector_unserialize(SEXP R_class, SEXP dataInfo) {
	try {
		using namespace Rcpp;
		DEBUG(Rprintf("unserializing data\n"));
		loadLibrary();
		DEBUG(Rprintf("Library loaded\n"));

		SEXP result = C_readSharedMemory(dataInfo);
		return result;
	}
	catch (const std::exception & ex) {
		Rf_error("Error in unserializing an altrep\n%s", ex.what());
	}
	return dataInfo;
}


template<class T>
SEXP template_coerce(T* x, R_xlen_t len, int type)
{
	DEBUG(Rprintf("coerce\n"));
	SEXP result = PROTECT(Rf_allocVector(type, len));
	for (R_xlen_t i = 0; i < len; i++) {
		switch (type)
		{
		case INTSXP:
			INTEGER(result)[i] = x[i];
			break;
		case REALSXP:
			REAL(result)[i] = x[i];
			break;
		default:
			Rf_error("Unknown type: %d\n", type);
		}
	}
	UNPROTECT(1);
	return(result);
}


template<class T>
T numeric_Elt(SEXP x, R_xlen_t i) {
	DEBUG(Rprintf("accessing numeric element %d\n", i));
	return ((T*)ALT_EXTPTR(x))[i];
}


template<class T>
R_xlen_t numeric_region(SEXP x, R_xlen_t start, R_xlen_t size, T* out) {
	DEBUG(Rprintf("accessing numeric region\n"));
	T* ptr = (T*)ALT_EXTPTR(x);
	R_xlen_t rest_len = Rf_length(x) - start;
	R_xlen_t ncopy = rest_len > size ? size : rest_len;
	memcpy(out, ptr + start, ncopy * sizeof(T));
	return ncopy;
}


#include <type_traits>
template<class T1, class T2>
void template_subset_assignment(T1* target, T1* source, T2* indx, R_xlen_t src_len, R_xlen_t ind_len) {
	source = source - 1L;
	DEBUG(Rprintf("Index:"));
	for (R_xlen_t i = 0; i < ind_len; i++) {
		DEBUG(Rprintf("%d,", (int)indx[i]));
		if (indx[i] <= src_len && indx[i] > 0) {
			if (std::is_same<T2, double>::value) {
				target[i] = source[(R_xlen_t)indx[i]];
			}
			else {
				target[i] = source[(int)indx[i]];
			}
		}
		else {
			Rf_error("Index out of bound:\n index: %llu length:%llu\n", (R_xlen_t)indx[i], (R_xlen_t)src_len);
		}
	}
	DEBUG(Rprintf("\n"));
}

template<int SXP_TYPE, class C_TYPE>
SEXP numeric_subset(SEXP x, SEXP indx, SEXP call) {
	using namespace Rcpp;
	try {
		bool sharedSubset = as<bool>(GET_ALT_SLOT(x, INFO_SHAREDSUBSET));
		DEBUG(Rprintf("Accessing subset, sharedSubset: %d\n", sharedSubset));
			
		//Allocate the subset vector and assign values
		R_xlen_t len = Rf_xlength(indx);
		SEXP subVector;
		if (sharedSubset) {
			List newDataInfo = Rf_duplicate(ALT_DATAINFO(x));

			Environment package_env(PACKAGE_ENV);
			Function calculateSharedMemorySize = package_env["calculateSharedMemorySize"];
			List opt = calculateSharedMemorySize(x);

			newDataInfo[INFO_LENGTH] = Rf_ScalarReal(Rf_xlength(indx));
			newDataInfo[INFO_TOTALSIZE] = Rf_ScalarReal(Rf_xlength(indx) * sizeof(C_TYPE));
			newDataInfo[INFO_OWNDATA] = Rf_ScalarLogical(true);

			subVector = PROTECT(C_createEmptySharedMemory(newDataInfo));
		}
		else {
			subVector = PROTECT(Rf_allocVector(SXP_TYPE, len));
		}

		C_TYPE* src_ptr = (C_TYPE*)DATAPTR(x);
		C_TYPE* dest_ptr = (C_TYPE*)DATAPTR(subVector);
		switch (TYPEOF(indx)) {
		case INTSXP:
			template_subset_assignment(dest_ptr, src_ptr, INTEGER(indx), Rf_xlength(x), Rf_xlength(indx));
			break;
		case REALSXP:
			template_subset_assignment(dest_ptr, src_ptr, REAL(indx), Rf_xlength(x), Rf_xlength(indx));
			break;
		}
		UNPROTECT(1);
		return subVector;
	}
	catch (const std::exception& ex) {
		Rf_error("Error in subsetting an altrep\n%s", ex.what());
	}
	// Just for suppressing the annoying warning, it should never be excuted
	return NULL;

}
