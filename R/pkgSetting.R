## usethis namespace: start
#' @useDynLib sharedObject, .registration = TRUE
#' @importFrom Rcpp sourceCpp
## usethis namespace: end
NULL
#' @importFrom stringr str_match
#' @importFrom rlist list.rbind

.onUnload <- function(libpath) {
  library.dynam.unload("sharedObject", libpath)
}



globalSettings=new.env()
globalSettings$supportLargeIndex=FALSE
