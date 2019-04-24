#include <R.h>
#include <Rinternals.h>
#include <R_ext/Altrep.h>
#include "tools.h"
#define PGKNAME "sharedObject"


#define SO_PTR(x) R_ExternalPtrAddr(SO_EPTR(x))
#define SO_EPTR(x) R_altrep_data1(x)
#define SO_STATE(x) R_altrep_data2(x)

#define SO_STATE_LENGTH(x) ((R_xlen_t) REAL_ELT(VECTOR_ELT(x,1), 0))
#define SO_STATE_SIZE(x) ((R_xlen_t) REAL_ELT(VECTOR_ELT(x,1), 1))

#define SO_LENGTH(x) SO_STATE_LENGTH(SO_STATE(x))
#define SO_SIZE(x) SO_STATE_SIZE(SO_STATE(x))

Rboolean sharedObject_Inspect(SEXP x, int pre, int deep, int pvec,
                      void (*inspect_subtree)(SEXP, int, int, int));

SEXP make_sharedObject_state( SEXP R_type,SEXP R_length,SEXP R_size);

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