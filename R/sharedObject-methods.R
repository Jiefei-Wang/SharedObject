#' @include sharedObject-class.R



sharedObject$methods(
  loadMemObj=function(){
    .self$address=.Call(C_readSharedMemory,.self$DID)
    .self$address_sig=compressSettings(NID=.self$NID,PID=.self$PID)
  }
)

sharedObject$methods(
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




