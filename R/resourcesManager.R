RM_data<-new.env()
RM_data$NID=Sys.info()["nodename"]
RM_data$PID=as.double(Sys.getpid())
RM_data$unloaded=FALSE


RM<-new.env()
RM$getNID<-function(){
  RM_data$NID
}
RM$getPID<-function(){
  RM_data$PID
}


#' Release all objects in the shared memory
#'
#' This function will force the package to delete all data in the shared memory.
#' Any try to read the data after the function call will crash R.
#'
#' @return no return value
#' @export
removeAllObject<-function(){
  dids=getDataID()
  removeObject(dids)
  invisible()
}
#' Release an objects in the shared memory
#'
#' This function will delete the data associated with the key provided by the function argument.
#' Any try to read the data after the function call will crash R.
#'
#' @param data_ids The data ID you want to delete
#' @return no return value
#' @export
removeObject<-function(data_ids){
  sapply(as.double(data_ids),removeObject_single)
  invisible()
}

removeObject_single<-function(id){
  C_clearObj(id)
  invisible()
}


getDataID<-function(){
  C_getDataID()
}

#' Get a summary report of the data in the shared memory
#'
#' This function will return a list of the summary of the data in the shared memory.
#'
#' @param data_ids The data ID. If NULL(default), all data will be returned.
#'
#' @examples
#' getDataInfo()
#' @export
getDataInfo<-function(data_ids=NULL){
  if(is.null(data_ids)){
    data_ids=getDataID()
  }
  res=sapply(data_ids, getDataInfo_single)
  if(length(res)==0){
    res=data.frame(matrix(vector(), 0, length(dataInfoName),
                          dimnames=list(c(), dataInfoName)),
                   stringsAsFactors=F)
    return(res)
  }
  res=as.data.frame(t(res))

  colnames(res)=dataInfoName
  res$DID=as.character(res$DID)

  res
}

getDataInfo_single<-function(data_ids){
  C_getDataInfo(data_ids)
}


