## usethis namespace: start
#' @useDynLib sharedObject, .registration = TRUE
#' @importFrom Rcpp sourceCpp
## usethis namespace: end
NULL
#' @importFrom extraDistr rdunif

.onUnload <- function(libpath) {
  library.dynam.unload("sharedObject", libpath)
  RM_data$unloaded=TRUE
}



globalSettings=new.env()
