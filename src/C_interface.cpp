#include <Rcpp.h>
using namespace Rcpp;
#include <Rinternals.h>
#include "R_ext/Altrep.h"
#include "tools.h"
#include "memoryManager.h"
#include "altrep_common_func.h"

using std::string;

SEXP C_peekSharedMemory(SEXP x) {
	if (!ALTREP(x)) {
		return R_NilValue;
	}
	while (ALTREP(x)) {
		x = R_altrep_data1(x);
	}
	return(x);
}

SEXP C_testFunc(SEXP a)
{
	STDVEC_DATAPTR(a);
	return(a);
}

DID C_findAvailableKey(DID did) {
	return findAvailableKey(did);
}

void C_createSharedMemory(SEXP R_x,SEXP R_dataInfo){
  //R_xlen_t len = Rf_xlength(R_x);
  //Rprintf("length:%d,type: %d, total:%f, pid: %llu\n", len, type,total_size, pid);
  //Rf_PrintValue(R_x);
  dataInfo di;
#define X(id,type, name) di.name=REAL(R_dataInfo)[id];
  DATAINFO_FIELDS
#undef X
  const void* data = getPointer(R_x);
  //printf("get pointer%p\n", data);
  createSharedOBJ(data, di);
}



SEXP C_readSharedMemory(double DID) {
	void* p = readSharedOBJ(DID);
	SEXP exter_p = R_MakeExternalPtr(p, R_NilValue, R_NilValue);
	return(exter_p);
}




//SEXP R_address,SEXP R_type,SEXP R_length,SEXP R_size
SEXP C_createAltrep(SEXP SM_obj){
  int type=SM_DATAINFO(SM_obj, type_id);
  DEBUG(Rprintf("type %d\n", type));
  R_altrep_class_t alt_class = getAltClass(type);
  DEBUG(Rprintf("get alt class\n"));
  SEXP res = Rf_protect(R_new_altrep(alt_class, SM_obj, R_NilValue));
  DEBUG(Rprintf("altrep generated with type %d\n", type));

  Rf_unprotect(1);
  return res;
}


void C_clearObj(double did) {
	try {
		destroyObj(did);
	}
	catch (const std::exception& ex) {
		errorHandle("Unexpected error in removing object: \n%s" , ex.what());
	}
}

std::vector<double> C_getDataID() {
	return getDataID();
}
NumericVector C_getDataInfo(DID did) {
	dataInfo& info= getDataInfo(did);
	NumericVector v(DATAINFO_FIELDS_NUMBER);
#define X(id,type, name) v[id]=info.name;
	DATAINFO_FIELDS
#undef X
	return v;
}


SEXP C_attachAttr(SEXP R_source, SEXP R_tag,SEXP R_attr) {
	const char* tag = R_CHAR(Rf_asChar(R_tag));
	Rf_setAttrib(R_source, Rf_install(tag), R_attr);
	return R_NilValue;
}


bool C_ALTREP(SEXP x) {
	return ALTREP(x);
}


bool C_getCopyOnWrite(DID did) {
	dataInfo& info = getDataInfo(did);
	return info.copyOnWrite;
}
bool C_getSharedSub(DID did) {
	dataInfo& info = getDataInfo(did);
	return info.sharedSub;
}
bool C_getSharedDuplicate(DID did) {
	dataInfo& info = getDataInfo(did);
	return info.sharedDuplicate;
}

void C_setCopyOnWrite(DID did, bool value) {
	dataInfo& info = getDataInfo(did);
	info.copyOnWrite = value;
}
void C_setSharedSub(DID did, bool value) {
	dataInfo& info = getDataInfo(did);
	info.sharedSub = value;
}
void C_setSharedDuplicate(DID did, bool value) {
	dataInfo& info = getDataInfo(did);
	info.sharedDuplicate = value;
}