#pragma once
#include <Rcpp.h>
#include "sharedObject_types.h"
using namespace Rcpp;


//[[Rcpp::init]]
void init_real_class(DllInfo* dll);
//[[Rcpp::init]]
void init_integer_class(DllInfo* dll);



// [[Rcpp::export]]
SEXP peekSharedMemory(SEXP x);



// [[Rcpp::export]]
void C_testFunc(SEXP a);
// [[Rcpp::export]]
DID C_createSharedMemory(SEXP R_x, int type, double total_size, double pid, double did);
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