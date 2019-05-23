#' A fundamental tool to share an R object.
#'
#' This function only handles an automic object and does not attach the object's attributes, Please call the function `sharedObject` instead
#' @param x An automic object
#' @param opt options
#' @export
sharedVector<-function(x,opt=list()){
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
#'   The default value is `TRUE`, but can be changed through `sharedParms.copyOnWrite()`. See details for the other issues.
#' @param sharedSub whether the subset of a shared object is still a shared object.
#'   The default value is `TRUE`, but can be changed through `sharedParms.sharedSub()`
#' @return A shared object
#' @details
#' When the function argument `x` is an automic object(e.g vector, matrix), the function will create an
#' ALTREP object to replace it.
#' When `x` is a data frame, each column of `x` will be replaced by an ALTREP object.
#'
#' On the R level, the behaviors of an ALTREP object is exactly the same as an automic object(AKA vector),
#' but the data of an ALTREP object is allocated in the shared memory space. Therefore an ALTREP object
#' can be easily exported to the other R processes without dulplicate the memory, which reduces the memory
#' usage and the overhead of data transmission.
#'
#' The most common scenario for using the shared memory is for the case where the data is large
#' and each R process only need to reads the data. Therefore, having one data in the shared memory will be
#' a better solution than having multiple copies of it for each process. However, if each process need to
#' update the data, a memory dulplication will be triggered when `copyOnWrite` is `TRUE`, so the other processes
#' will not see the changes in the data. When `copyOnWrite` is `FALSE`, all the other processes will see the changes
#' immediately.
#'
#' The no-copy-on-write feature is not fully supported by R. In other words, when `copyOnWrite` is `FALSE`,
#' the behaviors of a shared object might not be what an user expects. Please refer to the example code to
#' see the exceptions.
#'
#' @examples
#' #For vector
#' x=runif(10)
#' so=sharedObject(x)
#' x
#' so
#'
#' #For matrix
#' x=matrix(runif(10),2,5)
#' so=sharedObject(x)
#' x
#' so
#'
#' #For data frame
#' x=as.data.frame(matrix(runif(10),2,5))
#' so=sharedObject(x)
#' x
#' so
#'
#' #export the object
#' library(parallel)
#' cl=makeCluster(1)
#' clusterExport(cl,"so")
#' #check the exported object in the other cluster
#' clusterEvalQ(cl,so)
#'
#' #Copy-on-write
#' x=runif(10)
#' so1=sharedObject(x,copyOnWrite=TRUE)
#' so2=so1
#' so2[1]=10
#' #so1 is unchanged since copy-on-write feature is on.
#' so1
#' so2
#'
#' #No-copy-on-write
#' so1=sharedObject(x,copyOnWrite=FALSE)
#' so2=so1
#' so2[1]=10
#' #so1 is changed
#' so1
#' so2
#'
#' #Flaw of no-copy-on-write
#' #The following code changes the value of so1, highly unexpected! Please use with caution!
#' -so1
#' so1
#' #The reason is that the minus function trys to dulplicate so1 object,
#' #but the dulplicate function will return so1 itself, so the value in so1 also get changed.
#'
#' @export
sharedObject<-function(x,copyOnWrite=sharedParms.copyOnWrite(),sharedSub=sharedParms.sharedSub(),
                       sharedDuplicate=sharedParms.sharedDuplicate()){
 opt=list(copyOnWrite=copyOnWrite,sharedSub=sharedSub,sharedDuplicate=sharedDuplicate)
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




is.sharedObject<-function(x){
  if(is.atomic(x)){
    return(is.sharedVector(x))
  }
  if(is.data.frame(x)){
    res=vapply(x, is.sharedVector,logical(1))
    return(all(res))
  }
  return(FALSE)
}

is.sharedVector<-function(x){
  if(is.atomic(x)){
    sm=peekSharedMemory(x)
    if(!is.null(sm)&&is(sm,"sharedMemory")){
      return(TRUE)
    }
  }
  return(FALSE)
}


getSharedProperty<-function(x,prop){
  if(is.sharedVector(x)){
    sm=peekSharedMemory(x)
    if(prop%in%sharedOption){
      res=switch(prop,
                 copyOnWrite=C_getCopyOnWrite(getVecDID(x)),
                 sharedSub=C_getSharedSub(getVecDID(x)),
                 sharedDuplicate=C_getSharedDuplicate(getVecDID(x))
             )
      return(res)
    }
    if(prop%in%dataInfoName){
      return(sm$dataInfo[prop])
    }else{
      return(sm[[prop]])
    }
  }else{
    error("The object is not of shared memory class")
  }
}

setSharedProperty<-function(x,prop,value){
  if(is.sharedVector(x)){
    sm=peekSharedMemory(x)
    if(prop%in%sharedOption){
      res=switch(prop,
                 copyOnWrite=C_setCopyOnWrite(getVecDID(x),value),
                 sharedSub=C_setSharedSub(getVecDID(x),value),
                 sharedDuplicate=C_setSharedDuplicate(getVecDID(x),value)
      )
      sm$dataInfo[prop]=value
    }
    if(prop%in%dataInfoName){
      sm$dataInfo[prop]=value
    }else{
      sm[[prop]]=value
    }
  }else{
    error("The object is not of shared memory class")
  }
}

getVecDID<-function(x){
  getSharedProperty(x,"DID")
}
getVecPID<-function(x){
  getSharedProperty(x,"PID")
}
getVecType<-function(x){
  getSharedProperty(x,"type")
}
getVecTypeID<-function(x){
  getSharedProperty(x,"type_id")
}
getVecTotalSize<-function(x){
  getSharedProperty(x,"total_size")
}
getVecCopyOnWrite<-function(x){
  as.logical(getSharedProperty(x,"copyOnWrite"))
}
getVecSharedSub<-function(x){
  as.logical(getSharedProperty(x,"sharedSub"))
}
getVecSharedDuplicate<-function(x){
  as.logical(getSharedProperty(x,"sharedDuplicate"))
}
getVecOwnData<-function(x){
  getSharedProperty(x,"ownData")
}


setVecCopyOnwrite<-function(x,value){
  getSharedProperty(x,"copyOnwrite",value)
}
setVecSharedSub<-function(x,value){
  getSharedProperty(x,"sharedSub",value)
}
setVecSharedDuplicate<-function(x,value){
  getSharedProperty(x,"sharedDuplicate",value)
}
setVecOwnData<-function(x,value){
  setSharedProperty(x,"ownData",value)
}

