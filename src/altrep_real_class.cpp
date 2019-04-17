#include "altrep_real_class.h"

#define REAL_PTR(x) ((double*)SO_PTR(x))

R_altrep_class_t shared_real_class;
double real_Elt(SEXP x, R_xlen_t i);
R_xlen_t real_region(SEXP x, R_xlen_t start, R_xlen_t size, double* out);

void InitRealClass(DllInfo *dll)
{
  R_altrep_class_t cls =
    R_make_altreal_class("shared_real", PGKNAME, dll);
  shared_real_class = cls;

  /* override ALTREP methods */
  R_set_altrep_Inspect_method(cls, sharedObject_Inspect);
  R_set_altrep_Length_method(cls, sharedObject_length);

  /* override ALTVEC methods */
  R_set_altvec_Dataptr_method(cls, sharedObject_dataptr);
  R_set_altvec_Dataptr_or_null_method(cls, sharedObject_dataptr_or_null);

  /* override ALTREAL methods */
  R_set_altreal_Elt_method(cls, real_Elt);
  R_set_altreal_Get_region_method(cls, real_region);
}


double real_Elt(SEXP x, R_xlen_t i){

	DEBUG(Rprintf("accessing element %d\n",i));
  return REAL_PTR(x)[i];
}

R_xlen_t real_region(SEXP x, R_xlen_t start, R_xlen_t size, double* out){
	DEBUG(Rprintf("accessing region\n"));
  double* source = REAL_PTR(x) + start;
  R_xlen_t rest_len = SO_LENGTH(x) - start;
  R_xlen_t ncopy = rest_len > size ? size : rest_len;
  for (R_xlen_t k = 0; k < ncopy; k++) {
	  out[k] = source[k];
  }
  return ncopy;

}
