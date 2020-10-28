unshareANY <- function(x){
    if(isS4(x)){
        return(unshareS4(x))
    }else if(isSharableAtomic(x)){
        return(unshareAtomic(x))
    }else if(is.list(x)){
        return(unshareList(x))
    }else if(is.environment(x)){
        return(unshareEnvironment(x))
    }
    x
}

## x must be an atomic object
unshareAtomic <- function(x) {
    oldAttrs <- attributes(x)
    newAttrs <- unshare(oldAttrs)
    ## If the object is a shared object.
    if(C_isShared(x)){
        if(typeof(x)!="character"){
            unshareFunc <- C_unshare
        }else{
            unshareFunc <- C_unshareString
        }
        result<- unshareFunc(x, as.pairlist(newAttrs))
    }else{
        result <- x
        if(!C_isSameObject(newAttrs,oldAttrs)){
            attributes(result) <- newAttrs
        }
    }
    result
}

unshareList <- function(x) {
    doList(unshare,unshare,x)
}

unshareS4 <- function(x){
    doS4(unshare,x)
}
unshareEnvironment <- function(x){
    doEnvironment(unshare,unshare,x)
}

#' @export
setMethod("unshare", signature(x = "ANY"), unshareANY)


