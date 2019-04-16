typeName=c("logical","integer","double")
availableType=data.frame(
  row.names=typeName,
  size=as.integer(c(1,4,8)),
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



#convert a list to a string
compressSettings<-function(...){
  delimiter=globalSettings$delimiter
  parms=list(...)
  argsName=names(parms)
  argsValue=unlist(parms)
  res=c(argsName,argsValue)
  res=paste0(res,collapse = delimiter)
  res
}
#convert a string to a list
decompressSettings<-function(sig){
  delimiter=globalSettings$delimiter
  res=gregexpr(delimiter,sig,fixed = TRUE)
  nParm=(length(res[[1]])+1)/2
  matchPattern=paste0(c(rep("(.+?)",nParm*2-1),"(.+)"),collapse = delimiter)
  matchRes=str_match(sig,matchPattern)[-1]
  res=list()
  for(i in seq_len(nParm)){
    res[matchRes[i]]=matchRes[i+nParm]
  }
  return(res)
}
