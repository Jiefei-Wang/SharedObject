#include "altrep_real_class.h"


#define REAL_PTR(x) ((double*)SO_PTR(x))

R_altrep_class_t shared_real_class;
static double real_Elt(SEXP x, R_xlen_t i);
static R_xlen_t real_region(SEXP x, R_xlen_t start, R_xlen_t size, double* out);
static SEXP real_subset(SEXP x, SEXP indx, SEXP call);
static SEXP real_coerce(SEXP x, int type);


void InitRealClass(DllInfo* dll)
{
	R_altrep_class_t cls =
		R_make_altreal_class("shared_real", PGKNAME, dll);
	shared_real_class = cls;

	/* override ALTREP methods */
	R_set_altrep_Inspect_method(cls, sharedObject_Inspect);
	R_set_altrep_Length_method(cls, sharedObject_length);
	R_set_altrep_Duplicate_method(cls, sharedObject_dulplicate);
	R_set_altrep_Coerce_method(cls, real_coerce);

	/* override ALTVEC methods */
	R_set_altvec_Dataptr_method(cls, sharedObject_dataptr);
	R_set_altvec_Dataptr_or_null_method(cls, sharedObject_dataptr_or_null);
	R_set_altvec_Extract_subset_method(cls, real_subset);

	/* override ALTREAL methods */
	R_set_altreal_Elt_method(cls, real_Elt);
	R_set_altreal_Get_region_method(cls, real_region);
}


static double real_Elt(SEXP x, R_xlen_t i) {
	DEBUG(Rprintf("accessing element %d\n", i));
	return REAL_PTR(x)[i];
}

static R_xlen_t real_region(SEXP x, R_xlen_t start, R_xlen_t size, double* out) {
	DEBUG(Rprintf("accessing region\n"));
	double* source = REAL_PTR(x) + start;
	R_xlen_t rest_len = SO_LENGTH(x) - start;
	R_xlen_t ncopy = rest_len > size ? size : rest_len;
	for (R_xlen_t k = 0; k < ncopy; k++) {
		out[k] = source[k];
	}
	return ncopy;
}



static SEXP real_subset(SEXP x, SEXP indx, SEXP call) {
	DEBUG(Rprintf("accessing subset\n"));
	R_xlen_t len = xlength(indx);
	SEXP result = PROTECT(allocVector(REALSXP, len));
	switch (TYPEOF(indx) ) {
	case INTSXP:
		template_subset_assignment(REAL(result), REAL_PTR(x),INTEGER(indx),xlength(x),xlength(indx));
		break;
	case REALSXP:
		template_subset_assignment(REAL(result), REAL_PTR(x), REAL(indx), xlength(x), xlength(indx));
		break;
	}
	UNPROTECT(1);
	return(result);
}

static SEXP real_coerce(SEXP x, int type) {
	DEBUG(Rprintf("accessing subset\n"));
	R_xlen_t len = xlength(x);
	SEXP result=template_coerce(REAL(x), len, type);
	return(result);
}

/*
SEXP result = NULL;

	if (! OBJECT(x) && ATTRIB(x) == R_NilValue &&
	DEFERRED_STRING_STATE(x) != R_NilValue) {
	 For deferred string coercions, create a new conversion
	   using the subset of the argument.  Could try to
	   preserve/share coercions already done, if there are any.
SEXP data = DEFERRED_STRING_ARG(x);
SEXP info = DEFERRED_STRING_INFO(x);
PROTECT(result = ExtractSubset(data, indx, call));
result = R_deferred_coerceToString(result, info);
UNPROTECT(1);
return result;
	}

	return result;
*/