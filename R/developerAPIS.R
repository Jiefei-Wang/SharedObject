#' List all shared Objects
#'
#' @param start the start value of the ID. The default is `NULL`. See details.
#' @param end the end value of the ID. The default is `NULL`. See details.
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
#' the function will guess the range of the shared object IDs and search
#' for all IDs within the range. Therefore, if there are too many shared
#' objects(over 4 billions) ,the object id can be out of the searching range
#' and the result may not be complete.
#' Furthermore, there will be no named shared memory in the returned list.
#'
#' Note that the size in the return value is the true memory size
#' that is reserved for the shared object, so it might be larger
#' than the object size.
#'
#' @examples
#' x <- share(runif(10))
#' ## Automatically determine the search range
#' listSharedObjects()
#'
#' ## specify the search range
#' listSharedObjects(start = 10, end = 20)
#'
#' ## Search from 0 to 20
#' listSharedObjects(20)
#' @seealso \code{\link{getLastIndex}}, \code{\link{allocateSharedMemory}},
#' \code{\link{mapSharedMemory}}, \code{\link{unmapSharedMemory}},
#' \code{\link{freeSharedMemory}}, \code{\link{hasSharedMemory}}, \code{\link{getSharedMemorySize}}
#' @return A data.frame object with shared object id and size
#' @export
listSharedObjects <- function(end = NULL,start = NULL) {
    shared_memory_path <- C_getSharedMemoryPath()
    if(shared_memory_path!="" && file.exists(shared_memory_path)){
        usedIds <- getSharedFiles()
        usedIds <- usedIds[[as.character(getOSBit())]]
        if(!is.null(start))
            usedIds <- usedIds[usedIds>=start]
        if(!is.null(end))
            usedIds <- usedIds[usedIds<=end]
    }else{
        if(is.null(start))
            start <- 0
        if(is.null(end))
            end <- getLastIndex()
        if(end < start)
            ids <- c()
        else
            ids <- seq_len(end - start + 1) + start - 1
        usedIds <- ids[vapply(ids, hasSharedMemory, logical(1))]
    }
    memorySize <- unlist(lapply(usedIds, function(id)
        {
        tryCatch(
            getSharedMemorySize(id),
            error = function(e) -1
        )
    }))
    idx <- (memorySize != -1)
    data.frame(Id = usedIds[idx], size = memorySize[idx], row.names = NULL)
}



##########################################
## Functions to create shared memory
##########################################
#' Functions to manipulate shared memory
#'
#' These functions are designed for package developers only, they can
#' allocate, open, close and destroy shared memory without touching
#' C++ code. Normal users should not use these functions unless
#' dealing with memory leaking
#'
#' @param name,id The name of the shared memory
#' @param size The size of the shared memory that you want to allocate
#'
#' @details
#' \strong{Quick explanation}
#'
#' `getLastIndex`: the ID of the last created shared memory.
#'
#' `allocateSharedMemory`: allocate a shared memory of a given size,
#' the memory ID is returned by the function
#'
#' `mapSharedMemory`: map the shared memory to the current process memory space
#'
#' `unmapSharedMemory`: unmap the shared memory(without destroying it)
#'
#' `freeSharedMemory`: free the shared memory. This function will only unmap the
#' shared memory on Windows.
#'
#' `hasSharedMemory`: whether the memory exist?
#'
#' `getSharedMemorySize`: get the actual size of the shared memory, it may be larger than the
#' size that you required.
#'
#' \strong{Details}
#'
#' A complete lifecycle of a shared memory involves four steps:
#' allocating, mapping, unmapping and freeing the shared memory.
#'
#' The shared memory can be created by `allocateSharedMemory`.
#' The function `allocateSharedMemory` will return the ID of the shared memory.
#' After creating the shared memory, it can be mapped to the current process by
#' `mapSharedMemory`. The return value is an external pointer to the shared memory.
#' Once the shared memory is no longer needed, it can be unmapped and destroyed by
#' `unmapSharedMemory` and `freeSharedMemory` respectively.
#'
#'
#' @return
#' `getLastIndex`: An interger ID served as a hint of the last created shared memory ID.
#'
#' `allocateSharedMemory`: character ID(s) that can be used to find the shared memory
#'
#' `mapSharedMemory`: External pointer(s) to the shared memory
#'
#' `unmapSharedMemory`: No return value
#'
#' `freeSharedMemory`: No return value
#'
#' `hasSharedMemory`: Logical value(s) indicating whether the shared memory exist
#'
#' `getSharedMemorySize`: A numeric value
#' @seealso \code{\link{listSharedObjects}}
#' @rdname developer-API
#' @examples
#' size <- 10L
#' ## unnamed shared memory
#' id <- allocateSharedMemory(size)
#' hasSharedMemory(id)
#' ptr <- mapSharedMemory(id)
#' ptr
#' getSharedMemorySize(id)
#' unmapSharedMemory(id)
#' freeSharedMemory(id)
#' hasSharedMemory(id)
#'
#' ## named shared memory
#' name <- "SharedObjectExample"
#' if(!hasSharedMemory(name)){
#'     allocateSharedMemory(size, name = name)
#'     hasSharedMemory(name)
#'     ptr <- mapSharedMemory(name)
#'     ptr
#'     getSharedMemorySize(name)
#'     unmapSharedMemory(name)
#'     freeSharedMemory(name)
#'     hasSharedMemory(name)
#' }
#' @export
getLastIndex <- function(){
    C_getLastIndex()
}

#' @rdname developer-API
#' @export
allocateSharedMemory <- function(size, name = ""){
    if(length(size)>1&&name==""){
        name <- rep(name, length(size))
    }
    stopifnot(length(size)==length(name))
    if(length(size)>1){
        lapply(seq_along(size),
               function(i)allocateSharedMemory(size[i], name[i]))
    }else{
        C_allocateSharedMemory(size, name)
    }
}
#' @rdname developer-API
#' @export
mapSharedMemory <- function(id){
    if(length(id)>1){
        lapply(id, mapSharedMemory)
    }else{
        id <- as.character(id)
        C_mapSharedMemory(id)
    }
}
#' @rdname developer-API
#' @export
unmapSharedMemory <- function(id){
    if(length(id)>1){
        lapply(id, unmapSharedMemory)
    }else{
        id <- as.character(id)
        C_unmapSharedMemory(id)
    }
    invisible()
}
#' @rdname developer-API
#' @export
freeSharedMemory <- function(id){
    if(length(id)>1){
        lapply(id, freeSharedMemory)
    }else{
        id <- as.character(id)
        C_freeSharedMemory(id)
    }
    invisible()
}
#' @rdname developer-API
#' @export
hasSharedMemory <- function(id){
    if(length(id)>1){
        lapply(id, hasSharedMemory)
    }else{
        id <- as.character(id)
        C_hasSharedMemory(id)
    }
}
#' @rdname developer-API
#' @export
getSharedMemorySize <- function(id){
    if(length(id)>1){
        lapply(id, getSharedMemorySize)
    }else{
        id <- as.character(id)
        C_getSharedMemorySize(id)
    }
}
#' @rdname developer-API
#' @export
initialSharedObjectPackageData <- function(){
    C_initialPkgData()
}
#' @rdname developer-API
#' @export
releaseSharedObjectPackageData <- function(){
    C_releasePkgData()
}
