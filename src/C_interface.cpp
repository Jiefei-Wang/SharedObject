#include <Rcpp.h>
using namespace Rcpp;
#include "R_ext/libextern.h"
#include <R.h>
#include <Rinternals.h>
#include "R_ext/Altrep.h"
#include "tools.h"
#include "memoryManager.h"
#include "altrep_real_class.h"
using std::string;



SEXP C_testFunc(S4 a)
{
	Function f=REF_SLOT(a, "hello");
	f();
	return(R_NilValue);
}


DID C_createSharedMemory(SEXP R_x,int type, double total_size, double pid, double did){
  R_xlen_t len = Rf_xlength(R_x);
  //Rprintf("type: %d, total:%llu, pid: %llu\n", R_type,total_size, R_pid);
  void* data = nullptr;
  switch (TYPEOF(R_x)) {
  case LGLSXP:
    data=LOGICAL(R_x);
    break;
  case INTSXP:
    data = INTEGER(R_x);
    break;
  case REALSXP:
    data = REAL(R_x);
    break;
  default:
	  errorHandle("Unsupported data type\n");
  }
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
	//Rprintf("creating state\n");
  int type= Rf_asInteger(SM_DATA(SM_obj, type_id));
  R_altrep_class_t alt_class;
  switch(type) {
  case REAL_TYPE:
	  alt_class = shared_real_class;
    break;
  default: Rf_error("Type of %d is not supported yet", type);
  }

  SEXP res = Rf_protect(R_new_altrep(alt_class, SM_obj, R_NilValue));

  //Rprintf("altrep generated\n");

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


