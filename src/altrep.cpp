#include <string>
#include <Rcpp.h>
#include <R_ext/Altrep.h>
#include "altrep.h"
#include "altrepMacro.h"
#include "utils.h"
#include "sharedMemory.h"

using namespace Rcpp;
using std::string;

R_altrep_class_t shared_real_class;
R_altrep_class_t shared_integer_class;
R_altrep_class_t shared_logical_class;
R_altrep_class_t shared_raw_class;
R_altrep_class_t shared_complex_class;
R_altrep_class_t shared_string_class;

Rcpp::CharacterVector dataInfoNames = Rcpp::CharacterVector::create(
		"dataId", "length", "totalSize", "dataType", "ownData",
		"copyOnWrite", "sharedSubset", "sharedCopy");

Rcpp::CharacterVector stringDataInfoNames = Rcpp::CharacterVector::create(
		"length", "unitSize", "totalSize", "dataType", "uniqueChar","copyOnWrite");

R_altrep_class_t& getAltClass(int type) {
	switch (type) {
	case REALSXP:
		return shared_real_class;
	case INTSXP:
		return shared_integer_class;
	case LGLSXP:
		return shared_logical_class;
	case RAWSXP:
		return shared_raw_class;
	case CPLXSXP:
		return shared_complex_class;
	case STRSXP:
		return shared_string_class;
	default: Rf_error("Type of %d is not supported yet", type);
	}
	// Just for suppressing the annoying warning, it should never be excuted
	return shared_real_class;
}
SEXP getDataInfoTemplate()
{
	List dataInfo(INFO_SLOT_NUM);
	dataInfo.names() = dataInfoNames;
	return dataInfo;
}
SEXP getStringDataInfoTemplate()
{
	List dataInfo(STR_INFO_SLOT_NUM);
	dataInfo.names() = stringDataInfoNames;
	return dataInfo;
}

/*
##########################################
## Create shared objects
##########################################
*/
static void ptrFinalizer(SEXP extPtr)
{
	string id = as<string>(R_ExternalPtrTag(extPtr));
	packagePrint("Finalizer, id:%s\n", id.c_str());
	unmapSharedMemory(id);
}
/*
Create new shared object and make it auto
release after use.
*/
SEXP createEmptySharedObject(int type, uint64_t length,
							   bool copyOnWrite, bool sharedSubset, bool sharedCopy,
							   SEXP attributes)
{
	uint64_t dataSize = length * getTypeSize(type);
	PROTECT_GUARD guard;
	List dataInfo = getDataInfoTemplate();
	dataInfo[INFO_LENGTH] = length;
	dataInfo[INFO_TOTALSIZE] = dataSize;
	dataInfo[INFO_DATATYPE] = type;
	dataInfo[INFO_OWNDATA] = true;
	dataInfo[INFO_COPYONWRITE] = copyOnWrite;
	dataInfo[INFO_SHAREDSUBSET] = sharedSubset;
	dataInfo[INFO_SHAREDCOPY] = sharedCopy;
	//Allocate the shared memory
	string id = allocateSharedMemory(dataSize);
	SEXP R_id = guard.protect(wrap(id));
	dataInfo[INFO_DATAID] = wrap(id);
	//Map the shared memory to the current process
	void *ptr = mapSharedMemory(id);
	//Release the memory when unmap it
	autoReleaseAfterUse(id, true);
	SEXP sharedExtPtr = guard.protect(R_MakeExternalPtr(ptr, R_id, R_NilValue));
	R_RegisterCFinalizerEx(sharedExtPtr, ptrFinalizer, TRUE);
	//Create altrep
	R_altrep_class_t alt_class = getAltClass(as<int>(dataInfo[INFO_DATATYPE]));
	SEXP res = guard.protect(R_new_altrep(alt_class, sharedExtPtr, dataInfo));
	SET_ATTRIB(res, attributes);
	return res;
}

// Attributes must be a pairlist
// [[Rcpp::export]]
SEXP createSharedObjectFromSource(
	SEXP x, bool copyOnWrite, bool sharedSubset, bool sharedCopy,
	SEXP attributes)
{
	int type = TYPEOF(x);
	uint64_t length = XLENGTH(x);
	uint64_t dataSize = getObjectSize(x);
	SEXP result = createEmptySharedObject(type, length,
											copyOnWrite, sharedSubset, sharedCopy,
											attributes);
	PROTECT(result);
	// If x has data pointer, we just use memcpy function
	// Otherwise, we use get_region function to get the data from x
	if (DATAPTR_OR_NULL(x) != NULL)
		memcpy(DATAPTR(result), DATAPTR(x), dataSize);
	else
		copyData(DATAPTR(result), x);
	UNPROTECT(1);
	return result;
}

