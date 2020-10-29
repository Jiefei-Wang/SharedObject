#' Create an empty shared object
#'
#' Create an empty shared object with a specific length and attributes.
#'
#' @param mode the type of the shared object
#' @param length the length of the shared object
#' @param attrib the attributes of the shared object
#' @param ... Parameters that is used to create the shared object, please
#' refer to `?share` for details.
#' @examples
#' ## Create an empty shared vector
#' x1 <- SharedObject(mode = "numeric", length = 10)
#' x1
#' ## Create an empty shared matrix
#' x2 <- SharedObject(mode = "numeric", length = 6,
#'                  attrib = list(dim = c(2L,3L)))
#' x2
#' @return An R vector
#' @export
SharedObject <- function(mode = c("raw","logical","integer","numeric","complex"),
                       length, attrib = list(), ...) {
    mode <- match.arg(mode)
    options <- completeOptions(...)
    if(options$sharedAttributes){
        attrib <- doAttributes(tryShare, attrib,...)
    }
    result <- C_createEmptySharedObject(
        type = C_getDataTypeId(mode),
        length= length,
        copyOnWrite=options$copyOnWrite,
        sharedSubset=options$sharedSubset,
        sharedCopy=options$sharedCopy,
        attributes=as.pairlist(attrib)
    )
    result
}
