#include <Rcpp.h>
#include "R_ext/Altrep.h"
#include "R_ext/Itermacros.h"
#include "tools.h"
#include "sharedMemory.h"
#include "altrep.h"
#include "C_interface.h"
#include "macro.h"

// [[Rcpp::plugins(unwindProtect)]]
using namespace Rcpp;
using std::string;

/*
##########################################
## Create shared objects
##########################################
*/

void copyData(void *target, SEXP source);
static void ptrFinalizer(SEXP extPtr);
SEXP C_createEmptySharedMemory(List dataInfo)
{
	uint64_t dataSize = as<uint64_t>(dataInfo[INFO_TOTALSIZE]);
	//Allocate the shared memory
	uint32_t id = allocateSharedMemory(dataSize);
	SEXP R_id = PROTECT(Rf_ScalarReal(id));
	dataInfo[INFO_DATAID] = R_id;
	//Map the shared memory to the current process
	void *ptr = mapSharedMemory(id);
	SEXP sharedExtPtr = PROTECT(R_MakeExternalPtr(ptr, R_id, dataInfo[INFO_OWNDATA]));
	R_RegisterCFinalizerEx(sharedExtPtr, ptrFinalizer, TRUE);

	//Create altrep
	R_altrep_class_t alt_class = getAltClass(as<int>(dataInfo[INFO_DATATYPE]));
	SEXP res = PROTECT(R_new_altrep(alt_class, sharedExtPtr, dataInfo));
	UNPROTECT(3);
	return res;
}
// [[Rcpp::export]]
SEXP C_createSharedMemory(SEXP x, List dataInfo)
{
	SEXP R_type = PROTECT(Rf_ScalarReal(TYPEOF(x)));
	dataInfo[INFO_DATATYPE] = R_type;
	SEXP result = C_createEmptySharedMemory(dataInfo);
	// If x has data pointer, we just use memcpy function
	// Otherwise, we use get_region function to get the data from x
	if (DATAPTR_OR_NULL(x) != NULL)
	{
		uint64_t dataSize = as<uint64_t>(dataInfo[INFO_TOTALSIZE]);
		memcpy(DATAPTR(result), DATAPTR_OR_NULL(x), dataSize);
	}
	else
	{
		copyData(DATAPTR(result), x);
	}
	UNPROTECT(1);
	return result;
}

// [[Rcpp::export]]
SEXP C_readSharedMemory(SEXP dataInfo)
{
	SEXP R_id = GET_SLOT(dataInfo, INFO_DATAID);
	//Map the shared memory to the current process
	void *ptr = mapSharedMemory(as<uint32_t>(R_id));
	SEXP sharedExtPtr = PROTECT(R_MakeExternalPtr(ptr, R_id, GET_SLOT(dataInfo, INFO_OWNDATA)));
	R_RegisterCFinalizerEx(sharedExtPtr, ptrFinalizer, TRUE);

	//Create altrep
	R_altrep_class_t alt_class = getAltClass(as<int>(GET_SLOT(dataInfo, INFO_DATATYPE)));
	SEXP res = PROTECT(R_new_altrep(alt_class, sharedExtPtr, dataInfo));
	UNPROTECT(2);
	return res;
}

static void ptrFinalizer(SEXP extPtr)
{
	uint32_t id = as<uint32_t>(R_ExternalPtrTag(extPtr));
	bool own_data = as<bool>(R_ExternalPtrProtected(extPtr));
	
	DEBUG_SHARED_MEMORY(Rprintf("Finalizer, id:%d, own_data:%d\n", id,own_data));
	if (own_data)
	{
		freeSharedMemory(id);
	}
	else
	{
		unmapSharedMemory(id);
	}
	return;
}

/*
Copy data from source to target without using the 
data pointer of the source.
*/
void copyData(void *target, SEXP source)
{
	int data_type = TYPEOF(source);
	if (data_type == RAWSXP)
	{
		ITERATE_BY_REGION(source, buffer, ind, nbatch, Rbyte, RAW,
						  {
							  size_t size = sizeof(buffer[0]);
							  memcpy((Rbyte *)target + ind * nbatch, buffer, nbatch * size);
						  });
		return;
	}
	if (data_type == LGLSXP)
	{
		ITERATE_BY_REGION(source, buffer, ind, nbatch, int, LOGICAL,
						  {
							  size_t size = sizeof(buffer[0]);
							  memcpy((int *)target + ind * nbatch, buffer, nbatch * size);
						  });
		return;
	}
	if (data_type == INTSXP)
	{
		ITERATE_BY_REGION(source, buffer, ind, nbatch, int, INTEGER,
						  {
							  size_t size = sizeof(buffer[0]);
							  memcpy((int *)target + ind, buffer, nbatch * size);
						  });
		return;
	}

	if (data_type == REALSXP)
	{
		ITERATE_BY_REGION(source, buffer, ind, nbatch, double, REAL,
						  {
							  size_t size = sizeof(buffer[0]);
							  memcpy((double *)target + ind * nbatch, buffer, nbatch * size);
						  });
		return;
	}
	if (data_type == CPLXSXP)
	{
		ITERATE_BY_REGION(source, buffer, ind, nbatch, Rcomplex, COMPLEX,
						  {
							  size_t size = sizeof(buffer[0]);
							  memcpy((Rcomplex *)target + ind * nbatch, buffer, nbatch * size);
						  });
		return;
	}
}

