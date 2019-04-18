#' @useDynLib sharedObject, .registration = TRUE,  .fixes = 'C_'
#' @importFrom stringr str_match
#' @importFrom rlist list.rbind

.onUnload <- function(libpath) {
  library.dynam.unload("sharedObject", libpath)
}



globalSettings=new.env()
globalSettings$supportLargeIndex=FALSE
globalSettings$delimiter=":-:"
