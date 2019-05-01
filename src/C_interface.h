#pragma once
#include <Rcpp.h>
#include "sharedObject_types.h"
using namespace Rcpp;


//[[Rcpp::init]]
void InitRealClass(DllInfo* dll);


// [[Rcpp::export]]
SEXP C_testFunc(S4 a);
// [[Rcpp::export]]
DID C_createSharedMemory(SEXP R_x, int R_type, ULLong total_size, PID R_pid);
// [[Rcpp::export]]
SEXP C_readSharedMemory(PID R_DID);
//altrep
// [[Rcpp::export]]
SEXP C_createAltrep(SEXP SM_obj);


// [[Rcpp::export]]
void C_clearAll(bool verbose);
// [[Rcpp::export]]
void C_clearObj(DID did);



// [[Rcpp::export]]
double C_getDataCount();

// [[Rcpp::export]]
SEXP C_getFreedKeys();


// [[Rcpp::export]]
SEXP C_getProcessIDs();
// [[Rcpp::export]]
SEXP C_getDataIDs(PID pid);
// [[Rcpp::export]]
SEXP C_getProcessInfo();
// [[Rcpp::export]]
SEXP C_getDataInfo(PID pid);
// [[Rcpp::export]]
double C_getDataPID(DID did);
// [[Rcpp::export]]
SEXP C_recoverDataInfo(DID did);

// [[Rcpp::export]]
SEXP C_attachAttr(SEXP R_source, SEXP R_tag, SEXP R_attr);