#pragma once
#include <Rcpp.h>

SEXP C_createSharedMemory(SEXP x, Rcpp::List dataInfo);
SEXP C_readSharedMemory(SEXP dataInfo);
SEXP C_createEmptySharedMemory(Rcpp::List dataInfo);