#pragma once
#include "Rcpp.h"
#include "altrepMacro.h"



Rboolean sharedVector_Inspect(SEXP x, int pre, int deep, int pvec,
	void (*inspect_subtree)(SEXP, int, int, int))
{
	Rprintf(" Share object of type %s\n", SV_TYPE_NAME(x));
	return TRUE;
}

R_xlen_t sharedVector_length(SEXP x)
{
	DEBUG(Rprintf("accessing length:%.0f\n", SV_LENGTH(x)));
	return SV_LENGTH(x);
}

void* sharedVector_dataptr(SEXP x, Rboolean writeable) {
	DEBUG(Rprintf("accessing data pointer\n"));
	return SV_PTR(x);
}
const void* sharedVector_dataptr_or_null(SEXP x)
{
	DEBUG(Rprintf("accessing data pointer or null\n"));
	return sharedVector_dataptr(x, Rboolean::TRUE);
}

SEXP sharedVector_duplicate(SEXP x, Rboolean deep) {
	try {
		using namespace Rcpp;
		DEBUG(Rprintf("Duplicating data, deep: %d, copy on write: %d, shared duplicate %d\n", deep, SV_COPY_ON_WRITE(x), SV_SHARED_DUPLICATE(x)));
		//Rf_PrintValue(SV_DATA(x, dataInfo));
		if (SV_COPY_ON_WRITE(x)) {
			if (SV_SHARED_DUPLICATE(x)) {
				Environment package_env(PACKAGE_ENV);
				Function getSharedParms = package_env[".createInheritedParms"];
				List opt = getSharedParms(x);
				Function sv_constructor = package_env["share"];
				SEXP so = sv_constructor(x, opt);
				return(so);
			}
			else {
				return(NULL);
			}
		}
		else {
			return(x);
		}
	}
	catch (const std::exception & ex) {
		errorHandle("Error in duplicating an altrep\n%s", ex.what());
	}

	// Just for suppressing the annoying warning, it should never be excuted
	return(NULL);
}

SEXP sharedVector_serialized_state(SEXP x) {
	DEBUG(Rprintf("serialize state\n"));
	return(Rf_ScalarReal(SV_DATAID(x)));
}

void loadLibrary() {
	SEXP e;
	Rf_protect(e = Rf_lang2(Rf_install("library"), Rf_mkString(PACKAGE_NAME)));
	R_tryEval(e, R_GlobalEnv, NULL);
	Rf_unprotect(1);
}

SEXP sharedVector_unserialize(SEXP R_class, SEXP state) {
	try {
		using namespace Rcpp;
		DEBUG(Rprintf("unserializing data\n"));
		loadLibrary();
		DEBUG(Rprintf("Library loaded\n"));
		Environment package_env(PACKAGE_ENV);
		Function so_constructor = package_env["makeSharedVectorById"];
		SEXP so = so_constructor(state);
		return so;
	}
	catch (const std::exception & ex) {
		errorHandle("Error in unserializing an altrep\n%s", ex.what());
	}
	return state;
}


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
			Function getSharedParms = package_env["createInheritedParms"];
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
