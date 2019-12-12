#include <Rcpp.h>
#include "R_ext/Altrep.h"
#include "tools.h"
#include "sharedMemory.h"
#include "altrepMacro.h"
#include "altrep.h"
#include "C_interface.h"
#include "macro.h"

// [[Rcpp::plugins(unwindProtect)]]
using namespace Rcpp;
using std::string;


// [[Rcpp::export]]
SEXP C_createSharedMemory(SEXP x, List dataInfo) {
	uint64_t dataSize = as<uint64_t>(SLOT_TOTALSIZE(dataInfo));
	uint32_t id = allocateSharedMemory(dataSize);
	SET_SLOT_DATAID(dataInfo, Rf_ScalarReal(id));

	void* ptr = mapSharedMemory(id);
	SEXP sharedExtPtr = PROTECT(R_MakeExternalPtr(ptr, R_NilValue, R_NilValue));
	R_altrep_class_t alt_class = getAltClass(TYPEOF(x));
	SEXP res = PROTECT(R_new_altrep(alt_class, sharedExtPtr,dataInfo));
	UNPROTECT(2);
	return res;
}

// [[Rcpp::export]]
SEXP C_readSharedMemory(DID dataID) {
}



// [[Rcpp::export]]
SEXP C_getAltData1(SEXP x) {
	return R_altrep_data1(x);
}
// [[Rcpp::export]]
SEXP C_getAltData2(SEXP x) {
	return R_altrep_data2(x);
}


// [[Rcpp::export]]
void C_clearObj(double dataID) {
	try {
		destroyObject(dataID);
	}
	catch (const std::exception & ex) {
		errorHandle("Unexpected error in removing object: \n%s", ex.what());
	}
}

// [[Rcpp::export]]
SEXP C_attachAttr(SEXP R_source, SEXP R_tag, SEXP R_attr) {
	R_attr = PROTECT(Rf_duplicate(R_attr));
	const char* tag = R_CHAR(Rf_asChar(R_tag));
	Rf_setAttrib(R_source, Rf_install(tag), R_attr);
	UNPROTECT(1);
	return R_NilValue;
}

// [[Rcpp::export]]
bool C_ALTREP(SEXP x) {
	return ALTREP(x);
}

