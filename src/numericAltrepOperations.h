#include <Rcpp.h>

Rboolean sharedVector_Inspect(SEXP x, int pre, int deep, int pvec,
                              void (*inspect_subtree)(SEXP, int, int, int));
R_xlen_t sharedVector_length(SEXP x);
void *sharedVector_dataptr(SEXP x, Rboolean writeable);
const void *sharedVector_dataptr_or_null(SEXP x);
SEXP sharedVector_subset(SEXP x, SEXP indx, SEXP call);
SEXP sharedVector_duplicate(SEXP x, Rboolean deep);
SEXP sharedVector_serialized_state(SEXP x);
SEXP sharedVector_unserialize(SEXP R_class, SEXP dataInfo);
