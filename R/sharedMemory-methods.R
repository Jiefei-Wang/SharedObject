#' @include sharedMemory-class.R



sharedMemory$methods(
  updateAddress=function(){
    .self$address=C_readSharedMemory(.self$dataInfo["DID"])
  }
)
sharedMemory$methods(
  possess=function(){
    .self$ownData=TRUE
  },
  surrender=function(){
    .self$ownData=FALSE
  }
)

createSharedMemoryByID<-function(did){
 sm=sharedMemory()
 sm$initializeWithID(did)
 sm
}
is.sharedMemory<-function(x){
  if(is.atomic(x)){
    sm=peekSharedMemory(x)
    if(!is.null(sm)&&is(sm,"sharedMemory")){
      return(TRUE)
    }
  }
  return(FALSE)
}


getSharedProperty<-function(x,prop){
  if(is.sharedMemory(x)){
    sm=peekSharedMemory(x)
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
  if(is.sharedMemory(x)){
    sm=peekSharedMemory(x)
    if(prop%in%dataInfoName){
      sm$dataInfo[prop]=value
    }else{
      sm[[prop]]=value
    }
  }else{
    error("The object is not of shared memory class")
  }
}

getDID<-function(x){
  getSharedProperty(x,"DID")
}
getPID<-function(x){
  getSharedProperty(x,"PID")
}
getType<-function(x){
  getSharedProperty(x,"type")
}
getTypeID<-function(x){
  getSharedProperty(x,"type_id")
}
getTotalSize<-function(x){
  getSharedProperty(x,"total_size")
}
getCopyOnWrite<-function(x){
  as.logical(getSharedProperty(x,"copyOnWrite"))
}
getSharedSub<-function(x){
  as.logical(getSharedProperty(x,"sharedSub"))
}
getOwnData<-function(x){
  getSharedProperty(x,"ownData")
}

setDID<-function(x,value){
  setSharedProperty(x,"DID",value)
  x
}
setPID<-function(x,value){
  setSharedProperty(x,"PID",value)
}
setType<-function(x,value){
  setSharedProperty(x,"type",value)
}
setTypeID<-function(x,value){
  setSharedProperty(x,"type_id",value)
}
setTotalSize<-function(x,value){
  setSharedProperty(x,"total_size",value)
}
setCopyOnWrite<-function(x,value){
  setSharedProperty(x,"copyOnWrite",value)
}
setSharedSub<-function(x,value){
  setSharedProperty(x,"sharedSub",value)
}
setOwnData<-function(x,value){
  setSharedProperty(x,"ownData",value)
}
