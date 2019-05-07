#' @export
sharedVector<-function(x,duplicate=sharedParms.duplicate()){
  sm=sharedMemory(x,duplicate)
  obj=C_createAltrep(sm)
  obj
}


#' @export
sharedVectorById<-function(did){
  sm=createSharedMemoryByID(did)
  obj=C_createAltrep(sm)
  obj
}

