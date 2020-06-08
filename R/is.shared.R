## recursive: Whether to show the details of all elements
## of the object x which are not directly associated with x
## showAttributes: whether to show the information of the attributes of x
isSharedSEXP <- function(x, showAttributes = FALSE){
    result <- FALSE
    if (is.altrep(x)) {
        dataInfoTemplate <- getDataInfoTemplate()
        info <- C_getAltData2(x)
        if (is.list(info) &&
            length(info) == length(dataInfoTemplate) &&
            identical(names(dataInfoTemplate), names(dataInfoTemplate))) {
            result <- TRUE
        }
    }
    if(showAttributes&&!is.null(attributes(x))){
        attr(result, "sharedAttributes") <- is.shared(attributes(x), showAttributes = FALSE)
    }
    result
}

isSharedANY <- function(x,...,recursive,showAttributes){
    ## If the object is neither an S4 object or a list
    ## Just check if the SEXP is a shared altrep object
    result <- isSharedSEXP(x,showAttributes=showAttributes)
    result
}
isSharedList <- function(x,...,recursive,showAttributes){
    result <- lapply(x, function(x,...)is.shared(x,...),
                     ...,recursive=recursive,showAttributes=FALSE)
    if(!recursive){
        result <- lapply(result, function(x)any(unlist(x)))
    }
    if(showAttributes&&!is.null(attributes(x))){
        attr(result, "sharedAttributes") <- is.shared(attributes(x), showAttributes = FALSE)
    }
    result
}

isSharedS4 <- function(x,...,recursive,showAttributes){
    slots <- slotNames(x)
    result <- vector("list",length(slots))
    for(i in seq_along(slots)){
        result[[i]] <- is.shared(slot(x, slots[i]),...,recursive=recursive, showAttributes = FALSE)
    }
    names(result) <- slots
    if(".Data" %in% names(result)){
        if(isSEXPAtomic(x)){
            result[[".Data"]] <- isSharedSEXP(x,showAttributes = FALSE)
        }
        if(isSEXPList(x)){
            names(result[[".Data"]]) <- names(x)
        }
    }
    ## remove the empty slot
    result <- result[unlist(lapply(result, function(x) length(x) != 0))]
    if(!recursive){
        result <- lapply(result, function(x)any(unlist(x)))
    }
    result
}

#' @rdname is.shared
#' @export
setMethod("is.shared", "ANY", function(x,...,recursive,showAttributes){
    if(isS4(x)){
        return(isSharedS4(x,...,recursive=recursive,showAttributes=showAttributes))
    }
    if(isSEXPList(x)){
        return(isSharedList(x,...,recursive=recursive,showAttributes=showAttributes))
    }
    isSharedANY(x,...,recursive=recursive,showAttributes=showAttributes)
})





