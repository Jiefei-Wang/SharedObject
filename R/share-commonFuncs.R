doList <- function(func,tryFunc, x, ...){
    ## We do not create a new list unless
    ## there exists any sharable elements
    for(i in seq_along(x)){
        Elt <- func(x[[i]],...)
        if(!C_isSameObject(x[[i]],Elt)){
            x[[i]] <- Elt
        }
    }
    ## share attributes
    oldAttrs <- attributes(x)
    if(!is.null(oldAttrs)){
        ## Exclude names and class attributes.
        ## The former one will cause an infinit loop.
        ## If attributes(x) contains the names attribute,
        ## attributes(attributes(x)) still contains the names attributes,
        ## we must ignore it.
        ## The later one is not sharable
        if(!all(names(oldAttrs) %in% c("class","names"))){
            attrs <- tryFunc(oldAttrs,...)
            ## set the attributes only when it is different from the old one
            if(!C_isSameObject(oldAttrs,attrs)){
                attributes(x) <- attrs
            }
        }
    }
    x
}
doS4 <- function(func, x, ...){
    ## If the object is an S4SXP,
    ## share its slots
    if(isSharableAtomic(x)||isSEXPList(x)){
        ## If the object is not an S4SXP,
        ## Calling the right share method
        C_UNSETS4(x)
        if(isS4(x))
            stop("Unexpected error, cannot convert an S4 object to a non-S4 object")
        x1 <- func(x,...)
        C_SETS4(x)
        C_SETS4(x1)
    }else{
        x1 <- x
        slots <- slotNames(x1)
        for(i in slots){
            slot(x1, i, check = FALSE) <- func(slot(x, i), ...)
        }
    }
    validObject(x1)
    x1
}
