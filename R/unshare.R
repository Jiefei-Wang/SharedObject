unshareAttributes<-function(x){
    oldAttrs <- attributes(x)
    attrs <- attributes(x)
    if(!is.null(attrs)){
        if(any(!names(attrs)%in%c("names","class"))){
            for(i in seq_along(attrs)){
                unSharedAttrs <- unshare(attrs[[i]])
                if(!C_isSameObject(attrs[[i]],unSharedAttrs)){
                    attrs[[i]] <- unSharedAttrs
                }
            }
        }
        if(!C_isSameObject(attrs,oldAttrs)){
            attributes(x) <- attrs
        }
    }
    x
}
unshareANY <- function(x){
    if(isS4(x)){
        return(unshareS4(x))
    }
    dataType <- C_getType(x)
    if(isSEXPAtomic(x)){
        return(unshareAtomic(x))
    }
    if(isSEXPList(x)){
        return(unshareList(x))
    }
    x
}

## x must be an atomic object
unshareAtomic <- function(x) {
    ## if the object x has been shared and
    ## all the atomic options are the same,
    ## we will return the same object.
    result <- x
    attris <- attributes(x)
    if(!is.null(attris)){
        if(isSharedSEXP(x)){
            oldCopyOnWrite <- setCopyOnWrite(x, TRUE)
        }
        result <- unshareAttributes(x)
        if(isSharedSEXP(x)){
            setCopyOnWrite(x, oldCopyOnWrite)
        }
    }
    if(isSharedSEXP(result)){
        attris <- attributes(result)
        result <- vector(mode = C_getType(x),length = length(x))
        C_memcpy(x, result, calculateSharedMemorySize(x))
        attributes(result) <- attris
    }
    result
}

unshareList <- function(x,...) {
    result <- lapply(x,unshare,...)
    attributes(result) <- attributes(x)
    result <- unshareAttributes(result)
    result
}

unshareS4 <- function(x){
    dataType <- C_getType(x)
    ## If the object is an S4SXP,
    ## share its slots
    if(dataType=="S4"||
       dataType=="string"||
       dataType=="other"){
        x1 <- x
        slots <- slotNames(x1)
        for(i in slots){
            slot(x1, i, check = FALSE) <- unshare(slot(x, i))
        }
    }else{
        ## If the object is not an S4SXP,
        ## Calling the right share method
        C_UNSETS4(x)
        if(isS4(x))
            stop("Unexpected error, cannot convert an S4 object to a non-S4 object")
        x1 <- unshare(x)
        C_SETS4(x)
        C_SETS4(x1)
    }
    x1
}


#' @export
setMethod("unshare", signature(x = "ANY"), unshareANY)


