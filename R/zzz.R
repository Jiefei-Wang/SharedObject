#' @useDynLib SharedObject, .registration = TRUE
#' @importFrom Rcpp sourceCpp
#' @importFrom stats runif
#' @importFrom methods is new as
#' @import BiocGenerics
#' @import methods
NULL

.onLoad <- function(libname, pkgname){
    initialSharedObjectPackageData()
}

