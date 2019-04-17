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
  PID pid=asInteger(R_pid);
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
SEXP getProcessList() {
	size_t processNum = getProcessNum();
	SEXP processList=PROTECT(allocVector(INTSXP, processNum));
	getProcessID(INTEGER(processList));
	UNPROTECT(1);
	return(processList);
}

SEXP getDataList(SEXP R_pid) {
	PID pid = asInteger(R_pid);
	size_t dataNum = getDataNum(pid);
	SEXP dataList = PROTECT(allocVector(INTSXP, dataNum));
	getDataID(pid,INTEGER(dataList));
	UNPROTECT(1);
	return(dataList);
}

SEXP createAltrep(SEXP R_address,SEXP R_type,SEXP R_length,SEXP R_size){
	Rprintf("creating state\n");
  SEXP state = PROTECT(make_sharedObject_state(R_type,R_length, R_size));
  int type=asInteger(R_type);
  R_altrep_class_t alt_class;
  switch(type) {
  case REAL_TYPE:
	  Rprintf("real type\n");
	  alt_class = shared_real_class;
    break;
  default: error("Type of %ul is not supported yet", type);
  }

  SEXP res = PROTECT(R_new_altrep(alt_class, R_address, state));

  Rprintf("altrep generated\n");

  UNPROTECT(2);
  return res;
}


SEXP clearAll() {
	destroyAllObj();
	return(R_NilValue);
}
SEXP clearObj(SEXP objIndex) {
	destroyObj(INTEGER(objIndex), length(objIndex));
	return(R_NilValue);
}
