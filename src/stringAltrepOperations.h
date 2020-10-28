#include <Rcpp.h>
Rboolean sharedString_Inspect(SEXP x, int pre, int deep, int pvec,
                              void (*inspect_subtree)(SEXP, int, int, int));
R_xlen_t sharedString_length(SEXP x);
void *sharedString_dataptr(SEXP x, Rboolean writeable);
const void *sharedString_dataptr_or_null(SEXP x);
SEXP sharedString_elt(SEXP x, R_xlen_t i);
void sharedString_set_elt(SEXP x, R_xlen_t i, SEXP v);
SEXP sharedString_duplicate(SEXP x, Rboolean deep);
SEXP sharedString_serialized_state(SEXP x);
SEXP sharedString_unserialize(SEXP R_class, SEXP info);
