


globalSettings=new.env()
globalSettings$copyOnWrite=TRUE
globalSettings$sharedSubset=TRUE
globalSettings$sharedCopy=FALSE

#' Get or set the global options for the SharedObject package
#'
#' @param ...
#' `setSharedObjectOptions`: the options you want to set, it can be copyOnWrite,
#' sharedSubset and sharedCopy.
#'
#' `getSharedObjectOptions`: it is a character vector. If empty, all options will be returned.
#' @rdname sharedObjectOptions
#' @export
setSharedObjectOptions<-function(...){
  options=list(...)
  options=checkOptionExistance(options)
  for(i in seq_along(options)){
    globalSettings[[names(options)[i]]]=options[[i]]
  }
}

#' @rdname sharedObjectOptions
#' @export
getSharedObjectOptions<-function(...){
 options=list(...)
 if(length(options)==0){
   return(as.list(globalSettings))
 }
 return(as.list(globalSettings)[options])
}

## Check if options exist or not
## return the options that exist
checkOptionExistance<-function(options){
  noneExistOptions=!names(options)%in%names(globalSettings)
  if (any(noneExistOptions)) {
    sapply(paste0(names(options)[noneExistOptions]), function(x)
      warning(paste0("The option `", x, "` does not exist")))
  }
  options=options[!noneExistOptions]
  return(options)
}
