#' @useDynLib SharedObject, .registration = TRUE
#' @importFrom Rcpp sourceCpp


.globals <- local({
  unloaded <- FALSE
  list(
    getProcessID = function() as.double(Sys.getpid()),
    isPackageUnloaded=function() unloaded,
    setPackageUnloaded=function() unloaded<<-TRUE
  )
})



.onUnload <- function(libpath) {
  library.dynam.unload("SharedObject", libpath)
  .globals$setPackageUnloaded()
}