/*Copy data from source to target*/
// [[Rcpp::export]]
void C_memcpy(SEXP source, SEXP target, R_xlen_t byteSize)
{
	void *sourcePtr = DATAPTR(source);
	void *targetPtr = DATAPTR(target);
	memcpy(targetPtr, sourcePtr, byteSize);
}

// [[Rcpp::export]]
bool C_isSameObject(SEXP x, SEXP y)
{
	return ((void *)x) == ((void *)y);
}

//Function to set the ownership of a shared object
// [[Rcpp::export]]
void C_setSharedObjectOwership(SEXP x, bool ownData){
  DEBUG_SHARED_MEMORY(Rprintf("set owndata :%d\n",ownData));
  SEXP sharedExtPtr = R_altrep_data1(x);
  R_SetExternalPtrProtected(sharedExtPtr, wrap(ownData));
}

// [[Rcpp::export]]
void C_setAttributes(SEXP x, SEXP attrs){
  SET_ATTRIB(x, attrs);
}

/*
##########################################
## ALTREP related C API
##########################################
*/
// [[Rcpp::export]]
bool C_ALTREP(SEXP x)
{
	return ALTREP(x);
}

// [[Rcpp::export]]
SEXP C_getAltData1(SEXP x)
{
	return R_altrep_data1(x);
}
// [[Rcpp::export]]
SEXP C_getAltData2(SEXP x)
{
	return R_altrep_data2(x);
}

// [[Rcpp::export]]
void C_setAltData1(SEXP x, SEXP data)
{
	R_set_altrep_data1(x, data);
}
// [[Rcpp::export]]
void C_setAltData2(SEXP x, SEXP data)
{
	R_set_altrep_data2(x, data);
}

// [[Rcpp::export]]
int C_getObject(SEXP x){
  return OBJECT(x);
}
// [[Rcpp::export]]
void C_setObject(SEXP x, int i){
  SET_OBJECT(x,i);
}
// [[Rcpp::export]]
bool C_ISS4(SEXP x){
  return IS_S4_OBJECT(x);
}
// [[Rcpp::export]]
void C_SETS4(SEXP x)
{
	SET_S4_OBJECT(x);
}
// [[Rcpp::export]]
void C_UNSETS4(SEXP x)
{
	UNSET_S4_OBJECT(x);
}
/*
##########################################
## Export sharedMemory function to R
##########################################
*/
// [[Rcpp::export]]
void C_initialPkgData(){
	initialPkgData();
}
// [[Rcpp::export]]
void C_releasePkgData(){
	releasePkgData();
}
// [[Rcpp::export]]
int32_t C_getLastIndex()
{
	return getLastIndex();
}
// [[Rcpp::export]]
uint32_t C_allocateSharedMemory(size_t size_in_byte)
{
	return allocateSharedMemory(size_in_byte);
}
// [[Rcpp::export]]
SEXP C_mapSharedMemory(uint32_t id)
{
	return R_MakeExternalPtr(mapSharedMemory(id), R_NilValue, R_NilValue);
}
// [[Rcpp::export]]
bool C_unmapSharedMemory(uint32_t id)
{
	return unmapSharedMemory(id);
}
// [[Rcpp::export]]
bool C_freeSharedMemory(uint32_t id)
{
	return freeSharedMemory(id);
}
// [[Rcpp::export]]
bool C_hasSharedMemory(uint32_t id)
{
	return hasSharedMemory(id);
}
// [[Rcpp::export]]
double C_getSharedMemorySize(uint32_t id)
{
	return getSharedMemorySize(id);
}

// [[Rcpp::export]]
void C_allocateNamedSharedMemory(const string name, size_t size_in_byte)
{
	allocateNamedSharedMemory(name.c_str(), size_in_byte);
}
// [[Rcpp::export]]
SEXP C_mapNamedSharedMemory(const string name)
{
	return R_MakeExternalPtr(mapNamedSharedMemory(name.c_str()), R_NilValue, R_NilValue);
}
// [[Rcpp::export]]
bool C_unmapNamedSharedMemory(const string name)
{
	return unmapNamedSharedMemory(name.c_str());
}
// [[Rcpp::export]]
bool C_freeNamedSharedMemory(const string name)
{
	return freeNamedSharedMemory(name.c_str());
}
// [[Rcpp::export]]
bool C_hasNamedSharedMemory(const string name)
{
	return hasNamedSharedMemory(name.c_str());
}

// [[Rcpp::export]]
double C_getNamedSharedMemorySize(const string name)
{
	return getNamedSharedMemorySize(name.c_str());
}