setClassUnion("characterOrNULLOrMissing", c("character", "NULL", "missing"))

typeSize <- c(4, 4, 8, 1, 1)
names(typeSize) <-
    c("logical", "integer", "double", "raw", "character")

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
#' @param start the start of the ID, the default is `NULL`. see details.
#' @param end the end of the ID, the default is `NULL`. see details.
#'
#' @details
#' Since the records of shared objects created by the package are not located in
#' the shared memory, this function finds all shared objects by searching possible IDs
#' within a specific range. Therefore, if there are too many shared objects(over 4 billions)
#' ,the object id can be out of the searching range, so the result list may not be complete.
#'
#' @examples
#' ## Automatically determine the search range
#' listSharedObject()
#'
#' ## specify the search range
#' listSharedObject(start = 10, end = 20)
#'
#' ## Search from 0 to 20
#' listSharedObject(20)
#' @return A data.frame object with shared object id and size
#' @export
listSharedObject <- function(end = NULL,start = NULL) {
    if(is.null(start))
        start <- 0
    if(is.null(end))
        end <- getLastIndex()
    if(end < start)
        ids <- c()
    else
        ids <- seq_len(end - start + 1) + start - 1
    usedId <- ids[vapply(ids, C_hasSharedMemory, logical(1))]
    memorySize <- vapply(usedId, C_getSharedMemorySize, double(1))
    data.frame(Id = usedId, size = memorySize)
}



#' Whether an object is an ALTREP object
#'
#' Whether an object is an ALTREP object
#' @param x an R object
#' @examples
#' x <- share(runif(10))
#' is.altrep(x)
#' @return
#' A logical value
#' @export
is.altrep <- function(x) {
    C_ALTREP(x)
}
getLastIndex <- function() {
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


#' Find path of the shared memory header file
#'
#' This function will return the path of the shared memory header or
#' the flags that are used to compile the package
#' for the developers who want to use C++ level implementation of the
#' `SharedObject` package
#'
#' @param x Character, "PKG_LIBS" or "PKG_CPPFLAGS"
#' @return path to the header or compiler flags
#' @examples
#' SharedObject:::pkgconfig("PKG_LIBS")
#' SharedObject:::pkgconfig("PKG_CPPFLAGS")
pkgconfig <- function(x){
    space <- .Machine$sizeof.pointer
    if(space==8){
        folder <- "lib/x64"
    }else{
        folder <- "lib/i386"
    }
    if(x == "PKG_LIBS"){
        folder <- system.file(folder,
                    package = "SharedObject", mustWork = TRUE)
        files <- list.files(folder)
        if(length(files)>1){
            ind <- max(which(endsWith(files,".a")),0)
            if(ind == 0){
                ind <- max(which(endsWith(files,".dll")),0)
                stopifnot(ind != 0)
            }
            files <- files[ind]
        }
        result <- paste0(folder,"/",files)
    }else{
        result <- ""
    }
    cat(result)
}



