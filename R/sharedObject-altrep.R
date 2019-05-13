#' A fundamental tool to share an R object.
#' @param x An automic object
#' @param opt options
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
sharedObject<-function(x,copyOnWrite=sharedParms.copyOnWrite(),sharedSub=sharedParms.sharedSub()){
 opt=list(copyOnWrite=copyOnWrite,sharedSub=sharedSub)
 sharedObject_hidden(x,opt)
}

sharedObject_hidden<-function(x,options){
  if(is.atomic(x)){
    obj=sharedAtomic(x,options)
    return(obj)
  }
  if(is.data.frame(x)){
    obj=sharedDataFrame(x,options)
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
      sm=sharedObject_hidden(x[[i]],options=opt)
      obj[[i]]=sm
  }
  obj=copyAttribute(x,obj)
  obj
}





