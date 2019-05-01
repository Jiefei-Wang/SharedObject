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


DID C_createSharedMemory(SEXP R_x,int R_type, ULLong total_size, PID R_pid){
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
  DID did=createSharedOBJ(data, R_type, total_size, len, R_pid);
  return(did);
}

SEXP C_readSharedMemory(PID R_DID) {
	void* p = readSharedOBJ(R_DID);
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
  default: Rf_error("Type of %ul is not supported yet", type);
  }

  SEXP res = Rf_protect(R_new_altrep(alt_class, SM_obj, R_NilValue));

  //Rprintf("altrep generated\n");

  Rf_unprotect(1);
  return res;
}


void C_clearAll(bool verbose) {
	destroyAllObj(verbose);
}
void C_clearObj(DID did) {
	try {
		destroyObj(did);
	}
	catch (const std::exception& ex) {
		errorHandle(string("Unexpected error in removing object: \n") + ex.what());
	}
}


double C_getDataCount() {
	size_t count = getDataCount();
	return(count);
}

SEXP C_getFreedKeys() {
	size_t n = getFreedKeyNum();
	SEXP res = Rf_protect(Rf_allocVector(REALSXP, n));
	getFreedAllKeys(REAL(res));
	Rf_unprotect(1);
	return(res);
}


SEXP C_getProcessIDs() {
	size_t n = getProcessNum();
	SEXP processList = Rf_protect(Rf_allocVector(REALSXP, n));
	getProcessIDs(REAL(processList));
	Rf_unprotect(1);
	return(processList);
}

SEXP C_getDataIDs(PID pid) {
	size_t n = getDataNum(pid);
	SEXP dataList = Rf_protect(Rf_allocVector(REALSXP, n));
	getDataIDs(pid, REAL(dataList));
	Rf_unprotect(1);
	return(dataList);
}
SEXP C_getProcessInfo() {
	size_t n = getProcessNum();
	SEXP PIs = Rf_protect(Rf_allocVector(VECSXP, 3));
	SEXP pid = Rf_protect(C_getProcessIDs());
	SEXP dataNum = Rf_protect(Rf_allocVector(REALSXP, n));
	SEXP dataSize = Rf_protect(Rf_allocVector(REALSXP, n));
	//getProcessInfo(REAL(pid), REAL(dataNum), REAL(dataSize));
	for (size_t i = 0; i < n; i++) {
		const processInfo pi = getProcessInfo(REAL(pid)[i]);
		REAL(dataNum)[i] = pi.object_num;
		REAL(dataSize)[i] = pi.total_size;
	}

	SET_VECTOR_ELT(PIs, 0, pid);
	SET_VECTOR_ELT(PIs, 1, dataNum);
	SET_VECTOR_ELT(PIs, 2, dataSize);
	Rf_unprotect(4);
	return(PIs);
}


SEXP C_getDataInfo(PID pid) {
	size_t n = getDataNum(pid);
	SEXP DIs= Rf_protect(Rf_allocVector(VECSXP, 3));
	SEXP did = Rf_protect(C_getDataIDs(pid));
	SEXP size = Rf_protect(Rf_allocVector(REALSXP, n));
	SEXP type = Rf_protect(Rf_allocVector(REALSXP, n));

	for (unsigned int i = 0; i < n; i++) {
		const dataInfo di = getDataInfo(pid,REAL(did)[i]);
		REAL(size)[i] = di.size;
		REAL(type)[i] = di.type;
	}

	SET_VECTOR_ELT(DIs, 0, did);
	SET_VECTOR_ELT(DIs, 1, size);
	SET_VECTOR_ELT(DIs, 2, type);
	Rf_unprotect(4);
	return(DIs);
}

double C_getDataPID(DID did) {
	return getDataPID(did);
}

SEXP C_recoverDataInfo(DID did) {
	PID pid = getDataPID(did);
	dataInfo di = getDataInfo(pid, did);

	SEXP info = Rf_protect(Rf_allocVector(REALSXP, 3));
	REAL(info)[0] = di.size;
	REAL(info)[1] = di.length;
	REAL(info)[2] = di.type;
	Rf_unprotect(1);
	return info;
}

SEXP C_attachAttr(SEXP R_source, SEXP R_tag,SEXP R_attr) {
	const char* tag = R_CHAR(Rf_asChar(R_tag));
	Rf_setAttrib(R_source, Rf_install(tag), R_attr);
	return R_NilValue;
}


