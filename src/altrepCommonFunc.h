#pragma once
#include <Rinternals.h>
#include <R_ext/Altrep.h>
#include "tools.h"
#include "sharedObject_types.h"


Rboolean sharedVector_Inspect(SEXP x, int pre, int deep, int pvec,
                      void (*inspect_subtree)(SEXP, int, int, int));
R_altrep_class_t getAltClass(int type);

const void* getPointer(SEXP x);

R_xlen_t sharedVector_length(SEXP x);
void *sharedVector_dataptr(SEXP x, Rboolean writeable);
const void *sharedVector_dataptr_or_null(SEXP x);
SEXP sharedVector_duplicate(SEXP x, Rboolean deep);
SEXP sharedVector_serialized_state(SEXP x);
SEXP sharedVector_unserialize(SEXP R_class, SEXP state);


