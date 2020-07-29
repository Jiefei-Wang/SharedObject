checkID <- function(x){
    stopifnot(is.numeric(x)||is.character(x))
    stopifnot(length(x)==1)
}
checkNumericID <- function(id){
    stopifnot(is.numeric(id))
    assert(length(id)==1, "The id must be a length 1 vector")
}
checkNamedID <- function(name){
    assert(is.na(suppressWarnings(as.numeric(name))), "The named id must not be a number")
    stopifnot(is.character(name))
    assert(length(name)==1, "The named id must be a length 1 vector")
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
#' on some linux systems. See details and `?allocateNamedSharedMemory` for more information. 
#' The default is `FALSE`.
#'
#' @details
#' The parameter `start` and `end` specify the range of the ID. If not specified, all
#' IDs will be listed.
#'
#' On Ubuntu or some other linux systems, the shared objects
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
        all_ids <- getSharedFiles()
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



##########################################
## Functions to create shared memory
##########################################
#' Functions to manipulate shared memory
#'
#' These functions are for package developers only, they can
#' allocate, open, close and destroy shared memory without touching
#' C++ code. Normal users should not use these functions unless
#' dealing with memory leaking
#'
#' @param name character(1), a single name that names the shared memory
#' @param x integer(1) or character(1), an ID or a name that is used to find
#' the shared memory. If x is a character with pure number, it will be
#' treated as an ID.
#' @param size numeric(1), the size of the shared memory that you want to allocate
#'
#' @details
#' \strong{Quick explanation}
#'
#' `getLastIndex`: the ID of the last created shared memory.
#'
#' `allocateSharedMemory`: allocate a shared memory of a given size,
#' the memory ID is returned by the function
#'
#' `allocateNamedSharedMemory`: allocate a shared memory of a given size, the memory
#' can be found by the name that is passed to the function.
#'
#' `mapSharedMemory`: map the shared memory to the current process memory space
#'
#' `unmapSharedMemory`: unmap the shared memory(without destroying it)
#'
#' `freeSharedMemory`: destroy the shared memory. This function will only unmap the
#' shared memory on Windows.
#'
#' `hasSharedMemory`: whether the memory exist?
#'
#' `getSharedMemorySize`: get the actual size of the shared memory, it may be larger than the
#' size you required.
#'
#' \strong{Details}
#'
#' Creating and using shared memory involves three steps: allocating, mapping, and
#' destroying the shared memory. There are two types of naming scheme that you can
#' use to find the shared memory: an integer ID or a character name. They are determined
#' in the first creation step.
#'
#' The shared memory can be created by `allocateSharedMemory` or
#' `allocateNamedSharedMemory`.
#' The function `allocateSharedMemory` will return the ID of the shared memory.
#' After creating the shared memory, it can be mapped to the current process by
#' `mapSharedMemory`. The return value is an external pointer to the shared memory.
#' Once the shared memory is no longer needed, it can be destroyed by `freeSharedMemory`.
#' There is no need to unmap the shared memory unless you intentionally want to do so.
#'
#'
#' @return
#' `getLastIndex`: An interger ID served as a hint of the last created shared memory ID.
#'
#' `allocateSharedMemory`: an integer ID that can be used to find the shared memory
#'
#' `allocateNamedSharedMemory`: no return value
#'
#' `mapSharedMemory`: An external pointer to the shared memory
#'
#' `unmapSharedMemory`: Logical value indicating whether the operation is success.
#'
#' `freeSharedMemory`: Logical value indicating whether the operation is success.
#'
#' `hasSharedMemory`: Logical value indicating whether the shared memory exist
#'
#' `getSharedMemorySize`: A numeric value
#' @seealso \code{\link{listSharedObject}}
#' @rdname developer-API
#' @examples
#' size <- 10L
#' ## unnamed shared memory
#' id <- allocateSharedMemory(size)
#' hasSharedMemory(id)
#' ptr <- mapSharedMemory(id)
#' ptr
#' getSharedMemorySize(id)
#' freeSharedMemory(id)
#' hasSharedMemory(id)
#'
#' ## named shared memory
#' name <- "SharedObjectExample"
#' if(!hasSharedMemory(name)){
#'     allocateNamedSharedMemory(name,size)
#'     hasSharedMemory(name)
#'     ptr <- mapSharedMemory(name)
#'     ptr
#'     getSharedMemorySize(name)
#'     freeSharedMemory(name)
#'     hasSharedMemory(name)
#' }
#' @export
getLastIndex <- function(){
    C_getLastIndex()
}


#' @rdname developer-API
#' @export
allocateSharedMemory <- function(size){
    C_allocateSharedMemory(size)
}
#' @rdname developer-API
#' @export
allocateNamedSharedMemory <- function(name,size){
    checkNamedID(name)
    C_allocateNamedSharedMemory(name,size)
}
#' @rdname developer-API
#' @export
mapSharedMemory <- function(x){
    checkID(x)
    x <- tryChar2Int(x)
    if(is.numeric(x)){
        C_mapSharedMemory(x)
    }else{
        C_mapNamedSharedMemory(x)
    }
}
#' @rdname developer-API
#' @export
unmapSharedMemory <- function(x){
    checkID(x)
    x <- tryChar2Int(x)
    if(is.numeric(x)){
        C_unmapSharedMemory(x)
    }else{
        C_unmapNamedSharedMemory(x)
    }
}
#' @rdname developer-API
#' @export
freeSharedMemory <- function(x){
    checkID(x)
    x <- tryChar2Int(x)
    if(is.numeric(x)){
        C_freeSharedMemory(x)
    }else{
        C_freeNamedSharedMemory(x)
    }
}
#' @rdname developer-API
#' @export
hasSharedMemory <- function(x){
    checkID(x)
    x <- tryChar2Int(x)
    if(is.numeric(x)){
        C_hasSharedMemory(x)
    }else{
        C_hasNamedSharedMemory(x)
    }
}
#' @rdname developer-API
#' @export
getSharedMemorySize <- function(x){
    checkID(x)
    x <- tryChar2Int(x)
    if(is.numeric(x)){
        C_getSharedMemorySize(x)
    }else{
        C_getNamedSharedMemorySize(x)
    }
}


