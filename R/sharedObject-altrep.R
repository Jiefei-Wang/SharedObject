sharedObject<-function(x,id=NULL){
  if(!is.null(id)){
    obj=recoverById(id)
    return(obj)
  }
  SM_obj=sharedMemory(x)
  SM_obj$needPrint=FALSE
  obj=.Call(C_createAltrep,SM_obj)
  obj
}

recoverById<-function(id){
  SM_obj=sharedMemory()
  SM_obj$needPrint=FALSE
  SM_obj$initializeWithID(id)
  obj=.Call(C_createAltrep,SM_obj$address,SM_obj$type_id,SM_obj$length,SM_obj$total_size)
  attr(obj,"sharedMemory")=obj
  obj
}

