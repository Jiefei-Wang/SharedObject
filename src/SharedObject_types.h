#include <string>
using std::string;

#include <RcppCommon.h>
struct Bar;
// third party library that declares class Bar
// declaring the specialization
namespace Rcpp {
	template <> const char* as(SEXP x) {
		if (TYPEOF(x) != STRSXP) {
			Rf_error("The variable must be a string.");
		}
		if (Rf_length(x) != 1) {
			Rf_error("The variable must be of length 1.");
		}
		return R_CHAR(Rf_asChar(x));
	}
	
}
// this must appear after the specialization,
// otherwise the specialization will not be seen by Rcpp types