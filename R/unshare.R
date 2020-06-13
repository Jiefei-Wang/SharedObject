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
    result <- x
    oldAttrs <- attributes(x)
    ## we first unshare the attributes because
    ## it may be able to unshare the atomic vector as well
    ## when we call the `attributes<-` function
    if(!is.null(oldAttrs)){
        if(isSharedSEXP(x)){
            oldCopyOnWrite <- setCopyOnWrite(x, TRUE)
        }
        attrs <- unshare(oldAttrs)
        if(!C_isSameObject(oldAttrs,attrs)){
            attributes(result) <- attrs
        }
        if(isSharedSEXP(x)){
            setCopyOnWrite(x, oldCopyOnWrite)
        }
    }
    ## If the object is still a shared object.
    if(isSharedSEXP(result)){
        attris <- attributes(result)
        result <- vector(mode = typeof(x),length = length(x))
        C_memcpy(x, result, calculateSharedMemorySize(x))
        attributes(result) <- attris
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


