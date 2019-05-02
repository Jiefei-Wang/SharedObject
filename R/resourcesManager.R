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



removeAllObject<-function(){
  dids=getDataID()
  removeObject(dids)
  invisible()
}

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


getDataInfo<-function(did=NULL){
  if(is.null(did)){
    did=getDataID()
  }
  res=vapply(did, getDataInfo_single,numeric(5))
  res=as.data.frame(t(res))
  res$DID=as.character(res$DID)
  colnames(res)=c("DID","PID","type","length","size")
  res
}

getDataInfo_single<-function(did){
  C_getDataInfo(did)
}


