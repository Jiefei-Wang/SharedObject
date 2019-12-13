#' @useDynLib SharedObject, .registration = TRUE
#' @importFrom Rcpp sourceCpp
#' @importFrom stats runif
#' @importFrom methods is new
#' @importFrom xptr xptr_tag
#' @import BiocGenerics

.globals <- local({
    unloaded <- FALSE
    list(
        getProcessID = function()
            as.double(Sys.getpid()),
        isPackageUnloaded = function()
            unloaded,
        setPackageUnloaded = function()
            assign("unloaded", TRUE, envir = environment(), inherits = TRUE)
    )
})



.onUnload <- function(libpath) {
    .globals$setPackageUnloaded()
}
