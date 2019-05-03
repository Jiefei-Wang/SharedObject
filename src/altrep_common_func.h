#pragma once
#include <Rinternals.h>
#include <R_ext/Altrep.h>
#include "tools.h"
#include "sharedObject_types.h"
#include "altrep_macro.h"

Rboolean sharedObject_Inspect(SEXP x, int pre, int deep, int pvec,
                      void (*inspect_subtree)(SEXP, int, int, int));
R_altrep_class_t getAltClass(int type);

void* getPointer(SEXP x);

R_xlen_t sharedObject_length(SEXP x);
void *sharedObject_dataptr(SEXP x, Rboolean writeable);
const void *sharedObject_dataptr_or_null(SEXP x);
SEXP sharedObject_dulplicate(SEXP x, Rboolean deep);
//void sharedObject_updateAd(SEXP x);
SEXP sharedObject_serialized_state(SEXP x);
SEXP sharedObject_unserialize(SEXP R_class, SEXP state);


