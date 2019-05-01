#' @include sharedMemory-class.R



sharedMemory$methods(
  loadMemObj=function(){
    .self$address=C_readSharedMemory(.self$DID)
  }
)

sharedMemory$methods(
  updateAddress=function(){
      message("checking the address")
      if(!.self$AddressValid()){
        message("Updating the address")
        .self$loadMemObj()
        .self$address_NID=RM$getNID()
        .self$address_PID=RM$getPID()
      }
  }
)
sharedMemory$methods(
  AddressValid=function(){
    if(.self$address_NID==RM$getNID()&&.self$address_PID==RM$getPID()){
      return(TRUE)
    }else{
      return(FALSE)
    }
  }
)

createSharedMemoryByID<-function(nid,did){
 sm=sharedMemory()
 sm$initializeWithID(nid,did)
 sm
}


removeObject<-function(data_ids){
  sapply(as.double(data_ids),removeObject_single)
  invisible()
}


removeObject_single<-function(id){
  C_clearObj(id)
  invisible()
}




removeAllObject<-function(verbose){
  C_clearAll(verbose)
  invisible()
}


getDataCount<-function(){
  C_getDataCount()
}

getFreedKeyList<-function(){
  C_getFreedKeys()
}

getProcessInfo<-function(){
 res=C_getProcessInfo()
 res=as.data.frame(res)
 colnames(res)=c("processID","objectNum","totalSize")
 res
}

getDataInfo<-function(pid=NULL){
  varNames=c("processID","dataID","size","type")
  empTemplate=as.data.frame(matrix(nrow=0,ncol=length(varNames),
                                   dimnames=list(NULL,col_names=varNames)))
if(is.null(pid)){
  pid=getProcessInfo()$processID
}
  pid=as.double(pid)
  res=lapply(pid,getDataInfo_single)
  if(length(res)==0) return(empTemplate)
  res=list.rbind(res)
  res
}
getDataInfo_single<-function(pid){
  res=C_getDataInfo(pid)
  if(length(res[[1]])==0) return(NULL)
  res=as.data.frame(res)
  res=cbind(pid,res)
  colnames(res)=c("processID","dataID","size","type")
  res
}

