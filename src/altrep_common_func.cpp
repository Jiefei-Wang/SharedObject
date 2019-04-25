#include "altrep_common_func.h"
#include "tools.h"

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

