#' @include sharedMemory-class.R



sharedMemory$methods(
  loadMemObj=function(){
    .self$address=.Call(C_readSharedMemory,.self$DID)
    .self$address_sig=compressSettings(NID=RM$getNID(),PID=RM$getPID())
  }
)

sharedMemory$methods(
  subset_oneInd=function(i){
    if(is.null(.self$address)) .self$loadMemObj()
    if(globalSettings$supportLargeIndex){
      stop("Large index is not supported")
    }else{
      res=.Call(C_getValue_32,.self$address,.self$type_id,as.integer(i-1))
    }
    return(res)
  }
)


removeObject<-function(data_ids){
  sapply(as.double(data_ids),removeObject_single)
  invisible()
}


removeObject_single<-function(id){
  .Call("clearObj",id)
  invisible()
}




removeAllObject<-function(verbose){
  .Call(C_clearAll,as.logical(verbose))
  invisible()
}


getDataCount<-function(){
  .Call(C_getDataCount)
}

getFreedKeyList<-function(){
  .Call(C_getFreedKeys)
}

getProcessInfo<-function(){
 res=.Call(C_getProcessInfo)
 res=as.data.frame(res)
 colnames(res)=c("processID","objectNum","totalSize")
 res
}

getDataInfo<-function(pid=NULL){
if(is.null(pid)){
  pid=getProcessInfo()$processID
}
  if(length(pid)==0)
    return(NULL)
  pid=as.double(pid)
  res=lapply(pid,getDataInfo_single)
  if(length(res)==0) return(NULL)
  res=list.rbind(res)
  colnames(res)=c("processID","dataID","size","type")
  res
}
getDataInfo_single<-function(pid){
  res=.Call(C_getDataInfo,pid)
  if(length(res[[1]])==0) return(NULL)
  res=as.data.frame(res)
  res=cbind(pid,res)
  res
}

