#include "altrep_macro.h"
#include "tools.h"
#include "Rcpp.h"
#include "R_ext/Altrep.h"

#include "Rinternals.h"
#include "altrep_registration.h"

void* getPointer(SEXP x) {
	switch (TYPEOF(x))
	{
	case INTSXP:
		return INTEGER(x);
	case REALSXP:
		return REAL(x);
	case LGLSXP:
		return LOGICAL(x);
	case RAWSXP:
		return RAW(x);
		//return STDVEC_DATAPTR(x);
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
		return shared_str_class;
	default: errorHandle("Type of %d is not supported yet", type);
	}
}



Rboolean sharedVector_Inspect(SEXP x, int pre, int deep, int pvec,
                      void (*inspect_subtree)(SEXP, int, int, int))
{
  Rprintf(" Share object of type %s\n", SV_TYPE_CHAR(x));
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
	DEBUG(Rprintf("Duplicating data, deep: %d, copy on write: %d \n",deep, SV_COW(x)));
	//Rf_PrintValue(SV_DATA(x, dataInfo));
	if (SV_COW(x)) {
		return(NULL);
	}
	else {
		return(x);
	}
}


using namespace Rcpp;
void sharedVector_updateAd(SEXP x)
{
	/*Environment env = SV_ENV(x);
	Rcout << env.ls(true)<< "\n";
	Rf_PrintValue(env.ls(true));*/
	Function fun = SV_UPDATE_FUN(x);
	fun();
}


SEXP sharedVector_serialized_state(SEXP x) {
	DEBUG(Rprintf("serialize state\n");)
	SEXP e = Rf_protect( Rf_lang2(Rf_install("serializeSO"),x));
	SEXP did= R_tryEval(e, R_GlobalEnv, NULL);
	Rf_unprotect(1);
	return(did);
}

void loadLibrary() {
	SEXP e;
	Rf_protect(e = Rf_lang2(Rf_install("library"), Rf_mkString("sharedObject")));
	R_tryEval(e, R_GlobalEnv, NULL);
	Rf_unprotect(1);
}

SEXP sharedVector_unserialize(SEXP R_class, SEXP state) {
	//Rf_PrintValue(R_class);
	//return R_class;
	//return(Rf_ScalarInteger(1));
	DEBUG(Rprintf("unserializing data\n");)
	
	loadLibrary();
	Environment package_env("package:sharedObject");
	Function so_constructor = package_env["unserializeSO"];
	SEXP so = so_constructor(state);
	return so;
}


