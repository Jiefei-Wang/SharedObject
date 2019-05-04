#include <Rcpp.h>
using namespace Rcpp;
#include <Rinternals.h>
#include "R_ext/Altrep.h"
#include "tools.h"
#include "memoryManager.h"
#include "altrep_common_func.h"

using std::string;

SEXP peekSharedMemory(SEXP x) {
	SEXP sm = SO_ENV(x);
	return(sm);
}

SEXP C_testFunc(SEXP a)
{
	SEXP res = STRING_ELT(a, 1);
	messageHandle("len:%llu,true len: %llu\n", XLENGTH(res), XTRUELENGTH(res));
	return(res);
}


DID C_createSharedMemory(SEXP R_x,int type, double total_size, double pid, double did){
  R_xlen_t len = Rf_xlength(R_x);
  //Rprintf("length:%d,type: %d, total:%f, pid: %llu\n", len, type,total_size, pid);
  void* data = getPointer(R_x);
  //printf("get pointer%p\n", data);
  did=createSharedOBJ(data, type, total_size, len, pid,did);
  return(did);
}



SEXP C_readSharedMemory(double DID) {
	void* p = readSharedOBJ(DID);
	SEXP exter_p = R_MakeExternalPtr(p, R_NilValue, R_NilValue);
	return(exter_p);
}




//SEXP R_address,SEXP R_type,SEXP R_length,SEXP R_size
SEXP C_createAltrep(SEXP SM_obj){
  int type= Rf_asInteger(SM_DATA(SM_obj, type_id));
  Rprintf("type %d\n", type);
  R_altrep_class_t alt_class = getAltClass(type);
  Rprintf("get alt class\n");
  SEXP res = Rf_protect(R_new_altrep(alt_class, SM_obj, R_NilValue));
  Rprintf("altrep generated with type %d\n", type);

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
	dataInfo info= getDataInfo(did);
	NumericVector v = NumericVector::create(
		Named("DID") = did,
		Named("PID") = info.pid,
		Named("type") = info.type,
		Named("length") = info.length,
		Named("size") = info.size
		);
	return v;
}


SEXP C_attachAttr(SEXP R_source, SEXP R_tag,SEXP R_attr) {
	const char* tag = R_CHAR(Rf_asChar(R_tag));
	Rf_setAttrib(R_source, Rf_install(tag), R_attr);
	return R_NilValue;
}


