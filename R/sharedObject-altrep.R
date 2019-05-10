#' @export
sharedVector<-function(x,opt){
  sm=sharedMemory(x,opt)
  obj=C_createAltrep(sm)
  obj
}


#' @export
sharedVectorById<-function(did){
  sm=createSharedMemoryByID(did)
  obj=C_createAltrep(sm)
  obj
}

#' @export
sharedObject<-function(x,copyOnWrite=sharedParms.copyOnWrite(),sharedSub=sharedParms.sharedSub(),options=NULL){
  if(is.null(options)){
    opt=list(copyOnWrite=copyOnWrite,sharedSub=sharedSub)
  }else{
    opt=options
  }
  if(is.atomic(x)){
    obj=sharedAtomic(x,opt)
    return(obj)
  }
  if(is.data.frame(x)){
    obj=sharedDataFrame(x,opt)
    return(obj)
  }
  error("Unsupported data structure")
}

#' @export
sharedAtomic<-function(x,opt){
  obj=sharedVector(x,opt)
  obj=copyAttribute(x,obj)
  obj
}



#' @export
sharedDataFrame<-function(x,opt){
  obj=vector("list",length=length(x))
  for(i in seq_along(obj)){
      sm=sharedObject(x[[i]],options=opt)
      obj[[i]]=sm
  }
  obj=copyAttribute(x,obj)
  obj
}





