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
type_size_id<-function(x){
  type_size(get_type_name(x))
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
  key=C_findAvailableKey(key)
  key
}
#' Serialize a shared Object
#'
#' Internal usage only
#'
#' @param x A shared object
#' @export
serializeSO<-function(x){
res=attributes(x)
did=peekSharedMemory(x)$dataInfo["DID"]
state=did
#message(state)
return(state)
}
#' Unserialize a shared Object
#'
#' Internal usage only
#'
#' @param x A shared object
#' @export
unserializeSO<-function(did){
 # message(x)
  sv=sharedVectorById(did)
  return(sv)
}

dataInfoName=c("DID","PID","type_id","length","total_size","copyOnWrite","sharedSub")
makeSubsetProperty<-function(x,sub_len){
  prop=peekSharedMemory(x)
  dataInfo=prop$dataInfo
  dataInfo["length"]=sub_len
  dataInfo["total_size"]=sub_len*type_size_id(dataInfo["type_id"])
  return(dataInfo)
}




#' Is an Object ALTREP?
#'
#' @param x an R object
#' @export
is.altrep<-function(x){
  C_ALTREP(x)
}

#' Get or set the copy-on-write feature
#'
#' Get or set the copy-on-write feature for a shared object
#'
#' @param x A shared object
#' @return 'copyOnwriteProp': A copy-on-write property
#' @rdname copyOnWrite
#' @export
copyOnwriteProp<-function(x){
  if(is.altrep(x)){
    sm=peekSharedMemory(x)
    return(sm$dataInfo['copyOnWrite'])
  }

  if(is.data.frame(x)){
    res=logical(ncol(x))
    for(i in seq_len(ncol(x))){
      if(is.altrep(x)){
        sm=peekSharedMemory(x[,i])
        res[i]=sm$dataInfo['copyOnWrite']
      }else{
        res[i]=TRUE
      }
    }
    return(all(res))
  }

  message("The object is not a shared object")
}
#' @return 'setCopyOnwrite': No return value
#' @rdname copyOnWrite
#' @export
setCopyOnwrite<-function(x){
  copyOnWrite_hidden(x,TRUE)
  invisible()
}
#' @return 'unsetCopyOnwrite': No return value
#' @rdname copyOnWrite
#' @export
unsetCopyOnwrite<-function(x){
  copyOnWrite_hidden(x,FALSE)
  invisible()
}

copyOnWrite_hidden<-function(x,opt){
  if(is.altrep(x)){
  sm=peekSharedMemory(x)
  sm$dataInfo['copyOnWrite']=opt
  #return(x)
  }
  if(is.data.frame(x)){
    for(i in seq_len(ncol(x))){
      if(is.altrep(x)){
        sm=peekSharedMemory(x[,i])
        sm$dataInfo['copyOnWrite']=opt
      }
    }
    #return(x)
  }
}




