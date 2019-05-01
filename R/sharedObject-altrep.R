#' @export
sharedObject<-function(x){
  sm=sharedMemory(x)
  obj=C_createAltrep(sm)
  obj
}


#' @export
sharedObjectById<-function(nid,did){
  sm=createSharedMemoryByID(nid,did)
  obj=C_createAltrep(sm)
  obj
}

