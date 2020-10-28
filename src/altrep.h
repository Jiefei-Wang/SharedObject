#include <Rcpp.h>
#include <R_ext/Altrep.h>

//Get the ALTREP class object by type id
R_altrep_class_t& getAltClass(int type);
SEXP getDataInfoTemplate();
SEXP getStringDataInfoTemplate();
SEXP createSharedObjectFromSource(SEXP x,
                                    bool copyOnWrite = true, bool sharedSubset = false, bool sharedCopy = false,
                                    SEXP attributes = R_NilValue);
SEXP createEmptySharedObject(int type, uint64_t length,
                               bool copyOnWrite = true, bool sharedSubset = false, bool sharedCopy = false,
                               SEXP attributes = R_NilValue);
SEXP createSharedStringFromSource(SEXP x, bool copyOnWrite,	SEXP attributes = R_NilValue);
SEXP readSharedObject(SEXP dataInfo);
SEXP unshare(SEXP x, SEXP attributes = R_NilValue);
SEXP unshareString(SEXP x, SEXP attributes = R_NilValue);

