typeName=c("logical","integer","double","raw","character")
availableType=data.frame(
  row.names=typeName,
  size=as.integer(c(4,4,8,1,1)),
  id=as.integer(seq_along(typeName)),
  stringsAsFactors = FALSE)



#the size of the type
type_size<-function(x){
  if(!x%in%typeName)
    stop("The type has not been defined: ", x)
  availableType[x,"size"]
}

get_type_id<-function(x){
  if(!x%in%typeName)
    stop("The type has not been defined: ", x)
  availableType[x,"id"]
}

get_type_name<-function(id){
  ind=which(availableType[,"id"]%in%id)
  if(length(ind)==0)
    stop("The id does not correspond to any type: ", id)
  typeName[ind]
}

#' @export
peekSharedMemory<-function(x){
  C_peekSharedMemory(x)
}


copyAttribute<-function(source,target){
  att=attributes(source)
  for(i in names(att)){
    C_attachAttr(target,i,att[[i]])
  }
  target
}

generateKey<-function(){
  key=round(runif(1,0,2^53))
  key
}

#' @export
serializeSO<-function(x){
res=attributes(x)
did=peekSharedMemory(x)$DID
state=list(DID=did,attr=res)
#message(state)
return(state)
}

#' @export
unserializeSO<-function(x){
 # message(x)
  did=x[["DID"]]
  sv=sharedVectorById(did)
  attr_name=names(x[["attr"]])
  for(i in seq_along(x[["attr"]])){
    C_attachAttr(sv,attr_name[i],x[["attr"]][[i]])
  }
  return(sv)
}









