#' @include sharedMemory-class.R



sharedMemory$methods(
  loadMemObj=function(){
    .self$address=.Call(C_readSharedMemory,.self$DID)
    .self$address_sig=compressSettings(NID=.self$NID,PID=.self$PID)
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


showProcessIDs<-function(){
  .Call(C_getProcessList)
}

showDataIDs<-function(process_id){
  .Call(C_getDataList,as.integer(process_id))
}


removeObject<-function(data_ids){
  .Call(C_clearObj,as.integer(data_ids))
  invisible()
}


removeAllObject<-function(){
  .Call(C_clearAll)
  invisible()
}


