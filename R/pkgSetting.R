## usethis namespace: start
#' @useDynLib sharedObject, .registration = TRUE
#' @importFrom Rcpp sourceCpp
## usethis namespace: end
NULL
.onUnload <- function(libpath) {
  library.dynam.unload("sharedObject", libpath)
  RM_data$unloaded=TRUE
}



globalSettings=new.env()
globalSettings$copyOnWrite=TRUE
globalSettings$sharedSub=TRUE


setGlobal<-function(name,x){
  if(is.null(x)){
    return(globalSettings[[name]])
  }
  tmp=globalSettings[[name]]
  globalSettings[[name]]=x
  tmp
}


#' @export
sharedParms.copyOnWrite<-function(x=NULL){
  setGlobal("copyOnWrite",x)
}


sharedParms.sharedSub<-function(x=NULL){
  setGlobal("sharedSub",x)
}
