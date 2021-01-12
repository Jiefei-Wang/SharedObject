#' Whether an object is an ALTREP object
#'
#' Whether an object is an ALTREP object
#' @param x an R object
#' @return
#' A logical value
is.altrep <- function(x) {
    C_ALTREP(x)
}


isSharableAtomic <- function(x){
    typeof(x) %in% c("raw","logical","integer","double","complex","character")
}
isSEXPList <- function(x){
    is.list(x)&&!is.pairlist(x)
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
    if(x == "PKG_LIBS"){
        if(.Platform$OS.type=="windows"){
            folder <- sprintf("usrlib/%s", .Platform$r_arch)
            folder <- system.file(folder,
                                  package = "SharedObject", mustWork = FALSE)
        }else{
            folder <- system.file("usrlib",
                                  package = "SharedObject", mustWork = FALSE)
        }
        files <- "SharedObject.a"
        result <- file.path(folder,files)
    }else{
        result <- ""
    }
    cat(result)
    invisible(result)
}

## Get POSIX shared memory files
getSharedFiles <- function(){
    files <- list.files(C_getSharedMemoryPath())
    bits <- as.character(c(32,64))
    res <- list()
    for(i in bits){
        headerId <- paste0("SO_X",i,"_")
        idIndex <- startsWith(files, headerId)
        objectId <- substring(files[idIndex], nchar(headerId)+1)
        res[[as.character(i)]] <- objectId
    }
    res
}

setVerbose<- function(x){
    C_setSharedMemoryPrint(x)
    C_setAltrepPrint(x)
    C_setPackagePrint(x)
}


processArgs <- function(args, literal){
    argNames <- names(args)
    argsSetCommand <- list()
    argsGetCommand <- c()
    if("args"%in%names(args)){
        return(eval(args$args, envir = parent.frame(n=2)))
    }
    for(i in seq_along(args)){
        curArg <- args[[i]]
        if(is.null(argNames)||argNames[i]==""){
            if(literal){
                curArg <- as.character(args[[i]])
            }else{
                curArg <- eval(curArg, envir = parent.frame(n=2))
            }
            argsGetCommand <- c(argsGetCommand, curArg)
        }else{
            if(argNames[i]!="literal"){
                if(is.language(curArg)){
                    curArg <-eval(curArg, envir = parent.frame(n=2))
                }
                argsSetCommand[argNames[i]] <- curArg
            }
        }
    }
    list(argsGetCommand= argsGetCommand, argsSetCommand=argsSetCommand)
}


getOS <- function(){
    sysinf <- Sys.info()
    if (!is.null(sysinf)){
        os <- sysinf['sysname']
        if (os == 'Darwin')
            os <- "osx"
    } else { ## mystery machine
        os <- .Platform$OS.type
        if (grepl("^darwin", R.version$os))
            os <- "osx"
        if (grepl("linux-gnu", R.version$os))
            os <- "linux"
    }
    tolower(os)
}
