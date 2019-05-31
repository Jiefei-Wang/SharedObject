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

#' @export
getSharedProperty<-function(x){
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
#' @rdname typeCheck
#' @export
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

#' @rdname typeCheck
#' @export
is.sharedVector<-function(x){
  if(is.atomic(x)){
    sm=getSharedProperty(x)
    if(!is.null(sm)&&is(sm,"sharedMemory")){
      return(TRUE)
    }
  }
  return(FALSE)
}

#' Set the copy-on-write feature
#'
#' Set the copy-on-write feature for a shared object
#'
#' @param x A shared object
#' @return 'setCopyOnwrite': No return value
#' @rdname copyOnWrite
#' @export
setCopyOnwrite<-function(x){
  copyOnWriteHidden(x,TRUE)
  invisible()
}
#' @return 'unsetCopyOnwrite': No return value
#' @rdname copyOnWrite
#' @export
unsetCopyOnwrite<-function(x){
  copyOnWriteHidden(x,FALSE)
  invisible()
}

copyOnWriteHidden<-function(x,opt){
  if(is.sharedVector(x)){
    sm=getSharedProperty(x)
    sm$setCopyOnWrite(opt)
  #return(x)
  }
  if(is.data.frame(x)){
    for(i in seq_len(ncol(x))){
      if(is.altrep(x)){
        sm=getSharedProperty(x[,i])
        sm$setCopyOnWrite(opt)
      }
    }
    #return(x)
  }
}

#Get the parameters that will be inherit by the child of a shared object
#' @export
createInheritedParms<-function(x){
  sm=getSharedProperty(x)
  parms=list(
    copyOnWrite=sm$getCopyOnWrite(),
    sharedSubset=sm$getSharedSubset(),
    sharedDuplicate=sm$getSharedDuplicate()
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

