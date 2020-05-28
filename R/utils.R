setClassUnion("characterOrNULLOrMissing", c("character", "NULL", "missing"))

typeSize <- c(4, 4, 8, 1)
names(typeSize) <-
    c("logical", "integer", "double", "raw")

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
    #oldSettings <- getSharedCopy(target)
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
#' @param start the start value of the ID. The default is `NULL`. See details.
#' @param end the end value of the ID. The default is `NULL`. See details.
#' @param includeCharId Whether including the shared objects named by a character ID, it only works
#' on Unix-like systems. See `?allocateNamedSharedMemory` for more information. The default is `FALSE`.
#'
#' @details
#' The parameter `start` and `end` specify the range of the ID. If not specified, all
#' IDs will be listed.
#'
#' On Ubuntu or many other Unix-like operating systems, the shared objects
#' can be found in the folder `/dev/shm`. The function can find all shared objects
#' if the folder exists.
#'
#' On Windows, since there is no easy way to find all shared objects.
#' the function will guess the range of the shared object IDs and search all IDs
#' within the range. Therefore, if there are too many shared objects(over 4 billions)
#' ,the object id can be out of the searching range and the result may not be complete.
#' Furthermore, there will be no named shared object in the returned list.
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
#' @seealso \code{\link{getLastIndex}}, \code{\link{allocateSharedMemory}},
#' \code{\link{allocateNamedSharedMemory}}, \code{\link{mapSharedMemory}}, \code{\link{unmapSharedMemory}},
#' \code{\link{freeSharedMemory}}, \code{\link{hasSharedMemory}}, \code{\link{getSharedMemorySize}}
#' @return A data.frame object with shared object id and size
#' @export
listSharedObject <- function(end = NULL,start = NULL, includeCharId = FALSE) {
    if(file.exists("/dev/shm")){
        num_name <- paste0(getOSBit(),"_num")
        char_name <- paste0(getOSBit(),"_char")
        all_ids <- getSharedFiles(showInternal = FALSE)
        usedId <- all_ids[[num_name]]
        if(!is.null(start))
            usedId <- usedId[usedId>=start]
        if(!is.null(end))
            usedId <- usedId[usedId<=end]
        if(includeCharId)
            usedId <- c(usedId, all_ids[[char_name]])
    }else{
    if(is.null(start))
        start <- 0
    if(is.null(end))
        end <- getLastIndex()
    if(end < start)
        ids <- c()
    else
        ids <- seq_len(end - start + 1) + start - 1
        usedId <- ids[vapply(ids, hasSharedMemory, logical(1))]
    }
    memorySize <- vapply(usedId, getSharedMemorySize, double(1))
    data.frame(Id = usedId, size = memorySize, row.names = NULL)
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

getOSBit <- function(){
    .Machine$sizeof.pointer * 8
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
    if(getOSBit() == 64){
        folder <- "usrlib/x64"
    }else{
        folder <- "usrlib/i386"
    }
    if(x == "PKG_LIBS"){
        folder <- system.file(folder,
                              package = "SharedObject", mustWork = FALSE)
        if(folder == ""){
            folder <- system.file("usrlib",
                                  package = "SharedObject", mustWork = TRUE)
        }
        files <- "SharedObject.a"
        result <- paste0(folder,"/",files)
    }else{
        result <- ""
    }
    cat(result)
}

## Get POSIX shared memory files
getSharedFiles <- function(showInternal = FALSE){
    files <- list.files("/dev/shm")
    bits <- as.character(c(32,64))
    res <- list()
    for(i in bits){
        header <- paste0("shared_object_package_spaceX",i,"_id_")
        index <- startsWith(files, header)
        id <- substring(files[index], nchar(header)+1)
        if(!showInternal && !is.null(id)){
            id <- id[id!= "sharedObjectCounter"]
        }
        index_num <- is.charInteger(id)
        id_num <- as.numeric(id[index_num])
        id_char <- id[!index_num]
        res[[paste0(i,"_num")]] <- id_num
        res[[paste0(i,"_char")]] <- id_char
    }
    res
}

## Alternative to stopifnot
assert <- function(expr, error){
    if(!expr){
        stop(error, call. = FALSE)
    }
}
## x can be a vector of any length
is.charInteger <-function(x){
    !grepl("\\D",x)
}

## x must be a vector of lenght 1
tryChar2Int <-function(x){
    if(is.charInteger(x)){
        x <- as.numeric(x)
    }
    x
}

