#' @useDynLib SharedObject, .registration = TRUE
#' @importFrom Rcpp sourceCpp
#' @importFrom stats runif
#' @importFrom methods is new as
#' @import BiocGenerics
#' @import methods
NULL


.onLoad <- function(libname, pkgname){
    if(Sys.getenv("DEBUG_SHARED_OBJECT_PACKAGE")==""){
        setVerbose(FALSE)
    }else{
        C_setSharedMemoryPrint(TRUE)
        C_setAltrepPrint(FALSE)
        C_setPackagePrint(TRUE)
    }
    initialSharedObjectPackageData()
}
