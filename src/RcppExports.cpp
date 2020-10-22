// Generated by using Rcpp::compileAttributes() -> do not edit by hand
// Generator token: 10BE3573-1514-4C36-9D1C-5A225CD40393

#include "SharedObject_types.h"
#include <Rcpp.h>

using namespace Rcpp;

// C_createSharedMemory
SEXP C_createSharedMemory(SEXP x, List dataInfo);
RcppExport SEXP _SharedObject_C_createSharedMemory(SEXP xSEXP, SEXP dataInfoSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< SEXP >::type x(xSEXP);
    Rcpp::traits::input_parameter< List >::type dataInfo(dataInfoSEXP);
    rcpp_result_gen = Rcpp::wrap(C_createSharedMemory(x, dataInfo));
    return rcpp_result_gen;
END_RCPP
}
// C_readSharedMemory
SEXP C_readSharedMemory(SEXP dataInfo);
RcppExport SEXP _SharedObject_C_readSharedMemory(SEXP dataInfoSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< SEXP >::type dataInfo(dataInfoSEXP);
    rcpp_result_gen = Rcpp::wrap(C_readSharedMemory(dataInfo));
    return rcpp_result_gen;
END_RCPP
}
// C_memcpy
void C_memcpy(SEXP source, SEXP target, R_xlen_t byteSize);
RcppExport SEXP _SharedObject_C_memcpy(SEXP sourceSEXP, SEXP targetSEXP, SEXP byteSizeSEXP) {
BEGIN_RCPP
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< SEXP >::type source(sourceSEXP);
    Rcpp::traits::input_parameter< SEXP >::type target(targetSEXP);
    Rcpp::traits::input_parameter< R_xlen_t >::type byteSize(byteSizeSEXP);
    C_memcpy(source, target, byteSize);
    return R_NilValue;
END_RCPP
}
// C_isSameObject
bool C_isSameObject(SEXP x, SEXP y);
RcppExport SEXP _SharedObject_C_isSameObject(SEXP xSEXP, SEXP ySEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< SEXP >::type x(xSEXP);
    Rcpp::traits::input_parameter< SEXP >::type y(ySEXP);
    rcpp_result_gen = Rcpp::wrap(C_isSameObject(x, y));
    return rcpp_result_gen;
END_RCPP
}
// C_setSharedObjectOwership
void C_setSharedObjectOwership(SEXP x, bool ownData);
RcppExport SEXP _SharedObject_C_setSharedObjectOwership(SEXP xSEXP, SEXP ownDataSEXP) {
BEGIN_RCPP
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< SEXP >::type x(xSEXP);
    Rcpp::traits::input_parameter< bool >::type ownData(ownDataSEXP);
    C_setSharedObjectOwership(x, ownData);
    return R_NilValue;
END_RCPP
}
// C_setAttributes
void C_setAttributes(SEXP x, SEXP attrs);
RcppExport SEXP _SharedObject_C_setAttributes(SEXP xSEXP, SEXP attrsSEXP) {
BEGIN_RCPP
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< SEXP >::type x(xSEXP);
    Rcpp::traits::input_parameter< SEXP >::type attrs(attrsSEXP);
    C_setAttributes(x, attrs);
    return R_NilValue;
END_RCPP
}
// C_ALTREP
bool C_ALTREP(SEXP x);
RcppExport SEXP _SharedObject_C_ALTREP(SEXP xSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< SEXP >::type x(xSEXP);
    rcpp_result_gen = Rcpp::wrap(C_ALTREP(x));
    return rcpp_result_gen;
END_RCPP
}
// C_getAltData1
SEXP C_getAltData1(SEXP x);
RcppExport SEXP _SharedObject_C_getAltData1(SEXP xSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< SEXP >::type x(xSEXP);
    rcpp_result_gen = Rcpp::wrap(C_getAltData1(x));
    return rcpp_result_gen;
END_RCPP
}
// C_getAltData2
SEXP C_getAltData2(SEXP x);
RcppExport SEXP _SharedObject_C_getAltData2(SEXP xSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< SEXP >::type x(xSEXP);
    rcpp_result_gen = Rcpp::wrap(C_getAltData2(x));
    return rcpp_result_gen;
END_RCPP
}
// C_setAltData1
void C_setAltData1(SEXP x, SEXP data);
RcppExport SEXP _SharedObject_C_setAltData1(SEXP xSEXP, SEXP dataSEXP) {
BEGIN_RCPP
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< SEXP >::type x(xSEXP);
    Rcpp::traits::input_parameter< SEXP >::type data(dataSEXP);
    C_setAltData1(x, data);
    return R_NilValue;
END_RCPP
}
// C_setAltData2
void C_setAltData2(SEXP x, SEXP data);
RcppExport SEXP _SharedObject_C_setAltData2(SEXP xSEXP, SEXP dataSEXP) {
BEGIN_RCPP
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< SEXP >::type x(xSEXP);
    Rcpp::traits::input_parameter< SEXP >::type data(dataSEXP);
    C_setAltData2(x, data);
    return R_NilValue;
END_RCPP
}
// C_getObject
int C_getObject(SEXP x);
RcppExport SEXP _SharedObject_C_getObject(SEXP xSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< SEXP >::type x(xSEXP);
    rcpp_result_gen = Rcpp::wrap(C_getObject(x));
    return rcpp_result_gen;
END_RCPP
}
// C_setObject
void C_setObject(SEXP x, int i);
RcppExport SEXP _SharedObject_C_setObject(SEXP xSEXP, SEXP iSEXP) {
BEGIN_RCPP
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< SEXP >::type x(xSEXP);
    Rcpp::traits::input_parameter< int >::type i(iSEXP);
    C_setObject(x, i);
    return R_NilValue;
END_RCPP
}
// C_ISS4
bool C_ISS4(SEXP x);
RcppExport SEXP _SharedObject_C_ISS4(SEXP xSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< SEXP >::type x(xSEXP);
    rcpp_result_gen = Rcpp::wrap(C_ISS4(x));
    return rcpp_result_gen;
END_RCPP
}
// C_SETS4
void C_SETS4(SEXP x);
RcppExport SEXP _SharedObject_C_SETS4(SEXP xSEXP) {
BEGIN_RCPP
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< SEXP >::type x(xSEXP);
    C_SETS4(x);
    return R_NilValue;
END_RCPP
}
// C_UNSETS4
void C_UNSETS4(SEXP x);
RcppExport SEXP _SharedObject_C_UNSETS4(SEXP xSEXP) {
BEGIN_RCPP
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< SEXP >::type x(xSEXP);
    C_UNSETS4(x);
    return R_NilValue;
END_RCPP
}
// C_initialPkgData
void C_initialPkgData();
RcppExport SEXP _SharedObject_C_initialPkgData() {
BEGIN_RCPP
    Rcpp::RNGScope rcpp_rngScope_gen;
    C_initialPkgData();
    return R_NilValue;
END_RCPP
}
// C_releasePkgData
void C_releasePkgData();
RcppExport SEXP _SharedObject_C_releasePkgData() {
BEGIN_RCPP
    Rcpp::RNGScope rcpp_rngScope_gen;
    C_releasePkgData();
    return R_NilValue;
END_RCPP
}
// C_getLastIndex
int32_t C_getLastIndex();
RcppExport SEXP _SharedObject_C_getLastIndex() {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    rcpp_result_gen = Rcpp::wrap(C_getLastIndex());
    return rcpp_result_gen;
END_RCPP
}
// C_allocateSharedMemory
uint32_t C_allocateSharedMemory(size_t size_in_byte);
RcppExport SEXP _SharedObject_C_allocateSharedMemory(SEXP size_in_byteSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< size_t >::type size_in_byte(size_in_byteSEXP);
    rcpp_result_gen = Rcpp::wrap(C_allocateSharedMemory(size_in_byte));
    return rcpp_result_gen;
END_RCPP
}
// C_mapSharedMemory
SEXP C_mapSharedMemory(uint32_t id);
RcppExport SEXP _SharedObject_C_mapSharedMemory(SEXP idSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< uint32_t >::type id(idSEXP);
    rcpp_result_gen = Rcpp::wrap(C_mapSharedMemory(id));
    return rcpp_result_gen;
END_RCPP
}
// C_unmapSharedMemory
bool C_unmapSharedMemory(uint32_t id);
RcppExport SEXP _SharedObject_C_unmapSharedMemory(SEXP idSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< uint32_t >::type id(idSEXP);
    rcpp_result_gen = Rcpp::wrap(C_unmapSharedMemory(id));
    return rcpp_result_gen;
END_RCPP
}
// C_freeSharedMemory
bool C_freeSharedMemory(uint32_t id);
RcppExport SEXP _SharedObject_C_freeSharedMemory(SEXP idSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< uint32_t >::type id(idSEXP);
    rcpp_result_gen = Rcpp::wrap(C_freeSharedMemory(id));
    return rcpp_result_gen;
END_RCPP
}
// C_hasSharedMemory
bool C_hasSharedMemory(uint32_t id);
RcppExport SEXP _SharedObject_C_hasSharedMemory(SEXP idSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< uint32_t >::type id(idSEXP);
    rcpp_result_gen = Rcpp::wrap(C_hasSharedMemory(id));
    return rcpp_result_gen;
END_RCPP
}
// C_getSharedMemorySize
double C_getSharedMemorySize(uint32_t id);
RcppExport SEXP _SharedObject_C_getSharedMemorySize(SEXP idSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< uint32_t >::type id(idSEXP);
    rcpp_result_gen = Rcpp::wrap(C_getSharedMemorySize(id));
    return rcpp_result_gen;
END_RCPP
}
// C_allocateNamedSharedMemory
void C_allocateNamedSharedMemory(const string name, size_t size_in_byte);
RcppExport SEXP _SharedObject_C_allocateNamedSharedMemory(SEXP nameSEXP, SEXP size_in_byteSEXP) {
BEGIN_RCPP
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< const string >::type name(nameSEXP);
    Rcpp::traits::input_parameter< size_t >::type size_in_byte(size_in_byteSEXP);
    C_allocateNamedSharedMemory(name, size_in_byte);
    return R_NilValue;
END_RCPP
}
// C_mapNamedSharedMemory
SEXP C_mapNamedSharedMemory(const string name);
RcppExport SEXP _SharedObject_C_mapNamedSharedMemory(SEXP nameSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< const string >::type name(nameSEXP);
    rcpp_result_gen = Rcpp::wrap(C_mapNamedSharedMemory(name));
    return rcpp_result_gen;
END_RCPP
}
// C_unmapNamedSharedMemory
bool C_unmapNamedSharedMemory(const string name);
RcppExport SEXP _SharedObject_C_unmapNamedSharedMemory(SEXP nameSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< const string >::type name(nameSEXP);
    rcpp_result_gen = Rcpp::wrap(C_unmapNamedSharedMemory(name));
    return rcpp_result_gen;
END_RCPP
}
// C_freeNamedSharedMemory
bool C_freeNamedSharedMemory(const string name);
RcppExport SEXP _SharedObject_C_freeNamedSharedMemory(SEXP nameSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< const string >::type name(nameSEXP);
    rcpp_result_gen = Rcpp::wrap(C_freeNamedSharedMemory(name));
    return rcpp_result_gen;
END_RCPP
}
// C_hasNamedSharedMemory
bool C_hasNamedSharedMemory(const string name);
RcppExport SEXP _SharedObject_C_hasNamedSharedMemory(SEXP nameSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< const string >::type name(nameSEXP);
    rcpp_result_gen = Rcpp::wrap(C_hasNamedSharedMemory(name));
    return rcpp_result_gen;
END_RCPP
}
// C_getNamedSharedMemorySize
double C_getNamedSharedMemorySize(const string name);
RcppExport SEXP _SharedObject_C_getNamedSharedMemorySize(SEXP nameSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< const string >::type name(nameSEXP);
    rcpp_result_gen = Rcpp::wrap(C_getNamedSharedMemorySize(name));
    return rcpp_result_gen;
END_RCPP
}

