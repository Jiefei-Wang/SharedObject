#include <R.h>
#include <Rinternals.h>
#include <R_ext/Altrep.h>
#define PGKNAME "sharedObject"

#define DEBUG(x) x;

#define SO_PTR(x) R_ExternalPtrAddr(SO_EPTR(x))
#define SO_EPTR(x) R_altrep_data1(x)
#define SO_STATE(x) R_altrep_data2(x)

#define SO_STATE_LENGTH(x) ((size_t) REAL_ELT(VECTOR_ELT(x,1), 0))
#define SO_STATE_SIZE(x) ((size_t) REAL_ELT(VECTOR_ELT(x,1), 1))

#define SO_LENGTH(x) SO_STATE_LENGTH(SO_STATE(x))
#define SO_SIZE(x) SO_STATE_SIZE(SO_STATE(x))

Rboolean sharedObject_Inspect(SEXP x, int pre, int deep, int pvec,
                      void (*inspect_subtree)(SEXP, int, int, int));

SEXP make_sharedObject_state( SEXP R_type,SEXP R_length,SEXP R_size);

R_xlen_t sharedObject_length(SEXP x);

void *sharedObject_dataptr(SEXP x, Rboolean writeable);
const void *sharedObject_dataptr_or_null(SEXP x);