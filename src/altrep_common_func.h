#include <R.h>
#include <Rinternals.h>
#include <R_ext/Altrep.h>
#include "tools.h"
#define PGKNAME "sharedObject"

#define SM_ENV(x) getAttrib(x,install(".xData"))
#define SM_DATA(sm,data) findVar(install(#data), SM_ENV(sm))
#define SM_FUN(sm,fun) findFun(install(#fun), SM_ENV(sm))

#define SO_DATA(so,data) SM_DATA(R_altrep_data1(so),data)
#define SO_FUN(so,fun) SM_FUN(R_altrep_data1(so),fun)


#define SO_EPTR(x) SO_DATA(x,address)
#define SO_PTR(x) R_ExternalPtrAddr(SO_EPTR(x))
#define SO_LENGTH(x) asReal(SO_DATA(x,length))
#define SO_SIZE(x) asReal(SO_DATA(x,total_size))
#define SO_TYPE(x) asInteger(SO_DATA(x, type_id))
#define SO_TYPE_CHAR(x) CHAR(asChar((SO_DATA(x, type))))


Rboolean sharedObject_Inspect(SEXP x, int pre, int deep, int pvec,
                      void (*inspect_subtree)(SEXP, int, int, int));


R_xlen_t sharedObject_length(SEXP x);

void *sharedObject_dataptr(SEXP x, Rboolean writeable);
const void *sharedObject_dataptr_or_null(SEXP x);
SEXP sharedObject_dulplicate(SEXP x, Rboolean deep);



#include <type_traits>
template<class T1, class T2>
void template_subset_assignment(T1* target, T1* source, T2* indx, R_xlen_t src_len, R_xlen_t ind_len) {
	for (R_xlen_t i = 0; i < ind_len; i++) {
		if (indx[i] < src_len) {
			if (std::is_same<T2, double>::value) {
				target[i] = source[(R_xlen_t)indx[i]];
			}
			else {
				target[i] = source[(int)indx[i]];
			}
		}
		else {
			errorHandle("Index out of bound:\n index: %llu length:%llu\n", (ULLong)indx[i], (ULLong)src_len);
		}
	}
}


template<class T>
SEXP template_coerce(T* x, R_xlen_t len, int type)
{
	SEXP result;

	switch (type)
	{
	case INTSXP:
		result = PROTECT(allocVector(INTSXP, len));
		for (R_xlen_t i = 0; i < len; i++) {
			INTEGER(result)[i] = x[i];
		}
		break;
	case REALSXP:
		result = PROTECT(allocVector(REALSXP, len));
		for (R_xlen_t i = 0; i < len; i++) {
			REAL(result)[i] = x[i];
		}
		break;
	default:
		errorHandle("Unknown type: %d\n", type);
	}
	UNPROTECT(1);
	return(result);
}