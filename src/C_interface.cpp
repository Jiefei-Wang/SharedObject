#include <Rcpp.h>
#include "R_ext/Altrep.h"
#include "tools.h"
#include "sharedMemory.h"
#include "altrep.h"
#include "C_interface.h"
#include "macro.h"

// [[Rcpp::plugins(unwindProtect)]]
using namespace Rcpp;
using std::string;

static void ptrFinalizer(SEXP extPtr);


SEXP C_createEmptySharedMemory(List dataInfo) {
	uint64_t dataSize = as<uint64_t>(dataInfo[INFO_TOTALSIZE]);
	//Allocate the shared memory
	uint32_t id = allocateSharedMemory(dataSize);
	SEXP R_id = PROTECT(Rf_ScalarReal(id));
	dataInfo[INFO_DATAID] = R_id;
	//Map the shared memory to the current process
	void* ptr = mapSharedMemory(id);
	SEXP sharedExtPtr = PROTECT(R_MakeExternalPtr(ptr, R_id, R_NilValue));

	//Create altrep
	R_altrep_class_t alt_class = getAltClass(as<int>(dataInfo[INFO_DATATYPE]));
	SEXP res = PROTECT(R_new_altrep(alt_class, sharedExtPtr, dataInfo));
	if (as<bool>(dataInfo[INFO_OWNDATA]))
		R_RegisterCFinalizer(sharedExtPtr, ptrFinalizer);
	UNPROTECT(3);
	return res;
}
// [[Rcpp::export]]
SEXP C_createSharedMemory(SEXP x, List dataInfo) {
	uint64_t dataSize = as<uint64_t>(dataInfo[INFO_TOTALSIZE]);
	SEXP R_type = PROTECT(Rf_ScalarReal(TYPEOF(x)));
	dataInfo[INFO_DATATYPE] = R_type;
	SEXP result = C_createEmptySharedMemory(dataInfo);
	memcpy(ALT_EXTPTR(result), DATAPTR(x), dataSize);
	UNPROTECT(1);
	return result;
}






static void ptrFinalizer(SEXP extPtr) {
	uint32_t id = as<uint32_t>(R_ExternalPtrTag(extPtr));
	freeSharedMemory(id);
	DEBUG(Rprintf("finalizing data\n"));
	return;
}


// [[Rcpp::export]]
SEXP C_readSharedMemory(SEXP dataInfo) {
	SEXP R_id = GET_SLOT(dataInfo,INFO_DATAID);
	//Map the shared memory to the current process
	void* ptr = mapSharedMemory(as<uint32_t>(R_id));
	SEXP sharedExtPtr = PROTECT(R_MakeExternalPtr(ptr, R_id, R_NilValue));

	//Create altrep
	R_altrep_class_t alt_class = getAltClass(as<int>(GET_SLOT(dataInfo,INFO_DATATYPE)));
	SEXP res = PROTECT(R_new_altrep(alt_class, sharedExtPtr, dataInfo));
	if (as<bool>(GET_SLOT(dataInfo, INFO_OWNDATA)))
		R_RegisterCFinalizer(sharedExtPtr, ptrFinalizer);
	UNPROTECT(2);
	return res;
}

// [[Rcpp::export]]
bool C_hasSharedMemory(uint32_t id) {
	return hasSharedMemory(id);
}

// [[Rcpp::export]]
uint32_t C_getLastIndex() {
	return getLastIndex();
}
// [[Rcpp::export]]
double C_getSharedMemorySize(uint32_t id) {
	return getSharedMemorySize(id);
}




// [[Rcpp::export]]
bool C_ALTREP(SEXP x) {
	return ALTREP(x);
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
void C_setAltData1(SEXP x, SEXP data) {
	R_set_altrep_data1(x, data);
}
// [[Rcpp::export]]
void C_setAltData2(SEXP x, SEXP data) {
	R_set_altrep_data2(x, data);
}


// [[Rcpp::export]]
SEXP C_attachAttr(SEXP R_source, SEXP R_tag, SEXP R_attr) {
	R_attr = PROTECT(Rf_duplicate(R_attr));
	const char* tag = R_CHAR(Rf_asChar(R_tag));
	Rf_setAttrib(R_source, Rf_install(tag), R_attr);
	UNPROTECT(1);
	return R_NilValue;
}


