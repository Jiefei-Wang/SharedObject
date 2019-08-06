//These functions are the required functions for a numeric ALTREP object

#pragma once
#include "Rcpp.h"
#include "altrepCommonFunc.h"
#include "altrepMacro.h"

#define TEMPLATE_TYPE_PTR(x) ((T*)SV_PTR(x))
template<class T>
SEXP template_coerce(T* x, R_xlen_t len, int type)
{
	DEBUG(messageHandle("coerce\n"));
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
			errorHandle("Unknown type: %d\n", type);
		}
	}
	UNPROTECT(1);
	return(result);
}


template<class T>
T numeric_Elt(SEXP x, R_xlen_t i) {
	DEBUG(Rprintf("accessing numeric element %d\n", i));
	return TEMPLATE_TYPE_PTR(x)[i];
}


template<class T>
R_xlen_t numeric_region(SEXP x, R_xlen_t start, R_xlen_t size, T* out) {
	DEBUG(Rprintf("accessing numeric region\n"));
	R_xlen_t rest_len = SV_LENGTH(x) - start;
	R_xlen_t ncopy = rest_len > size ? size : rest_len;
	memcpy(out, TEMPLATE_TYPE_PTR(x) + start, ncopy * sizeof(T));
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
			errorHandle("Index out of bound:\n index: %llu length:%llu\n", (ULLong)indx[i], (ULLong)src_len);
		}
	}
	DEBUG(Rprintf("\n"));
}

template<int SXP_TYPE, class C_TYPE>
SEXP numeric_subset(SEXP x, SEXP indx, SEXP call) {
	try {
		DEBUG(printf("Accessing subset, sharedSubset: %d\n", SV_SHARED_SUBSET(x));)
			
		//Allocate the subset vector and assign values
		R_xlen_t len = Rf_xlength(indx);
		SEXP sub_vector = PROTECT(Rf_allocVector(SXP_TYPE, len));
		switch (TYPEOF(indx)) {
		case INTSXP:
			template_subset_assignment((C_TYPE*)dataptr(sub_vector), (C_TYPE*)SV_PTR(x), INTEGER(indx), Rf_xlength(x), Rf_xlength(indx));
			break;
		case REALSXP:
			template_subset_assignment((C_TYPE*)dataptr(sub_vector), (C_TYPE*)SV_PTR(x), REAL(indx), Rf_xlength(x), Rf_xlength(indx));
			break;
		}
		//Check if the subset vector needs to be a shared object
		if (SV_SHARED_SUBSET(x)) {
			using namespace Rcpp;
			Environment package_env(PACKAGE_ENV);
			Function getSharedParms = package_env[".createInheritedParms"];
			List opt = getSharedParms(x);
			Function sv_constructor = package_env["share"];
			SEXP so = sv_constructor(sub_vector, opt);
			UNPROTECT(1);
			return so;
		}
		else {
			UNPROTECT(1);
			return sub_vector;
		}
	}
	catch (const std::exception& ex) {
		errorHandle("Error in subsetting an altrep\n%s", ex.what());
	}
	// Just for suppressing the annoying warning, it should never be excuted
	return NULL;

}
