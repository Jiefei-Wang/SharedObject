#' A fundamental tool to share an R object.
#'
#' This function only handles an automic object and does not attach the object's attributes, Please call the function `sharedObject` instead
#' @param x An automic object
#' @param opt options
#' @export
sharedVector<-function(x,opt){
  sm=sharedMemory(x,opt)
  obj=C_createAltrep(sm)
  obj
}

#' A fundamental tool to share an R object.
#'
#' retrieve an R object by the data ID, this function is for internal usage only
#' @param did The data id
#' @export
sharedVectorById<-function(did){
  sm=createSharedMemoryByID(did)
  obj=C_createAltrep(sm)
  obj
}
#' Create an R object in the shared memory
#'
#' This function will create an object in the shared memory for the function argument `x`,
#' It can be exported to the other R processes. If exported, the object in all processes will use
#' the same data in the same memory location. Therefore, there is no cost to share the return value of this function.
#' @param x An R object that you want to shared. The data type can be `vector`,
#' `matrix` and `data.frame`. List is not supported.
#' @param copyOnWrite Whether a new R object need to be allocated when the shared object is changed.
#'   The default value is `TRUE`, but can be changed through `sharedParms.copyOnWrite()`
#' @param sharedSub whether the subset of a shared object is still a shared object.
#'   The default value is `TRUE`, but can be changed through `sharedParms.sharedSub()`
#' @return A shared object
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


sharedAtomic<-function(x,opt){
  obj=sharedVector(x,opt)
  obj=copyAttribute(x,obj)
  obj
}




sharedDataFrame<-function(x,opt){
  obj=vector("list",length=length(x))
  for(i in seq_along(obj)){
      sm=sharedObject_hidden(x[[i]],options=opt)
      obj[[i]]=sm
  }
  obj=copyAttribute(x,obj)
  obj
}





