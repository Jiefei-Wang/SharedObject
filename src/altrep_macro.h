#pragma once
#define PGKNAME "sharedObject"

#define SM_ENV(x) Rf_getAttrib(x,Rf_install(".xData"))
#define SM_DATA(sm,data) Rf_findVar(Rf_install(#data), SM_ENV(sm))
#define SM_FUN(sm,fun) Rf_findFun(Rf_install(#fun), SM_ENV(sm))


#define SV_ENV(so) SM_ENV(R_altrep_data1(so))
#define SV_DATA(so,data) SM_DATA(R_altrep_data1(so),data)
#define SV_FUN(so,fun) SM_FUN(R_altrep_data1(so),fun)


#define SV_EPTR(x) SV_DATA(x,address)
#define SV_PTR(x) R_ExternalPtrAddr(SV_EPTR(x))
#define SV_UPDATE_FUN(x) SV_FUN(x,updateAddress)
#define SV_LENGTH(x) Rf_asReal(SV_DATA(x,length))
#define SV_SIZE(x) Rf_asReal(SV_DATA(x,total_size))
#define SV_TYPE(x) Rf_asInteger(SV_DATA(x, type_id))
#define SV_TYPE_CHAR(x) CHAR(Rf_asChar((SV_DATA(x, type))))
#define SV_COW(x) Rf_asLogical(SV_DATA(x, copyOnWrite))
#define SV_SHAREDSUB(x) Rf_asLogical(SV_DATA(x, sharedSub))