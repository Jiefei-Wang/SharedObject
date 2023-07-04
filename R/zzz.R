#' @useDynLib SharedObject, .registration = TRUE
#' @importFrom Rcpp sourceCpp
#' @importFrom stats runif
#' @importFrom methods is new as
#' @import BiocGenerics
#' @import methods
NULL

errorHandler<- function(e){
    msg <- conditionMessage(e)
    if(getOS()=="osx"){
        msg <- paste0(msg, 
                      "\n If the error is 'Operation not permitted', you might not have permission to access '/tmp/boost_interprocess'")
    }
    warning(msg)
}


.onLoad <- function(libname, pkgname){
    if(Sys.getenv("DEBUG_SHARED_OBJECT_PACKAGE")==""){
        setVerbose(FALSE)
    }else{
        C_setSharedMemoryPrint(TRUE)
        C_setAltrepPrint(FALSE)
        C_setPackagePrint(TRUE)
    }
    tryCatch({
        initialSharedObjectPackageData()
    }, error = errorHandler)
    initialSharedObjectPackageData()
}
