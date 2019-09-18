#pragma once
#include <Rcpp.h>
#include <R_ext/Altrep.h>
#include "tools.h"
#include "SharedObject_types.h"

/*
Altrep common functions, functions include:

1. some ALTREP related functions

2. A collection of the functions that all ALTREP classes
have to registered with.
*/

//Get the pointer of an SEXP
//If the SEXP is STREXP, return itself.
const void* getPointer(SEXP x);

//Get the ALTREP class object by type id
//Type id is defined in tools.h
R_altrep_class_t getAltClass(int type);





extern R_altrep_class_t shared_real_class;
void init_real_class(DllInfo* dll);


extern R_altrep_class_t shared_integer_class;
void init_integer_class(DllInfo* dll);


extern R_altrep_class_t shared_logical_class;
void init_logical_class(DllInfo* dll);

extern R_altrep_class_t shared_raw_class;
void init_raw_class(DllInfo* dll);