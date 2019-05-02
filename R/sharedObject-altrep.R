#' @export
sharedObject<-function(x){
  sm=sharedMemory(x)
  obj=C_createAltrep(sm)
  obj
}


#' @export
sharedObjectById<-function(did){
  sm=createSharedMemoryByID(did)
  obj=C_createAltrep(sm)
  obj
}

