typeSize <- list(
    logical = 4,
    integer = 4,
    double = 8,
    raw = 1,
    complex = 16
)
## the size of the type
getTypeSize <- function(x) {
    if (is.null(typeSize[[x]]))
        stop("The type has not been defined: ", x)
    typeSize[[x]]
}

calculateSharedMemorySize <- function(x) {
    length(x) * getTypeSize(typeof(x))
}



getDataInfoTemplate <- function(...){
    args <- list(...)
    dataInfoPropNames = c("dataId", "length", "totalSize", "dataType", "ownData")
    dataInfoNames = c(dataInfoPropNames, sharedAtomicOptions)
    dataInfo = as.list(rep(0, length(dataInfoNames)))
    names(dataInfo) = dataInfoNames
    if(any(!names(args)%in%dataInfoNames)){
        stop("Illegal data info name has been found")
    }
    dataInfo[names(args)] <- args
    dataInfo
}


isSharableAtomic <- function(x){
    typeof(x) %in% c("raw","logical","integer","double","complex")
}
isSEXPList <- function(x){
    is.list(x)&&!is.pairlist(x)
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

