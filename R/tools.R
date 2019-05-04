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

