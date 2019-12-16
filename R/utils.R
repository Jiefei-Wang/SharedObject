setClassUnion("characterOrNULL", c("character", "NULL"))

typeSize <- c(4, 4, 8, 1, 1)
names(typeSize) <- c("logical", "integer", "double", "raw", "character")


## the size of the type
getTypeSize <- function(x) {
    if (!any(x %in% names(typeSize)))
        stop("The type has not been defined: ", x)
    unname(typeSize[x])
}

calculateSharedMemorySize <- function(x) {
    length(x) * getTypeSize(typeof(x))
}


## Copy all attributes from source to target without
## duplicate source
copyAttribute <- function(target, source) {
    oldSettings <- getSharedCopy(target)
    att = attributes(source)
    for (i in names(att)) {
        C_attachAttr(target, i, att[[i]])
    }
    target
}


#' Get the shared object usage report
#'
#' Get the shared object usage report. The size is the real memory size
#' that a system allocates for the shared object, so it might be larger
#' than the object size. The size unit is byte.
#'
#' @details
#' Since the records of shared objects created by the package are not stored in
#' a shared memory, the function finds all shared memory by searching possible IDs
#' within a specific range. Therefore, the function may not be able to find
#' all shared objects. However, If there are less than 4 billions shared object created,
#' the returned result is complete.
#'
#' @return A data.frame object with shared object id and size
#' @export
listSharedObject<-function(){
    ids <- seq_len(getLastIndex())
    usedId <- ids[vapply(ids,C_hasSharedMemory,logical(1))]
    memorySize <- vapply(usedId,C_getSharedMemorySize,double(1))
    data.frame(Id = usedId, size = memorySize)
}








is.altrep <- function(x) {
    C_ALTREP(x)
}
getLastIndex<-function(){
    C_getLastIndex()
}

## The function return the value in data 1
## if and only if x is a shared object
getAltData1 <- function(x) {
    return(C_getAltData1(x))
}

getAltData2 <- function(x) {
    return(C_getAltData2(x))
}

setAltData1 <- function(x, value) {
    return(C_setAltData1(x, value))
}

setAltData2 <- function(x, value) {
    return(C_setAltData2(x, value))
}

