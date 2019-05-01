RM<-new.env()
RM_data<-new.env()

RM_data$NID=Sys.info()["nodename"]
RM_data$PID=as.double(Sys.getpid())

RM$getNID<-function(){
  RM_data$NID
}
RM$getPID<-function(){
  RM_data$PID
}

RM$setNID<-function(id){
  RM_data$NID=id
}
RM$setPID<-function(id){
  RM_data$PID=as.double(id)
}

RM$memoryUsage<-function(){
  #processList=.Call(C_getProcessList)
  processInfo=matrix(0,nrow=length(processList),ncol=3)
for(i in seq_along(processList)){
curPID=processList[i]

}



}
