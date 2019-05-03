#include "altrep_macro.h"
#include "tools.h"
#include "Rcpp.h"
#include "R_ext/Altrep.h"

#include "Rinternals.h"
#include "altrep_numeric_registration.h"

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
	default: errorHandle("Type of %d is not supported yet", type);
	}
}



Rboolean sharedObject_Inspect(SEXP x, int pre, int deep, int pvec,
                      void (*inspect_subtree)(SEXP, int, int, int))
{
  Rprintf(" shareObject of type %s", SO_TYPE_CHAR(x));
  return TRUE;
}

R_xlen_t sharedObject_length(SEXP x)
{
	DEBUG(Rprintf("accessing length:%.0f\n", SO_LENGTH(x)));
  return SO_LENGTH(x);
}

void *sharedObject_dataptr(SEXP x, Rboolean writeable){
	DEBUG(Rprintf("accessing data pointer\n"));
  return SO_PTR(x);
}
const void *sharedObject_dataptr_or_null(SEXP x)
{
	DEBUG(Rprintf("accessing data pointer or null\n"));
	return sharedObject_dataptr(x, Rboolean::TRUE);
}
SEXP sharedObject_dulplicate(SEXP x, Rboolean deep) {
	return(x);
}

using namespace Rcpp;
void sharedObject_updateAd(SEXP x)
{
	/*Environment env = SO_ENV(x);
	Rcout << env.ls(true)<< "\n";
	Rf_PrintValue(env.ls(true));*/
	Function fun = SO_UPDATE_FUN(x);
	fun();
}


SEXP sharedObject_serialized_state(SEXP x) {
	printf("serialize state\n");
	SEXP did = SO_DATA(x, DID);
	return(did);
}

void loadLibrary() {
	SEXP e;
	Rf_protect(e = Rf_lang2(Rf_install("library"), Rf_mkString("sharedObject")));
	R_tryEval(e, R_GlobalEnv, NULL);
	Rf_unprotect(1);
}

SEXP sharedObject_unserialize(SEXP R_class, SEXP state) {
	//Rf_PrintValue(R_class);
	//return R_class;
	//return(Rf_ScalarInteger(1));
	printf("unserializing data\n");
	
	loadLibrary();
	Environment package_env("package:sharedObject");
	Function so_constructor = package_env["sharedObjectById"];
	SEXP so = so_constructor(state);
	return so;
}


