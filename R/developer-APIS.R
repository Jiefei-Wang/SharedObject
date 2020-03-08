checkID <- function(x){
    stopifnot(is.numeric(x)||is.character(x))
    stopifnot(length(x)==1)
}
checkNumericID <- function(id){
    stopifnot(is.numeric(id))
    stopifnot(length(id)==1)
}
checkNamedID <- function(name){
    stopifnot(is.character(name))
    stopifnot(length(name)==1)
}

#' Functions to manipulate shared memory
#'
#' These functions are for package developers only, they can
#' allocate, open, close and destroy shared memory without touching
#' C++ code. Normal users should not use these functions unless
#' dealing with memory leaking
#'
#' @param name character(1), a single name that names the shared memory
#' @param x integer(1) or character(1), an ID or a name that is used to find
#' the shared memory.
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
    if(is.numeric(x)){
        C_getSharedMemorySize(x)
    }else{
        C_getNamedSharedMemorySize(x)
    }
}


