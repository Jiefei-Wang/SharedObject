#pragma once
#include "C_memoryManagerInterface.h"

//get data from dataReferenceInfo 
#define DRI_DATAID(x) Rf_asReal(VECTOR_ELT(x,0))
#define DRI_EPTR(x) VECTOR_ELT(x,1)
#define DRI_PTR(x) R_ExternalPtrAddr(DRI_EPTR(x))
#define DRI_TYPE_NAME(x) VECTOR_ELT(x,2)

#define DRI_PROCESSID(x) C_getProcessID(DRI_DATAID(x))
#define DRI_TYPEID(x) C_getTypeID(DRI_DATAID(x))
#define DRI_LENGTH(x) C_getLength(DRI_DATAID(x))
#define DRI_TOTAL_SIZE(x) C_getTotalSize(DRI_DATAID(x))
#define DRI_COPY_ON_WRITE(x) C_getCopyOnWrite(DRI_DATAID(x))
#define DRI_SHARED_SUBSET(x)  C_getSharedSubset(DRI_DATAID(x))
#define DRI_SHARED_DUPLICATE(x)  C_getSharedCopy(DRI_DATAID(x))


//Access to shared vector's Info
#define SV_DATAID(x) DRI_DATAID(R_altrep_data1(x))
#define SV_EPTR(x) DRI_EPTR(R_altrep_data1(x)
#define SV_PTR(x) DRI_PTR(R_altrep_data1(x))
#define SV_TYPE_NAME(x) DRI_TYPE_NAME(R_altrep_data1(x))


#define SV_PROCESSID(x) DRI_PROCESSID(R_altrep_data1(x))
#define SV_TYPEID(x) DRI_TYPEID(R_altrep_data1(x))
#define SV_LENGTH(x) DRI_LENGTH(R_altrep_data1(x))
#define SV_TOTAL_SIZE(x) DRI_TOTAL_SIZE(R_altrep_data1(x))
#define SV_COPY_ON_WRITE(x) DRI_COPY_ON_WRITE(R_altrep_data1(x))
#define SV_SHARED_SUBSET(x)  DRI_SHARED_SUBSET(R_altrep_data1(x))
#define SV_SHARED_DUPLICATE(x)  DRI_SHARED_DUPLICATE(R_altrep_data1(x))
