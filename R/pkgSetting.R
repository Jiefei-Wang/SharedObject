#' @useDynLib sharedObject, .registration = TRUE,  .fixes = 'C_'
#' @importFrom xptr set_xptr_tag
#' @importFrom stringr str_match

.onUnload <- function(libpath) {
  library.dynam.unload("sharedObject", libpath)
}



globalSettings=new.env()
globalSettings$supportLargeIndex=FALSE
globalSettings$delimiter=":-:"
