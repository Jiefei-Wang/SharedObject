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

void copyData(SEXP source, void *target);
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
	//uint64_t dataSize = as<uint64_t>(dataInfo[INFO_TOTALSIZE]);
	SEXP R_type = PROTECT(Rf_ScalarReal(TYPEOF(x)));
	dataInfo[INFO_DATATYPE] = R_type;
	SEXP result = C_createEmptySharedMemory(dataInfo);
	copyData(x,ALT_EXTPTR(result));
	//memcpy(ALT_EXTPTR(result), DATAPTR(x), dataSize);
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
	if (own_data)
	{
		freeSharedMemory(id);
	}
	else
	{
		unmapSharedMemory(id);
	}
	DEBUG(Rprintf("Finalizer, id %d\n", id););
	return;
}

/*Copy data from SEXP to target*/
void copyData(SEXP source, void *target)
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
SEXP C_attachAttr(SEXP R_source, SEXP R_tag, SEXP R_attr)
{
	R_attr = PROTECT(Rf_duplicate(R_attr));
	const char *tag = R_CHAR(Rf_asChar(R_tag));
	Rf_setAttrib(R_source, Rf_install(tag), R_attr);
	UNPROTECT(1);
	return R_NilValue;
}

/*
##########################################
## Export sharedMemory function to R
##########################################
*/
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
	allocateSharedMemory(name.c_str(), size_in_byte);
}
// [[Rcpp::export]]
SEXP C_mapNamedSharedMemory(const string name)
{
	return R_MakeExternalPtr(mapSharedMemory(name.c_str()), R_NilValue, R_NilValue);
}
// [[Rcpp::export]]
bool C_unmapNamedSharedMemory(const string name)
{
	return unmapSharedMemory(name.c_str());
}
// [[Rcpp::export]]
bool C_freeNamedSharedMemory(const string name)
{
	return freeSharedMemory(name.c_str());
}
// [[Rcpp::export]]
bool C_hasNamedSharedMemory(const string name)
{
	return hasSharedMemory(name.c_str());
}

// [[Rcpp::export]]
double C_getNamedSharedMemorySize(const string name)
{
	return getSharedMemorySize(name.c_str());
}