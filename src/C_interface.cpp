#include <Rcpp.h>
#include "R_ext/Altrep.h"
#include "tools.h"
#include "memoryManager.h"
#include "altrepMacro.h"
#include "altrep.h"
#include "C_interface.h"

// [[Rcpp::plugins(unwindProtect)]]
using namespace Rcpp;
using std::string;

//Put the declaration in the front
SEXP C_readSharedMemory(DID dataID, bool ownData);
SEXP makeExternalSharedPtr(void* ptr, DID dataId, bool ownData);
static void ptr_finalizer(SEXP extPtr);


// [[Rcpp::export]]
SEXP C_createSharedMemory(SEXP x, SEXP R_dataInfo) {
	//R_xlen_t len = Rf_xlength(R_x);
	DEBUG(Rprintf("Creating a shared memory object\n"));
	//Rf_PrintValue(R_x);
	dataInfo di;
#define X(id,type, name) di.name=REAL(R_dataInfo)[id];
	DATAINFO_FIELDS
#undef X
	const void* data = getPointer(x);
	//printf("get pointer%p\n", data);
	createSharedObject(data, di);
	return C_readSharedMemory(di.dataId, true);
}

// [[Rcpp::export]]
SEXP C_readSharedMemory(DID dataID, bool ownData) {
	DEBUG(Rprintf("reading the shared memory object, data ID: %llu\n", dataID));
	void* ptr = readSharedObject(dataID);
	SEXP exter_p = makeExternalSharedPtr(ptr, dataID, ownData);
	return(exter_p);
}


// [[Rcpp::export]]
SEXP C_createAltrep(SEXP dataReferenceInfo) {
	DEBUG(Rprintf("creating the altrep objec\n"));
	int type = DRI_TYPEID(dataReferenceInfo);
	DEBUG(Rprintf("type %d\n", type));
	R_altrep_class_t alt_class = getAltClass(type);
	DEBUG(Rprintf("get alt class\n"));
	SEXP altClassName = PROTECT(Rf_mkString("shared memory"));
	SEXP res = PROTECT(R_new_altrep(alt_class, dataReferenceInfo, altClassName));
	DEBUG(Rprintf("altrep generated with type %d\n", type));

	UNPROTECT(2);
	return res;
}

SEXP makeExternalSharedPtr(void* ptr, DID dataId, bool ownData) {
	NumericVector info = NumericVector::create(dataId, ownData);
	SEXP extPtr = PROTECT(R_MakeExternalPtr(ptr, wrap(info), R_NilValue));
	R_RegisterCFinalizer(extPtr, ptr_finalizer);
	UNPROTECT(1);
	return extPtr;
}

static void ptr_finalizer(SEXP extPtr) {
	NumericVector info = as<NumericVector>(R_ExternalPtrTag(extPtr));
	DID dataId = info[0];
	bool ownData = info[1];
	if (ownData) {
		DEBUG(Rprintf("finalizing data\n"));
		destroyObject(dataId);
	}
	else {
		DEBUG(Rprintf("nothing to finalize\n"));
	}
	return;
}


// [[Rcpp::export]]
SEXP C_getAltData1(SEXP x) {
	if (!ALTREP(x)) {
		return R_NilValue;
	}
	while (ALTREP(R_altrep_data1(x))) {
		x = R_altrep_data1(x);
	}
	return R_altrep_data1(x);
}
// [[Rcpp::export]]
SEXP C_getAltData2(SEXP x) {
	if (!ALTREP(x)) {
		return R_NilValue;
	}
	while (ALTREP(R_altrep_data1(x))) {
		x = R_altrep_data1(x);
	}
	return R_altrep_data2(x);
}


// [[Rcpp::export]]
DID C_findAvailableKey(DID dataID) {
	return findAvailableDataId(dataID);
}
// [[Rcpp::export]]
bool C_hasDataID(DID key) {
	return hasDataID(key);
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
std::vector<double> C_getDataIdList() {
	return getDataIdList();
}

// [[Rcpp::export]]
SEXP C_getDataInfo(DID dataID) {
	dataInfo& info = getDataInfo(dataID);
	NumericVector v(DATAINFO_FIELDS_NUMBER);
#define X(id,type, name) v[id]=info.name;
	DATAINFO_FIELDS
#undef X
	return wrap(v);
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


// [[Rcpp::export]]
DID C_getDataID(DID did) {
	dataInfo& info = getDataInfo(did);
	return info.dataId;
}
// [[Rcpp::export]]
PID C_getProcessID(DID did) {
	dataInfo& info = getDataInfo(did);
	return info.processId;
}
// [[Rcpp::export]]
int C_getTypeID(DID did) {
	dataInfo& info = getDataInfo(did);
	return info.typeId;
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
void C_setSharedSubset(DID did, bool value) {
	dataInfo& info = getDataInfo(did);
	info.sharedSubset = value;
}
// [[Rcpp::export]]
void C_setSharedCopy(DID did, bool value) {
	dataInfo& info = getDataInfo(did);
	info.sharedCopy = value;
}

