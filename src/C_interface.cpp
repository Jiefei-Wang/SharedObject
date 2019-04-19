#include "C_interface.h"
#include "tools.h"
#include "memoryManager.h"
#include "altrep_real_class.h"
using std::string;
SEXP testFunc(SEXP x)
{
  showDataInfo(asInteger(x));
  return(x);
}


SEXP createSharedMemory(SEXP R_x,SEXP R_type, SEXP R_total_size,SEXP R_pid){
  ULLong total_size=asReal(R_total_size);
  PID pid=asReal(R_pid);
  //Rprintf("total:%d\n",total_size);
  void* data = nullptr;
  switch (TYPEOF(R_x)) {
  case LGLSXP:
    data=LOGICAL(R_x);
    break;
  case INTSXP:
    //printf("int type\n");
    data = INTEGER(R_x);
    break;
  case REALSXP:
    data = REAL(R_x);
    break;
  }
  DID did=createSharedOBJ(data,asInteger(R_type), total_size, pid);
  return(ScalarReal((double)did));
}


SEXP readSharedMemory(SEXP R_DID) {
	void* p = readSharedOBJ(asInteger(R_DID));
	SEXP exter_p = R_MakeExternalPtr(p, R_NilValue, R_NilValue);
	return(exter_p);
}


SEXP getValue_32(SEXP data, SEXP type_id,SEXP i){
  void* p = R_ExternalPtrAddr(data);
  //
  int dataType=asInteger(type_id);
  //printf("The value of p is: %p\n", p);
  int* ind=INTEGER(i);
  int n = length(i);
  //Allocate the output vector
  SEXP out;
  switch(dataType){
  case BOOL_TYPE:
    out=PROTECT(allocVector(LGLSXP, n));
    break;
  case INT_TYPE:
    out=PROTECT(allocVector(INTSXP, n));
    break;
  case REAL_TYPE:
    out=PROTECT(allocVector(REALSXP, n));
    break;
  default:
    errorHandle("Unexpected data type");
  }

  for(int j=0;j<n;j++){
    switch (dataType) {
    case BOOL_TYPE:
      LOGICAL(out)[j]=((int*)p)[ind[j]];
      break;
    case INT_TYPE:
      INTEGER(out)[j]=((int*)p)[ind[j]];
      break;
    case REAL_TYPE:
      REAL(out)[j]=((double*)p)[ind[j]];
      break;
    }
  }
  UNPROTECT(1);
   return(out);
}

SEXP createAltrep(SEXP R_address,SEXP R_type,SEXP R_length,SEXP R_size){
	//Rprintf("creating state\n");
  SEXP state = PROTECT(make_sharedObject_state(R_type,R_length, R_size));
  int type=asInteger(R_type);
  R_altrep_class_t alt_class;
  switch(type) {
  case REAL_TYPE:
	  //Rprintf("real type\n");
	  alt_class = shared_real_class;
    break;
  default: error("Type of %ul is not supported yet", type);
  }

  SEXP res = PROTECT(R_new_altrep(alt_class, R_address, state));

  //Rprintf("altrep generated\n");

  UNPROTECT(2);
  return res;
}


SEXP clearAll(SEXP output) {
	destroyAllObj(asLogical(output));
	return(R_NilValue);
}
SEXP clearObj(SEXP objID) {
	try {
		destroyObj(asReal(objID));
	}
	catch (const std::exception& ex) {
		errorHandle(string("Unexpected error in removing object: \n") + ex.what());
	}
	return(R_NilValue);
}


SEXP R_getDataCount() {
	size_t count = getDataCount();
	return(ScalarInteger(count));
}

SEXP R_getFreedKeys() {
	size_t n = getFreedKeyNum();
	SEXP res = PROTECT(allocVector(REALSXP, n));
	getFreedAllKeys(REAL(res));
	UNPROTECT(1);
	return(res);
}


SEXP R_getProcessIDs() {
	size_t n = getProcessNum();
	SEXP processList = PROTECT(allocVector(REALSXP, n));
	getProcessIDs(REAL(processList));
	UNPROTECT(1);
	return(processList);
}

SEXP R_getDataIDs(SEXP R_pid) {
	PID pid = asReal(R_pid);
	size_t n = getDataNum(pid);
	SEXP dataList = PROTECT(allocVector(REALSXP, n));
	getDataIDs(pid, REAL(dataList));
	UNPROTECT(1);
	return(dataList);
}
SEXP R_getProcessInfo() {
	size_t n = getProcessNum();
	SEXP PIs = PROTECT(allocVector(VECSXP, 3));
	SEXP pid = PROTECT(R_getProcessIDs());
	SEXP dataNum = PROTECT(allocVector(REALSXP, n));
	SEXP dataSize = PROTECT(allocVector(REALSXP, n));
	//getProcessInfo(REAL(pid), REAL(dataNum), REAL(dataSize));
	for (int i = 0; i < n; i++) {
		const processInfo pi = getProcessInfo(REAL(pid)[i]);
		REAL(dataNum)[i] = pi.object_num;
		REAL(dataSize)[i] = pi.total_size;
	}

	SET_VECTOR_ELT(PIs, 0, pid);
	SET_VECTOR_ELT(PIs, 1, dataNum);
	SET_VECTOR_ELT(PIs, 2, dataSize);
	UNPROTECT(4);
	return(PIs);
}


SEXP R_getDataInfo(SEXP R_pid) {
	PID pid = asReal(R_pid);
	size_t n = getDataNum(pid);
	SEXP DIs= PROTECT(allocVector(VECSXP, 3));
	SEXP did = PROTECT(R_getDataIDs(R_pid));
	SEXP size = PROTECT(allocVector(REALSXP, n));
	SEXP type = PROTECT(allocVector(REALSXP, n));

	for (int i = 0; i < n; i++) {
		const dataInfo di = getDataInfo(pid,REAL(did)[i]);
		REAL(size)[i] = di.size;
		REAL(type)[i] = di.type;
	}

	SET_VECTOR_ELT(DIs, 0, did);
	SET_VECTOR_ELT(DIs, 1, size);
	SET_VECTOR_ELT(DIs, 2, type);
	UNPROTECT(4);
	return(DIs);
}

SEXP R_getDataPID(SEXP R_did) {
	return ScalarReal(getDataPID(asReal(R_did)));
}

SEXP R_recoverDataInfo(SEXP R_did) {
	DID did = asReal(R_did);
	PID pid = getDataPID(did);
	dataInfo di = getDataInfo(pid, did);

	SEXP info = PROTECT(allocVector(REALSXP, 2));
	REAL(info)[0] = di.size;
	REAL(info)[1] = di.type;
	unprotect(1);
	return info;
}

SEXP attachAttr(SEXP R_source, SEXP R_tag,SEXP R_attr) {
	const char* tag = CHAR(asChar(R_tag));
	setAttrib(R_source, install(tag), R_attr);
	return R_NilValue;
}