#include <Rcpp.h>
#include "R_ext/Altrep.h"
#include "tools.h"
#include "memoryManager.h"
#include "altrepMacro.h"
#include "altrepCommonFunc.h"
#include "C_interface.h"

// [[Rcpp::plugins(unwindProtect)]]
using namespace Rcpp;
using std::string;


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
	return findAvailableKey(dataID);
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
	const char* tag = R_CHAR(Rf_asChar(R_tag));
	Rf_setAttrib(R_source, Rf_install(tag), R_attr);
	return R_NilValue;
}

// [[Rcpp::export]]
bool C_ALTREP(SEXP x) {
	return ALTREP(x);
}

// [[Rcpp::export]]
std::vector<double> C_getUsedKey() {
	return getUsedKey();
}




// [[Rcpp::export]]
SEXP C_testFunc(SEXP a)
{
	/* creating a pointer to a vector<int> */
	std::vector<int>* v = new std::vector<int>;
	v->push_back(1);
	v->push_back(2);

	Rcpp::XPtr< std::vector<int> > p(v, true);
	Environment package_env("SharedObject");
	return(package_env);
}




// [[Rcpp::export]]
SEXP C_test1(SEXP f, SEXP x) {
	SEXP call = PROTECT(Rf_lang2(f, x));
	SEXP val = R_forceAndCall(call, 1, R_GlobalEnv);
	UNPROTECT(1);
	return val;
}

// [[Rcpp::export]]
SEXP C_test2(SEXP expr, SEXP env) {
	SEXP val = Rf_eval(expr, env);
	return val;
}

// [[Rcpp::export]]
SEXP C_test3(SEXP f, SEXP x) {
	Function fun(f);
	return fun(x);
}

