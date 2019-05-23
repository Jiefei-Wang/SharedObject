#pragma once
#include <Rcpp.h>
#include "sharedObject_types.h"
using namespace Rcpp;


//[[Rcpp::init]]
void init_real_class(DllInfo* dll);
//[[Rcpp::init]]
void init_integer_class(DllInfo* dll);
//[[Rcpp::init]]
void init_logical_class(DllInfo* dll);
//[[Rcpp::init]]
void init_raw_class(DllInfo* dll);
//[[Rcpp::init]]
void init_str_class(DllInfo* dll);



// [[Rcpp::export]]
SEXP C_peekSharedMemory(SEXP x);



// [[Rcpp::export]]
SEXP C_testFunc(SEXP a);
// [[Rcpp::export]]
DID C_findAvailableKey(DID did);
// [[Rcpp::export]]
void C_createSharedMemory(SEXP R_x, SEXP R_dataInfo);
// [[Rcpp::export]]
SEXP C_readSharedMemory(double R_DID);
//altrep
// [[Rcpp::export]]
SEXP C_createAltrep(SEXP SM_obj);


// [[Rcpp::export]]
void C_clearObj(double did);



// [[Rcpp::export]]
std::vector<double> C_getDataID();


// [[Rcpp::export]]
NumericVector C_getDataInfo(DID did);


// [[Rcpp::export]]
SEXP C_attachAttr(SEXP R_source, SEXP R_tag, SEXP R_attr);

// [[Rcpp::export]]
bool C_ALTREP(SEXP x);


// [[Rcpp::export]]
bool C_getCopyOnWrite(DID did);
// [[Rcpp::export]]
bool C_getSharedSub(DID did);
// [[Rcpp::export]]
bool C_getSharedDuplicate(DID did);
// [[Rcpp::export]]
void C_setCopyOnWrite(DID did, bool value);
// [[Rcpp::export]]
void C_setSharedSub(DID did, bool value);
// [[Rcpp::export]]
void C_setSharedDuplicate(DID did, bool value);