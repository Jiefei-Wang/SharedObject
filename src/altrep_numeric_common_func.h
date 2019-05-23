#pragma once
#include "Rcpp.h"
#include "altrep_common_func.h"

#define TMP_PTR(x) ((T*)SV_PTR(x))
template<class T>
SEXP template_coerce(T* x, R_xlen_t len, int type)
{
	SEXP result;

	switch (type)
	{
	case INTSXP:
		result = Rf_protect(Rf_allocVector(INTSXP, len));
		for (R_xlen_t i = 0; i < len; i++) {
			INTEGER(result)[i] = x[i];
		}
		break;
	case REALSXP:
		result = Rf_protect(Rf_allocVector(REALSXP, len));
		for (R_xlen_t i = 0; i < len; i++) {
			REAL(result)[i] = x[i];
		}
		break;
	default:
		errorHandle("Unknown type: %d\n", type);
	}
	DEBUG(messageHandle("coerce\n"));
	UNPROTECT(1);
	return(result);
}


template<class T>
T numeric_Elt(SEXP x, R_xlen_t i) {
	DEBUG(Rprintf("accessing numeric element %d\n", i));
	return TMP_PTR(x)[i];
}




template<class T>
R_xlen_t numeric_region(SEXP x, R_xlen_t start, R_xlen_t size, T* out) {
	DEBUG(Rprintf("accessing numeric region\n"));
	R_xlen_t rest_len = SV_LENGTH(x) - start;
	R_xlen_t ncopy = rest_len > size ? size : rest_len;
	memcpy(out, TMP_PTR(x) + start, ncopy * sizeof(T));
	return ncopy;
}
#include <type_traits>
template<class T1, class T2>
void template_subset_assignment(T1* target, T1* source, T2* indx, R_xlen_t src_len, R_xlen_t ind_len) {
	source = source - 1L;
	DEBUG(messageHandle("Index:"));
	for (R_xlen_t i = 0; i < ind_len; i++) {
		DEBUG(messageHandle("%d,", (int)indx[i]));
		if (indx[i] <= src_len&& indx[i]>0) {
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
	messageHandle("\n");
}

template<int SXP_TYPE, class C_TYPE>
SEXP numeric_subset(SEXP x, SEXP indx, SEXP call) {
	using namespace Rcpp;
	DEBUG(Rprintf("accessing subset\n"));
	Environment package_env("package:sharedObject");


		R_xlen_t len = Rf_xlength(indx);
		C_TYPE* result = Calloc(len ,C_TYPE);
		switch (TYPEOF(indx)) {
		case INTSXP:
			template_subset_assignment(result, (C_TYPE*)SV_PTR(x), INTEGER(indx), Rf_xlength(x), Rf_xlength(indx));
			break;
		case REALSXP:
			template_subset_assignment(result, (C_TYPE*)SV_PTR(x), REAL(indx), Rf_xlength(x), Rf_xlength(indx));
			break;
		}
		SEXP res= wrap(Vector<SXP_TYPE>(result, result + len));
		DEBUG(Rprintf("sharedSubset:%d\n", SV_SHAREDSUB(x));)
		if (SV_SHAREDSUB(x)) {
			Function getSharedParms = package_env["createInheritedParms"];
			List opt = getSharedParms(x);
			Function sv_constructor = package_env["sharedVector"];
			SEXP so = sv_constructor(res, opt);
			return so;
		}
		else {
			return res;

		}
	
	


}
