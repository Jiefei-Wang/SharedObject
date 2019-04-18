#include "altrep_common_func.h"
#include "tools.h"

Rboolean sharedObject_Inspect(SEXP x, int pre, int deep, int pvec,
                      void (*inspect_subtree)(SEXP, int, int, int))
{
  Rprintf(" shareObject of type %s", type2char(TYPEOF(x)));
  return TRUE;
}
/*
 A list stores the info of the shared data
 list 1: type
 list 2: lenght, size
*/

SEXP make_sharedObject_state( SEXP R_type,SEXP R_length,SEXP R_size){
//list 2
  SEXP sizes = PROTECT(allocVector(REALSXP, 2));
  REAL(sizes)[0]=asReal(R_length);
  REAL(sizes)[1]=asReal(R_size);

  SEXP vec = PROTECT(allocVector(VECSXP, 2));

  SET_VECTOR_ELT(vec, 0, R_type);
  SET_VECTOR_ELT(vec, 1, sizes);


  UNPROTECT(2);
  return(vec);
}

R_xlen_t sharedObject_length(SEXP x)
{
	DEBUG(Rprintf("accessing length:%llu\n", SO_LENGTH(x)));
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
