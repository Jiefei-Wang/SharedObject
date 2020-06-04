isSharedSEXP <- function(x, verbose = FALSE){
    shared <- FALSE
    if (is.altrep(x)) {
        dataInfoTemplate <- getDataInfoTemplate()
        info <- C_getAltData2(x)
        if (is.list(info) &&
            length(info) == length(dataInfoTemplate) &&
            identical(names(dataInfoTemplate), names(dataInfoTemplate))) {
            shared <- TRUE
        }
    }
    if(verbose){
        attributes(shared) <- is.shared(attributes(x),verbose = verbose)
    }
    shared
}

#' @rdname is.shared
#' @export
setMethod("is.shared", "ANY", function(x,...,recursive,verbose){
    if(isS4(x)){
        return(isSharedS4(x,...,recursive=recursive))
    }
    result <- isSharedSEXP(x)
})
isSharedS4 <- function(x,...,recursive,verbose){
    slots <- slotNames(x)
    res <- vector("list",length(slots))
    for(i in seq_along(slots)){
        res[[i]] <- is.shared(slot(x, slots[i]),recursive=recursive,...)
    }
    names(res) <- slots
    if(isSEXPAtomic(x) &&
       ".Data" %in% names(res)){
        res[[".Data"]] <- isSharedSEXP(x)
    }
    ## remove the empty slot
    res <- res[unlist(lapply(res, function(x) length(x) != 0))]
    if(!recursive){
        res <- lapply(res, function(x)any(unlist(x)))
    }
    res
}


#' @rdname is.shared
#' @export
setMethod("is.shared", "list", function(x,...,recursive,verbose){
    res <- lapply(x, function(x,...)is.shared(x,...),
                  recursive=recursive,...)
    if(!recursive){
        res <- lapply(res, function(x)any(unlist(x)))
    }
    res
})