// [[Rcpp::export]]
SEXP createSharedStringFromSource(SEXP x, bool copyOnWrite,	SEXP attributes)
{
	std::map<SEXP, size_t> uniqueCharSet;
	size_t length = XLENGTH(x);
	//Find all unique CHARSXPs
	for (size_t i = 0; i < length; i++)
	{
		SEXP curChar = STRING_ELT(x, i);
		if (uniqueCharSet.find(curChar) == uniqueCharSet.end())
		{
			uniqueCharSet.insert(std::pair<SEXP, size_t>(curChar, uniqueCharSet.size()));
		}
	}
	size_t unitBytes = std::ceil(std::log2(uniqueCharSet.size()) / 8);
	unitBytes = unitBytes==0?1:unitBytes;
	const size_t unitSize = std::pow(2, std::ceil(std::log2(unitBytes)));
	if (unitSize != 1 && unitSize != 2 && unitSize != 4 && unitSize != 8)
	{
		Rf_error("Something is wrong with the unit size: %llu\n", unitSize);
	}
	size_t totalSize = unitSize * length;
	PROTECT_GUARD guard;
	SEXP sharedIndex = guard.protect(createEmptySharedObject(RAWSXP, totalSize));
	SEXP charSet = guard.protect(Rf_allocVector(STRSXP, uniqueCharSet.size()));
	void *indexPtr = DATAPTR(sharedIndex);
	for (size_t i = 0; i < length; i++)
	{
		SEXP curChar = STRING_ELT(x, i);
		switch (unitSize)
		{
		case 1:
			((uint8_t *)indexPtr)[i] = uniqueCharSet.find(curChar)->second;
			break;
		case 2:
			((uint16_t *)indexPtr)[i] = uniqueCharSet.find(curChar)->second;
			break;
		case 4:
			((uint32_t *)indexPtr)[i] = uniqueCharSet.find(curChar)->second;
			break;
		case 8:
			((uint64_t *)indexPtr)[i] = uniqueCharSet.find(curChar)->second;
			break;
		}
	}
	for(auto& i: uniqueCharSet){
		SET_STRING_ELT(charSet,i.second,i.first);
	}
	Rcpp::List data(3);
	data[STR_DATA_STRVEC] = x;
	data[STR_DATA_INDEX] = sharedIndex;
	data[STR_DATA_CHARSET] = charSet;
	List dataInfo = getStringDataInfoTemplate();
	dataInfo[STR_INFO_LENGTH] = length;
	dataInfo[STR_INFO_UNITSIZE] = unitSize;
	dataInfo[STR_INFO_TOTALSIZE] = totalSize;
	dataInfo[STR_INFO_DATATYPE] = TYPEOF(x);
	dataInfo[STR_INFO_UNIQUECHAR] = uniqueCharSet.size();
	dataInfo[STR_INFO_COPYONWRITE] = copyOnWrite;
	R_altrep_class_t alt_class = getAltClass(TYPEOF(x));
	SEXP res = guard.protect(R_new_altrep(alt_class, data, dataInfo));
	SET_ATTRIB(res, attributes);
	return res;
}
SEXP readSharedObject(SEXP dataInfo)
{
	SEXP R_id = GET_SLOT(dataInfo, INFO_DATAID);
	//Map the shared memory to the current process
	void *ptr = mapSharedMemory(as<string>(R_id));
	SEXP sharedExtPtr = PROTECT(R_MakeExternalPtr(ptr, R_id, R_NilValue));
	R_RegisterCFinalizerEx(sharedExtPtr, ptrFinalizer, TRUE);

	//Create altrep
	R_altrep_class_t alt_class = getAltClass(as<int>(GET_SLOT(dataInfo, INFO_DATATYPE)));
	SEXP res = PROTECT(R_new_altrep(alt_class, sharedExtPtr, dataInfo));
	UNPROTECT(2);
	return res;
}

SEXP unshare(SEXP x, SEXP attributes)
{
	PROTECT_GUARD guard;
	SEXP res = guard.protect(Rf_allocVector(TYPEOF(x), XLENGTH(x)));
	memcpy(DATAPTR(res), DATAPTR(x), getObjectSize(x));
	SET_ATTRIB(res, attributes);
	return res;
}
//Create a regular string vector based on x
SEXP unshareString(SEXP x, SEXP attributes)
{
    SEXP sharedIndex = VECTOR_ELT(STR_ALT_DATA(x), STR_DATA_INDEX);
    SEXP charSet = VECTOR_ELT(STR_ALT_DATA(x), STR_DATA_CHARSET);
    void *indexPtr = DATAPTR(sharedIndex);
    const size_t unitSize = Rcpp::as<size_t>(GET_ALT_SLOT(x, STR_INFO_UNITSIZE));
    R_xlen_t length = XLENGTH(x);
    SEXP stringVec = PROTECT(Rf_allocVector(STRSXP, length));
    for (R_xlen_t i = 0; i < length; i++)
    {
        SEXP curChar;
        switch (unitSize)
        {
        case 1:
            curChar = STRING_ELT(charSet, ((uint8_t *)indexPtr)[i]);
            break;
        case 2:
            curChar = STRING_ELT(charSet, ((uint16_t *)indexPtr)[i]);
            break;
        case 4:
            curChar = STRING_ELT(charSet, ((uint32_t *)indexPtr)[i]);
            break;
        case 8:
            curChar = STRING_ELT(charSet, ((uint64_t *)indexPtr)[i]);
            break;
        }
        SET_STRING_ELT(stringVec, i, curChar);
    }
	SET_ATTRIB(stringVec, attributes);
    UNPROTECT(1);
    return stringVec;
}