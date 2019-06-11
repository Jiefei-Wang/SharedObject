typeName=c("logical","integer","double","raw","character")
availableType=data.frame(
  row.names=typeName,
  size=as.integer(c(4,4,8,1,1)),
  id=as.integer(seq_along(typeName)),
  stringsAsFactors = FALSE)



#the size of the type
typeSize<-function(x){
  if(!x%in%typeName)
    stop("The type has not been defined: ", x)
  availableType[x,"size"]
}
typeSizeByID<-function(x){
  typeSize(getTypeNameByID(x))
}

getTypeIDByName<-function(x){
  if(!x%in%typeName)
    stop("The type has not been defined: ", x)
  availableType[x,"id"]
}

getTypeNameByID<-function(id){
  ind=which(availableType[,"id"]%in%id)
  if(length(ind)==0)
    stop("The id does not correspond to any type: ", id)
  typeName[ind]
}
#' get a summary report for a shared object
#'
#' @param x A shared object
#' @param as.list Logical, Whether the result should be in a list format even
#' if only one shared property is reported
#' @export
getSharedProperty<-function(x,as.list=FALSE){
  if(is.data.frame(x)){
      res=lapply(seq_len(ncol(x)),function(i)C_getSharedProperty(x[,i]))
  }else{
    res=C_getSharedProperty(x)
    if(as.list){
      res=list(res)
    }
  }
  res
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
  key=C_findAvailableKey(key)
  key
}

#' Is an Object a desired type?
#'
#' @param x an R object
#' @rdname typeCheck
#' @export
is.altrep<-function(x){
  C_ALTREP(x)
}
is.sharedObject<-function(x){
  if(is.atomic(x)){
    return(is.sharedVector(x))
  }
  if(is.data.frame(x)){
    res=vapply(x, is.sharedVector,logical(1))
    return(all(res))
  }
  return(FALSE)
}

is.sharedVector<-function(x){
  if(is.atomic(x)){
    sm=getSharedProperty(x)
    if(!is.null(sm)&&is(sm,"sharedMemory")){
      return(TRUE)
    }
  }
  return(FALSE)
}
#' @rdname typeCheck
#' @param recursive Logical, whether a `data.frame` can be treated as a shared object. If `TRUE`, a `data.frame`
#' is called a shared object if and only if all of its columns are shared objects.
#' @export
is.shared<-function(x, recursive = TRUE){
  ifelse(recursive,is.sharedObject(x),is.sharedVector(x))
}


#' @details
#' .createInheritedParms: Get the parameters that can be inherited by the child of a shared object
#' @rdname internal
#' @export
.createInheritedParms<-function(x){
  sm=getSharedProperty(x)
  parms=list(
    copyOnWrite=sm$getCopyOnWrite(),
    sharedSubset=sm$getSharedSubset(),
    sharedCopy=sm$getSharedCopy()
  )
  parms
}



calculateSharedMemerySize<-function(x){
  n=length(x)
  if(typeof(x)=="character"){
    char_size=sum(sapply(x,length))+n
    return(n*8+char_size)
  }else{
    return(n*typeSize(typeof(x)))
  }
}

