#pragma once
#include <Rcpp.h>
#include "SharedObject_types.h"
using namespace Rcpp;




SEXP C_getAltData1(SEXP x);

DID C_findAvailableKey(DID dataID);
SEXP C_createSharedMemory(SEXP R_x, SEXP R_dataInfo);
SEXP C_readSharedMemory(DID dataID, bool ownData);
//altrep
SEXP C_createAltrep(SEXP SM_obj);

void C_clearObj(double did);


std::vector<double> C_getDataIdList();


NumericVector C_getDataInfo(DID did);


SEXP C_attachAttr(SEXP R_source, SEXP R_tag, SEXP R_attr);

bool C_ALTREP(SEXP x);



SEXP C_testFunc(SEXP a);


