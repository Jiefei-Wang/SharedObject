#include <string>
#include "Rcpp.h"
#include "R_ext/Altrep.h"
#include "altrep.h"
#include "altrepMacro.h"
#include "utils.h"
using std::string;
using namespace Rcpp;

Rboolean sharedString_Inspect(SEXP x, int pre, int deep, int pvec,
                              void (*inspect_subtree)(SEXP, int, int, int))
{
    Rprintf(" (len=%llu, COW=%d) shared string object\n", Rf_xlength(x),
    as<bool>(GET_ALT_SLOT(x, STR_INFO_COPYONWRITE)));
    return TRUE;
}

R_xlen_t sharedString_length(SEXP x)
{
    R_xlen_t size = Rcpp::as<R_xlen_t>(GET_ALT_SLOT(x, STR_INFO_LENGTH));
    altrepPrint("string: accessing length:%llu\n", size);
    return size;
}



void *sharedString_dataptr(SEXP x, Rboolean writeable)
{
    altrepPrint("string: accessing data pointer for string\n");
    if (VECTOR_ELT(STR_ALT_DATA(x), STR_DATA_STRVEC) != R_NilValue)
    {
        return DATAPTR(VECTOR_ELT(STR_ALT_DATA(x), STR_DATA_STRVEC));
    }
    //Allocate the entire string vector
    SEXP stringVec = PROTECT(unshareString(x));
    SET_VECTOR_ELT(STR_ALT_DATA(x), STR_DATA_STRVEC, stringVec);
    UNPROTECT(1);
    return DATAPTR(stringVec);
}
const void *sharedString_dataptr_or_null(SEXP x)
{
    altrepPrint("string: accessing data pointer or null for string\n");
    if (VECTOR_ELT(STR_ALT_DATA(x), STR_DATA_STRVEC) != R_NilValue)
    {
        return DATAPTR(VECTOR_ELT(STR_ALT_DATA(x), STR_DATA_STRVEC));
    }else{
        return NULL;
    }
}

SEXP sharedString_elt(SEXP x, R_xlen_t i)
{
    altrepPrint("string: getting element %llu\n", (uint64_t)i);
    if (VECTOR_ELT(STR_ALT_DATA(x), STR_DATA_STRVEC) != R_NilValue)
    {
        return STRING_ELT(VECTOR_ELT(STR_ALT_DATA(x), STR_DATA_STRVEC),i);
    }
    SEXP sharedIndex = VECTOR_ELT(STR_ALT_DATA(x), STR_DATA_INDEX);
    SEXP charSet = VECTOR_ELT(STR_ALT_DATA(x), STR_DATA_CHARSET);
    void *indexPtr = DATAPTR(sharedIndex);
    size_t unitSize = Rcpp::as<size_t>(GET_ALT_SLOT(x, STR_INFO_UNITSIZE));
    switch (unitSize)
    {
    case 1:
        return STRING_ELT(charSet, ((uint8_t *)indexPtr)[i]);
    case 2:
        return STRING_ELT(charSet, ((uint16_t *)indexPtr)[i]);
    case 4:
        return STRING_ELT(charSet, ((uint32_t *)indexPtr)[i]);
    case 8:
        return STRING_ELT(charSet, ((uint64_t *)indexPtr)[i]);
    }
    return R_NilValue;
}

void sharedString_set_elt(SEXP x, R_xlen_t i, SEXP v)
{
    altrepPrint("string: Setting element %llu to <%s>\n", (uint64_t)i, DATAPTR(v));
    SEXP sharedIndex = VECTOR_ELT(STR_ALT_DATA(x), STR_DATA_INDEX);
    SEXP charSet = VECTOR_ELT(STR_ALT_DATA(x), STR_DATA_CHARSET);
    void *indexPtr = DATAPTR(sharedIndex);
    size_t unitSize = Rcpp::as<size_t>(GET_ALT_SLOT(x, STR_INFO_UNITSIZE));
    SEXP *iter_begin = (SEXP *)DATAPTR(charSet);
    SEXP *iter_end = (SEXP *)DATAPTR(charSet) + XLENGTH(charSet);
    SEXP *find_result = std::find(iter_begin, iter_end, v);
    if (find_result != iter_end)
    {
        size_t offset = find_result - iter_begin;
        switch (unitSize)
        {
        case 1:
            ((uint8_t *)indexPtr)[i] = offset;
            break;
        case 2:
            ((uint8_t *)indexPtr)[i] = offset;
            break;
        case 4:
            ((uint8_t *)indexPtr)[i] = offset;
            break;
        case 8:
            ((uint8_t *)indexPtr)[i] = offset;
            break;
        }
    }else{
        Rf_error("%s%s",
            "You cannot set the value of a shared string vector ",
            "to a CHARSXP that has not been presented in the string vector before!");
    }
}

SEXP sharedString_duplicate(SEXP x, Rboolean deep)
{
    bool copyOnWrite = as<bool>(GET_ALT_SLOT(x, STR_INFO_COPYONWRITE));
    altrepPrint("string: Duplicating data, deep: %d, copy on write: %d\n",
                deep, copyOnWrite);
    if (copyOnWrite)
    {
        return unshareString(x);
    }
    else
    {
        R_altrep_class_t alt_class = getAltClass(STRSXP);
        SEXP res = R_new_altrep(alt_class, STR_ALT_DATA(x), ALT_DATAINFO(x));
        return res;
    }
}


SEXP sharedString_serialized_state(SEXP x)
{
	altrepPrint("string: Serialize state\n");
    Rcpp::List info(2);
    info[0]= Rf_shallow_duplicate(STR_ALT_DATA(x));
    info[1]= ALT_DATAINFO(x);
    SET_VECTOR_ELT(info[0],STR_DATA_STRVEC,R_NilValue);
	return info;
}

SEXP sharedString_unserialize(SEXP R_class, SEXP info)
{
	altrepPrint("string: Unserializing data\n");
	loadLibrary();
	altrepPrint("Library loaded\n");
    SEXP slot1 = VECTOR_ELT(info,0);
    SEXP slot2 = VECTOR_ELT(info,1);
    if(GET_SLOT(slot1,STR_DATA_INDEX)!=R_NilValue){
        R_altrep_class_t alt_class = getAltClass(STRSXP);
        SEXP res = R_new_altrep(alt_class, slot1, slot2);
	    return res;
    }else{
        Rf_warning("Fail to unserialize STRSXP for its internal data is corrupted\n");
        return R_NilValue;
    }
}