static const R_CallMethodDef CallEntries[] = {
    {"_SharedObject_C_createSharedMemory", (DL_FUNC) &_SharedObject_C_createSharedMemory, 2},
    {"_SharedObject_C_readSharedMemory", (DL_FUNC) &_SharedObject_C_readSharedMemory, 1},
    {"_SharedObject_C_memcpy", (DL_FUNC) &_SharedObject_C_memcpy, 3},
    {"_SharedObject_C_isSameObject", (DL_FUNC) &_SharedObject_C_isSameObject, 2},
    {"_SharedObject_C_setSharedObjectOwership", (DL_FUNC) &_SharedObject_C_setSharedObjectOwership, 2},
    {"_SharedObject_C_setAttributes", (DL_FUNC) &_SharedObject_C_setAttributes, 2},
    {"_SharedObject_C_ALTREP", (DL_FUNC) &_SharedObject_C_ALTREP, 1},
    {"_SharedObject_C_getAltData1", (DL_FUNC) &_SharedObject_C_getAltData1, 1},
    {"_SharedObject_C_getAltData2", (DL_FUNC) &_SharedObject_C_getAltData2, 1},
    {"_SharedObject_C_setAltData1", (DL_FUNC) &_SharedObject_C_setAltData1, 2},
    {"_SharedObject_C_setAltData2", (DL_FUNC) &_SharedObject_C_setAltData2, 2},
    {"_SharedObject_C_getObject", (DL_FUNC) &_SharedObject_C_getObject, 1},
    {"_SharedObject_C_setObject", (DL_FUNC) &_SharedObject_C_setObject, 2},
    {"_SharedObject_C_ISS4", (DL_FUNC) &_SharedObject_C_ISS4, 1},
    {"_SharedObject_C_SETS4", (DL_FUNC) &_SharedObject_C_SETS4, 1},
    {"_SharedObject_C_UNSETS4", (DL_FUNC) &_SharedObject_C_UNSETS4, 1},
    {"_SharedObject_C_initialPkgData", (DL_FUNC) &_SharedObject_C_initialPkgData, 0},
    {"_SharedObject_C_releasePkgData", (DL_FUNC) &_SharedObject_C_releasePkgData, 0},
    {"_SharedObject_C_getLastIndex", (DL_FUNC) &_SharedObject_C_getLastIndex, 0},
    {"_SharedObject_C_allocateSharedMemory", (DL_FUNC) &_SharedObject_C_allocateSharedMemory, 1},
    {"_SharedObject_C_mapSharedMemory", (DL_FUNC) &_SharedObject_C_mapSharedMemory, 1},
    {"_SharedObject_C_unmapSharedMemory", (DL_FUNC) &_SharedObject_C_unmapSharedMemory, 1},
    {"_SharedObject_C_freeSharedMemory", (DL_FUNC) &_SharedObject_C_freeSharedMemory, 1},
    {"_SharedObject_C_hasSharedMemory", (DL_FUNC) &_SharedObject_C_hasSharedMemory, 1},
    {"_SharedObject_C_getSharedMemorySize", (DL_FUNC) &_SharedObject_C_getSharedMemorySize, 1},
    {"_SharedObject_C_allocateNamedSharedMemory", (DL_FUNC) &_SharedObject_C_allocateNamedSharedMemory, 2},
    {"_SharedObject_C_mapNamedSharedMemory", (DL_FUNC) &_SharedObject_C_mapNamedSharedMemory, 1},
    {"_SharedObject_C_unmapNamedSharedMemory", (DL_FUNC) &_SharedObject_C_unmapNamedSharedMemory, 1},
    {"_SharedObject_C_freeNamedSharedMemory", (DL_FUNC) &_SharedObject_C_freeNamedSharedMemory, 1},
    {"_SharedObject_C_hasNamedSharedMemory", (DL_FUNC) &_SharedObject_C_hasNamedSharedMemory, 1},
    {"_SharedObject_C_getNamedSharedMemorySize", (DL_FUNC) &_SharedObject_C_getNamedSharedMemorySize, 1},
    {NULL, NULL, 0}
};

void init_real_class(DllInfo* dll);
void init_integer_class(DllInfo* dll);
void init_logical_class(DllInfo* dll);
void init_raw_class(DllInfo* dll);
void init_complex_class(DllInfo* dll);
RcppExport void R_init_SharedObject(DllInfo *dll) {
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);
    init_real_class(dll);
    init_integer_class(dll);
    init_logical_class(dll);
    init_raw_class(dll);
    init_complex_class(dll);
}
