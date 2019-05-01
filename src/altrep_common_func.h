#include <Rinternals.h>
#include "tools.h"
#include "sharedObject_types.h"
#include "altrep_macro.h"


Rboolean sharedObject_Inspect(SEXP x, int pre, int deep, int pvec,
                      void (*inspect_subtree)(SEXP, int, int, int));


R_xlen_t sharedObject_length(SEXP x);

void *sharedObject_dataptr(SEXP x, Rboolean writeable);
const void *sharedObject_dataptr_or_null(SEXP x);
SEXP sharedObject_dulplicate(SEXP x, Rboolean deep);
//void sharedObject_updateAd(SEXP x);
SEXP sharedObject_serialized_state(SEXP x);
SEXP sharedObject_unserialize(SEXP R_class, SEXP state);


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
		result = Rf_protect(Rf_allocVector(INTSXP, len));
		for (R_xlen_t i = 0; i < len; i++) {
			INTEGER(result)[i] = x[i];
		}
		break;
	case REALSXP:
		result = Rf_protect(Rf_allocVector(REALSXP, len));
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