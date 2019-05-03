#pragma once
#define PGKNAME "sharedObject"

#define SM_ENV(x) Rf_getAttrib(x,Rf_install(".xData"))
#define SM_DATA(sm,data) Rf_findVar(Rf_install(#data), SM_ENV(sm))
#define SM_FUN(sm,fun) Rf_findFun(Rf_install(#fun), SM_ENV(sm))


#define SO_ENV(so) SM_ENV(R_altrep_data1(so))
#define SO_DATA(so,data) SM_DATA(R_altrep_data1(so),data)
#define SO_FUN(so,fun) SM_FUN(R_altrep_data1(so),fun)


#define SO_EPTR(x) SO_DATA(x,address)
#define SO_PTR(x) R_ExternalPtrAddr(SO_EPTR(x))
#define SO_UPDATE_FUN(x) SO_FUN(x,updateAddress)
#define SO_LENGTH(x) Rf_asReal(SO_DATA(x,length))
#define SO_SIZE(x) Rf_asReal(SO_DATA(x,total_size))
#define SO_TYPE(x) Rf_asInteger(SO_DATA(x, type_id))
#define SO_TYPE_CHAR(x) CHAR(Rf_asChar((SO_DATA(x, type))))