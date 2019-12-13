#pragma once
#include <Rcpp.h>
#include <R_ext/Altrep.h>

//Get the ALTREP class object by type id
R_altrep_class_t getAltClass(int type);
