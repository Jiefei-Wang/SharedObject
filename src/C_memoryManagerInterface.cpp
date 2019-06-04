#include <Rcpp.h>
using namespace Rcpp;
#include <Rinternals.h>
#include "tools.h"
#include "memoryManager.h"

// [[Rcpp::export]]
DID C_getDataID(DID did) {
	dataInfo& info = getDataInfo(did);
	return info.dataID;
}
// [[Rcpp::export]]
PID C_getProcessID(DID did) {
	dataInfo& info = getDataInfo(did);
	return info.processID;
}
// [[Rcpp::export]]
int C_getTypeID(DID did) {
	dataInfo& info = getDataInfo(did);
	return info.typeID;
}
// [[Rcpp::export]]
ULLong C_getLength(DID did) {
	dataInfo& info = getDataInfo(did);
	return info.length;
}
// [[Rcpp::export]]
ULLong C_getTotalSize(DID did) {
	dataInfo& info = getDataInfo(did);
	return info.totalSize;
}
// [[Rcpp::export]]
bool C_getCopyOnWrite(DID did) {
	dataInfo& info = getDataInfo(did);
	return info.copyOnWrite;
}
// [[Rcpp::export]]
bool C_getSharedSubset(DID did) {
	dataInfo& info = getDataInfo(did);
	return info.sharedSubset;
}
// [[Rcpp::export]]
bool C_getSharedCopy(DID did) {
	dataInfo& info = getDataInfo(did);
	return info.sharedCopy;
}


// [[Rcpp::export]]
void C_setCopyOnWrite(DID did, bool value) {
	dataInfo& info = getDataInfo(did);
	info.copyOnWrite = value;
}
// [[Rcpp::export]]
void C_setSharedSub(DID did, bool value) {
	dataInfo& info = getDataInfo(did);
	info.sharedSubset = value;
}
// [[Rcpp::export]]
void C_setSharedCopy(DID did, bool value) {
	dataInfo& info = getDataInfo(did);
	info.sharedCopy = value;
}

