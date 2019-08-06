/*
Altrep common functions, functions include:

1. some ALTREP related functions

2. A collection of the functions that all ALTREP classes
have to registered with.
*/

#pragma once
#include <Rinternals.h>
#include <R_ext/Altrep.h>
#include "tools.h"
#include "SharedObject_types.h"


//Get the pointer of an SEXP
//If the SEXP is STREXP, return itself.
const void* getPointer(SEXP x);

//Get the ALTREP class object by type id
//Type id is defined in tools.h
R_altrep_class_t getAltClass(int type);



// ALTREP basic function
Rboolean sharedVector_Inspect(SEXP x, int pre, int deep, int pvec,
                      void (*inspect_subtree)(SEXP, int, int, int));
R_xlen_t sharedVector_length(SEXP x);
void *sharedVector_dataptr(SEXP x, Rboolean writeable);
const void *sharedVector_dataptr_or_null(SEXP x);
SEXP sharedVector_duplicate(SEXP x, Rboolean deep);
SEXP sharedVector_serialized_state(SEXP x);
SEXP sharedVector_unserialize(SEXP R_class, SEXP state);


