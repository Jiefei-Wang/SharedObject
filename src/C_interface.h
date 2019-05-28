#pragma once
#include <Rcpp.h>
#include "sharedObject_types.h"
using namespace Rcpp;





SEXP C_peekSharedMemory(SEXP x);



SEXP C_testFunc(SEXP a);
DID C_findAvailableKey(DID dataID);
void C_createSharedMemory(SEXP R_x, SEXP R_dataInfo);
SEXP C_readSharedMemory(DID dataID);
//altrep
SEXP C_createAltrep(SEXP SM_obj);

void C_clearObj(double did);


std::vector<double> C_getDataIDList();


NumericVector C_getDataInfo(DID did);


SEXP C_attachAttr(SEXP R_source, SEXP R_tag, SEXP R_attr);

bool C_ALTREP(SEXP x);



