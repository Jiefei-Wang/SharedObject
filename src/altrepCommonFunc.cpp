#include "Rcpp.h"
#include "Rinternals.h"
#include "tools.h"
#include "altrepRegistration.h"
#include "altrepMacro.h"

const void* getPointer(SEXP x) {
	const void* ptr;
	switch (TYPEOF(x))
	{
	/*case INTSXP:
		return INTEGER(x);
	case REALSXP:
		return REAL(x);
	case LGLSXP:
		return LOGICAL(x);
	case RAWSXP:
		return RAW(x);*/
	case INTSXP:
	case REALSXP:
	case LGLSXP:
	case RAWSXP:
		ptr = DATAPTR_OR_NULL(x);
		if (ptr == NULL) {
			return DATAPTR(x);
		}
		else {
			return ptr;
		}
	case STRSXP:
		return x;
	default:
		errorHandle("Unexpected SEXP of type %d\n", TYPEOF(x));
	}
	return nullptr;
}

R_altrep_class_t getAltClass(int type) {
	switch (type) {
	case REAL_TYPE:
		return shared_real_class;
	case INT_TYPE:
		return shared_integer_class;
	case LOGICAL_TYPE:
		return shared_logical_class;
	case RAW_TYPE:
		return shared_raw_class;
	case STR_TYPE:
		//return shared_str_class;
	default: errorHandle("Type of %d is not supported yet", type);
	}
}



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

void *sharedVector_dataptr(SEXP x, Rboolean writeable){
	DEBUG(Rprintf("accessing data pointer\n"));
  return SV_PTR(x);
}
const void *sharedVector_dataptr_or_null(SEXP x)
{
	DEBUG(Rprintf("accessing data pointer or null\n"));
	return sharedVector_dataptr(x, Rboolean::TRUE);
}

SEXP sharedVector_duplicate(SEXP x, Rboolean deep) {
	using namespace Rcpp;
	DEBUG(Rprintf("Duplicating data, deep: %d, copy on write: %d, shared duplicate %d\n",deep, SV_COPY_ON_WRITE(x),SV_SHARED_DUPLICATE(x)));
	//Rf_PrintValue(SV_DATA(x, dataInfo));
	if (SV_COPY_ON_WRITE(x)) {
		if (SV_SHARED_DUPLICATE(x)) {
			Environment package_env(PACKAGE_ENV_NAME);
			Function getSharedParms = package_env["createInheritedParms"];
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

SEXP sharedVector_serialized_state(SEXP x) {
	DEBUG(Rprintf("serialize state\n");)
	//SEXP e = Rf_protect( Rf_lang2(Rf_install("serializeSO"),x));
	//SEXP did= R_tryEval(e, R_GlobalEnv, NULL);
	//Rf_unprotect(1);

	return(Rf_ScalarReal(SV_DATAID(x)));
}

void loadLibrary() {
	SEXP e;
	Rf_protect(e = Rf_lang2(Rf_install("library"), Rf_mkString(PACKAGE_NAME)));
	R_tryEval(e, R_GlobalEnv, NULL);
	Rf_unprotect(1);
}

SEXP sharedVector_unserialize(SEXP R_class, SEXP state) {
	using namespace Rcpp;
	DEBUG(Rprintf("unserializing data\n");)
	
	loadLibrary();
	DEBUG(Rprintf("Library loaded\n");)
	Environment package_env(PACKAGE_ENV_NAME);
	Function so_constructor = package_env["sharedVectorById"];
	SEXP so = so_constructor(state);
	return so;
}


