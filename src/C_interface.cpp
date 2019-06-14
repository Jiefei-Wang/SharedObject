#include <Rcpp.h>
using namespace Rcpp;
#include "R_ext/Altrep.h"
#include "tools.h"
#include "memoryManager.h"
#include "altrepMacro.h"
#include "altrepCommonFunc.h"

using std::string;

// [[Rcpp::export]]
SEXP C_getSharedProperty(SEXP x) {
	if (!ALTREP(x)) {
		return R_NilValue;
	}
	while (ALTREP(R_altrep_data1(x))) {
		x = R_altrep_data1(x);
	}
	SEXP altClassName = R_altrep_data2(x);
	if (TYPEOF(altClassName)!=STRSXP||as<string>(altClassName)!="shared memory") {
		x = R_NilValue;
	}
	return R_altrep_data1(x);
}

// [[Rcpp::export]]
SEXP C_testFunc(SEXP a)
{
	Environment package_env("SharedObject");
	return(package_env);
}
// [[Rcpp::export]]
SEXP C_testFunc2(SEXP a)
{
	Environment package_env(R_FindNamespace(Rf_mkString("SharedObject")));
	return(package_env);
}

// [[Rcpp::export]]
SEXP C_testFunc3(SEXP a)
{
	Function getSharedParms(PACKAGE_FUNC(".createInheritedParms"));
	return(getSharedParms);
}

// [[Rcpp::export]]
SEXP C_testFunc4(SEXP a)
{
	Environment package_env(R_FindNamespace(Rf_mkString("SharedObject")));
	Function getSharedParms = package_env[".createInheritedParms"];
	return(getSharedParms);
}




// [[Rcpp::export]]
DID C_findAvailableKey(DID dataID) {
	return findAvailableKey(dataID);
}

// [[Rcpp::export]]
void C_createSharedMemory(SEXP R_x, SEXP R_dataInfo) {
	//R_xlen_t len = Rf_xlength(R_x);
	DEBUG(Rprintf("Creating a shared memory object\n"));
	//Rf_PrintValue(R_x);
	dataInfo di;
#define X(id,type, name) di.name=REAL(R_dataInfo)[id];
	DATAINFO_FIELDS;
#undef X
	const void* data = getPointer(R_x);
	//printf("get pointer%p\n", data);
	createSharedObject(data, di);
}



// [[Rcpp::export]]
SEXP C_readSharedMemory(DID dataID) {
	DEBUG(Rprintf("reading the shared memory object, data ID: %llu\n", dataID));
	void* p = readSharedObject(dataID);
	SEXP exter_p = R_MakeExternalPtr(p, R_NilValue, R_NilValue);
	return(exter_p);
}




// [[Rcpp::export]]
SEXP C_createAltrep(SEXP SM_obj) {
	DEBUG(Rprintf("creating the altrep objec\n"));
	int type = SM_TYPEID(SM_obj);
	DEBUG(Rprintf("type %d\n", type));
	R_altrep_class_t alt_class = getAltClass(type);
	DEBUG(Rprintf("get alt class\n"));
	SEXP altClassName = PROTECT(Rf_mkString("shared memory"));
	SEXP res = PROTECT(R_new_altrep(alt_class, SM_obj, altClassName));
	DEBUG(Rprintf("altrep generated with type %d\n", type));

	UNPROTECT(2);
	return res;
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
std::vector<double> C_getDataIDList() {
	return getDataIDList();
}
// [[Rcpp::export]]
NumericVector C_getDataInfo(DID dataID) {
	dataInfo& info = getDataInfo(dataID);
	NumericVector v(DATAINFO_FIELDS_NUMBER);
#define X(id,type, name) v[id]=info.name;
	DATAINFO_FIELDS;
#undef X
	return v;
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