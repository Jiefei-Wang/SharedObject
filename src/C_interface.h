#pragma once
#include <Rcpp.h>
#include "SharedObject_types.h"


//allocate a shared memory space and copy the values to the memory.
SEXP C_createSharedMemory(SEXP R_x, SEXP R_dataInfo);
// Get the shared data pointer by data ID
SEXP C_readSharedMemory(DID dataID, bool ownData);
// Create the ALTREP object by the data reference info
SEXP C_createAltrep(SEXP dataReferenceInfo);

// Make a external pointer with a finalizer
SEXP makeExternalSharedPtr(void* ptr, DID dataId, bool ownData);
// Finalizer of a pointer of a shared memory
static void ptr_finalizer(SEXP extPtr);

// get the data 1 or 2 slot of an ALTREP
SEXP C_getAltData1(SEXP x);
SEXP C_getAltData2(SEXP x);

//Find an unused key, the search will start at dataID
DID C_findAvailableKey(DID dataID);

//Delete the shared memory by data ID
void C_clearObj(double dataID);

// Get a list of used data IDs
std::vector<double> C_getDataIdList();

// Get the data information that associated with data ID
SEXP C_getDataInfo(DID dataID);

// Attach an attribute to an object, internal usage only
SEXP C_attachAttr(SEXP R_source, SEXP R_tag, SEXP R_attr);

//Check whether an object is an ALTREP
bool C_ALTREP(SEXP x);



SEXP C_testFunc(SEXP a);


