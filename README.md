# Note for developers
We plan to make a major update in Bioc 3.13, this includes performance improvement and more general C++ level API design. The update should only have minor effect on users, but if you plan to develop a package based upon `SharedObject`, it is recommended to use the package in Bioc 3.13 for the old version will not be supported in the next release.

# Introduction
`SharedObject` is designed for sharing data across many R workers. It allows multiple workers to read and write the same R object located in the same memory location. This feature is useful in parallel computing when a large R object needs to be read by all R workers. It has the potential to reduce the memory consumption and the overhead of data transmission. 


# Quick example
To share an R object, all you need to do is to call the `share` function with the object you want to share. In this example, we will create a cluster with 2 workers and share an n-by-n matrix `A`, we use the function `share` to create a shared object `shared_A` and call the function `clusterExport` to export it:


```r
library(parallel)
## Create the cluster
cl <- makeCluster(2)
## Create data
n <- 3
A <- matrix(1:(n^2), n, n)
## Create a shared object
shared_A <- share(A)
## Export the shared object
clusterExport(cl,"shared_A")
## Check the exported object
clusterEvalQ(cl, shared_A)
#> [[1]]
#>      [,1] [,2] [,3]
#> [1,]    1    4    7
#> [2,]    2    5    8
#> [3,]    3    6    9
#> 
#> [[2]]
#>      [,1] [,2] [,3]
#> [1,]    1    4    7
#> [2,]    2    5    8
#> [3,]    3    6    9

stopCluster(cl)
```
As the code shows above, the procedure of exporting a shared object to the other R workers is the same as the procedure of exporting a regular R object. Notably, there is no visible difference between the matrix `A` and the shared matrix `shared_A`. There is no need to change the existing code to work with the shared object. We can verify this through

```r
## Check the data
A
#>      [,1] [,2] [,3]
#> [1,]    1    4    7
#> [2,]    2    5    8
#> [3,]    3    6    9
shared_A
#>      [,1] [,2] [,3]
#> [1,]    1    4    7
#> [2,]    2    5    8
#> [3,]    3    6    9
## Check the class
class(A)
#> [1] "matrix" "array"
class(shared_A)
#> [1] "matrix" "array"
## Check if they are identical
identical(A, shared_A)
#> [1] TRUE
```
Users can treat the shared object `shared_A` as a regular matrix and do operations on it as usual. If one needs to distinguish the shared object, the function `is.shared` can be used to check whether an object is shared.

```r
## Check if an object is shared
is.shared(A)
#> [1] FALSE
is.shared(shared_A)
#> [1] TRUE
```
There are several properties associated with the shared object, one can check them via

```r
## This function has been renamed to `sharedObjectProperties` 
## in Bioc 3.13
## get a summary report
getSharedObjectProperty(shared_A)
#> $dataId
#> [1] 47
#> 
#> $length
#> [1] 9
#> 
#> $totalSize
#> [1] 36
#> 
#> $dataType
#> [1] 13
#> 
#> $ownData
#> [1] TRUE
#> 
#> $copyOnWrite
#> [1] TRUE
#> 
#> $sharedSubset
#> [1] FALSE
#> 
#> $sharedCopy
#> [1] FALSE

## get the individual properties
getCopyOnWrite(shared_A)
#> [1] TRUE
getSharedSubset(shared_A)
#> [1] FALSE
getSharedCopy(shared_A)
#> [1] FALSE
```
The properties can be changed via `setCopyOnWrite`, `setSharedSubset` and `setSharedCopy`. Please see `Global options` and `Advanced topics` sections to see the meaning of the properties and how to use them in a proper way.

# Supported data types and structures
For the basic R type, the function supports `raw`, `logical`, `integer`, `double` and `complex`. `character` is not supported as it closely relates to R cache(`character` has been supported in Bioc 3.13).

For the container, the function supports `list`, `pairlist` and `environment`. Sharing a container is equivalent to sharing all elements in the container, the container itself will not be shared. Therefore, adding or replacing an element in a shared container in one worker will not implicitly change the shared container in the other workers. Since a data frame is fundamentally a list object, sharing a data frame will follow the same principle. 

For the more complicated data structure like `S3` and `S4` class. They are available out-of-box. Therefore, there is no need to customize the `share` function to support an S3/S4 class. However, if the S3/S4 class has a special design(e.g. on-disk data), the function `share` is an S4 generic and developers are free to define their own `share` method.

When an object is not sharable, no error will be given and the same object will be returned. The argument `mustWork = TRUE` can be used if you want to make sure the return value is a shared object.

```r
## the element `A` is sharable and `B` is not
x <- list(A = 1:3, B = as.symbol("x"))

## No error will be given, 
## but the element `B` is not shared
shared_x <- share(x)

## Use the `mustWork` argument
## An error will be given for the non-sharable object `B`
tryCatch({
  shared_x <- share(x, mustWork = TRUE)
},
error=function(msg)message(msg$message)
)
#> The object of the class <name> cannot be shared.
#> To suppress this error and return the same object, 
#> provide `mustWork = FALSE` as a function argument
#> or change its default value in the package settings
```
As we mentioned before, the package provides `is.shared` function to identify a shared object.
By default, `is.shared` function returns a single logical value indicating whether the object is a shared object or contains any shared objects. If the object is a container(e.g. list), you can explore the details using the `depth` parameter.

```r
## A single logical is returned
is.shared(shared_x)
#> [1] TRUE
## Check each element in x
is.shared(shared_x, depth = 1)
#> $A
#> [1] TRUE
#> 
#> $B
#> [1] FALSE
```

# Global options
There are some options that can control the creation and the behavior of a shared object, you can view them via

```r
## This function has been renamed to `sharedObjectPkgOptions` 
## in Bioc 3.13
getSharedObjectOptions()
#> $mustWork
#> [1] FALSE
#> 
#> $copyOnWrite
#> [1] TRUE
#> 
#> $sharedSubset
#> [1] FALSE
#> 
#> $sharedCopy
#> [1] FALSE
```
As we have seen previously, the option `mustWork` suppress the error message when the function `share` encounter a non-sharable object and force the function to return the same object. The option `sharedSubset` controls whether the subset of a shared object is still a shared object. We will talk about the options `copyOnWrite` and `sharedCopy` in the advanced section, but for most users it is safe to ignore these two options. The global setting can be modified via `setSharedObjectOptions`

```r
## These functions have been renamed to `sharedObjectPkgOptions` 
## in Bioc 3.13
## change the default setting
setSharedObjectOptions(mustWork = TRUE)
## Check if the change is made
getSharedObjectOptions("mustWork")
#> [1] TRUE
## Resume to default
setSharedObjectOptions(mustWork = FALSE)
```
Note that all the options can be temporary overwritten by providing the named parameter to the function `share`. For example, you can also turn `mustwork` on via `share(x, mustWork = TRUE)`.

# Advanced topics
## Copy-On-Write
Since all workers are using shared objects located in the same memory location, a change made on a shared object in one worker can affect the value of the object in the other workers. To prevent users from changing the values of a shared object without awareness, a shared object will duplicate itself if a change of its value is made. For example

```r
x1 <- share(1:4)
x2 <- x1

## x2 became a regular R object after the change
is.shared(x2)
#> [1] TRUE
x2[1] <- 10L
is.shared(x2)
#> [1] FALSE

## x1 is not changed
x1
#> [1] 1 2 3 4
x2
#> [1] 10  2  3  4
```
The change made on `x2` results in a memory duplication. The vector `x1` is intact. This default behavior can be overwritten by passing an argument `copyOnWrite` to the function `share`

```r
x1 <- share(1:4, copyOnWrite=FALSE)
x2 <- x1

## x2 will not be duplicated when a change is made
is.shared(x2)
#> [1] TRUE
x2[1] <- 0L
is.shared(x2)
#> [1] TRUE

## x1 has been changed
x1
#> [1] 0 2 3 4
x2
#> [1] 0 2 3 4
```
A change in the matrix `x2` cause a change in `x1`. This feature could be potentially useful to return the result from each R worker without additional memory allocation. A shared object can be pre-allocated to collect the final result from workers. However, due to the limitation of R, it is possible to change the value of a shared object unexpectedly. For example


```r
x <- share(1:4, copyOnWrite = FALSE)
x
#> [1] 1 2 3 4
-x
#> [1] -1 -2 -3 -4
x
#> [1] -1 -2 -3 -4
```
The above example shows a surprising result when the copy-on-write feature is off. Simply calling an unary function can change the values of a shared object. Therefore, users must use the feature with caution. The copy-on-write feature of an object can be set via the `setCopyOnwrite` function or the `copyOnWrite` parameter in the `share` function.


```r
## Create x1 with copy-on-write off
x1 <- share(1:4, copyOnWrite=FALSE)
x2 <- x1
## change the value of x2
x2[1] <- 0L
## Both x1 and x2 are affected
x1
#> [1] 0 2 3 4
x2
#> [1] 0 2 3 4

## Enable copy-on-write
## x2 is now independent with x1
setCopyOnWrite(x2, TRUE)
x2[2] <- 0L
## only x2 is affected
x1
#> [1] 0 2 3 4
x2
#> [1] 0 0 3 4
```
This flexibility provide us a way to do safe operations during the computation and return the results without memory duplication.

### Warning
If a high-precision value is assigned to a low-precision shared object(E.g. assigning a numeric value to an integer shared object), an implicit type conversion will be triggered for correctly storing the change. The resulting object would be a regular R object, not a shared object. Therefore, the change will not be broadcasted even if the copy-on-write feature is off. Users should be cautious with the data type that a shared object is using.

## shared copy
The options `sharedCopy` determines if the duplication of a shared object is still a shared object. For example

```r
x1 <- share(1:4)
x2 <- x1
## x2 is not shared after the duplication
is.shared(x2)
#> [1] TRUE
x2[1] <- 0L
is.shared(x2)
#> [1] FALSE


x1 <- share(1:4, sharedCopy = TRUE)
x2 <- x1
## x2 is still shared(but different from x1) 
## after the duplication
is.shared(x2)
#> [1] TRUE
x2[1] <- 0L
is.shared(x2)
#> [1] TRUE
```
For performance consideration, the default settings are `sharedCopy=FALSE`, but you can turn it on and off at any time via `setSharedCopy`. Please note that `sharedCopy` is only available when `copyOnWrite = TRUE`.

## listing the shared object 
For listing the shared objects the package created, it can be done via
```
listSharedObject()
```
Getting a list of shared object should have a rare use case, but the function can be useful if you have a memory leaking problem. The shared object can be manually released by `freeSharedMemory(ID)`.

# Developing package based upon SharedObject
The package offers three levels of APIs to help the package developers to build their own shared object. 

## user API
The simplest and recommended way to make your own shared object is to define an S4 function `share` in your own package, where you can rely on the existing `share` functions to quickly add the support for an S4 class which is not provided by `SharedObject`. We recommend to use this method to build your package for the developers do not have to bother with the memory management. The package will automatically free the shared object after use.

## R memory management APIs
It is a common request to have a low level control to the shared memory. To achieve that, the package exports some low-level R APIs for the developers who want to have a fine control of their shared objects. These functions are `allocateSharedMemory`, `allocateNamedSharedMemory`, `mapSharedMemory`, `unmapSharedMemory`, `freeSharedMemory`, `hasSharedMemory` and `getSharedMemorySize`. Note that developers are responsible for freeing the shared memory after use. Please see the function documentation for more information

## C++ memory management APIs
For the most sophisticated package developers, it might be more comfortable to use the C++ APIs rather than the R APIs. All the R functions in `SharedObject` are based upon its C++ APIs. Here is the instruction on show how to use the `SharedObject` C++ APIs in your package. 

### Step 1
For using the C++ APIS, you must add `SharedObject` to the LinkingTo field of the DESCRIPTION file, e.g.,
```
LinkingTo: SharedObject
```
### Step 2
In C++ files, including the header of the shared object `#include "SharedObject/sharedMemory.h"`.

### Step 3
To compile and link your package successfully against the `SharedObject` C++ library, you must include a src/Makevars file.
```
SHARED_OBJECT_LIBS = $(shell echo 'SharedObject:::pkgconfig("PKG_LIBS")'|\
"${R_HOME}/bin/R" --vanilla --slave)
SHARED_OBJECT_CPPFLAGS = $(shell echo 'SharedObject:::pkgconfig("PKG_CPPFLAGS")'|\
"${R_HOME}/bin/R" --vanilla --slave)

PKG_LIBS := $(PKG_LIBS) $(SHARED_OBJECT_LIBS)
PKG_CPPFLAGS := $(PKG_CPPFLAGS) $(SHARED_OBJECT_CPPFLAGS)
```
Note that `$(shell ...)` is GNU make syntax so you should add GNU make to the SystemRequirements field of the DESCRIPTION file of your package, e.g.,
```
SystemRequirements: GNU make
```

You can find the documentation of the C++ functions in the header file.

# Session Information

```r
sessionInfo()
#> R Under development (unstable) (2020-09-03 r79126)
#> Platform: x86_64-w64-mingw32/x64 (64-bit)
#> Running under: Windows 10 x64 (build 19041)
#> 
#> Matrix products: default
#> 
#> locale:
#> [1] LC_COLLATE=English_United States.1252  LC_CTYPE=English_United States.1252   
#> [3] LC_MONETARY=English_United States.1252 LC_NUMERIC=C                          
#> [5] LC_TIME=English_United States.1252    
#> 
#> attached base packages:
#> [1] parallel  stats     graphics  grDevices utils     datasets  methods   base     
#> 
#> other attached packages:
#> [1] SharedObject_1.3.22
#> 
#> loaded via a namespace (and not attached):
#>  [1] Rcpp_1.0.5          digest_0.6.25       magrittr_1.5        evaluate_0.14       rlang_0.4.7        
#>  [6] stringi_1.4.6       rmarkdown_2.3       BiocStyle_2.17.0    tools_4.1.0         stringr_1.4.0      
#> [11] xfun_0.16           yaml_2.2.1          compiler_4.1.0      BiocGenerics_0.35.4 BiocManager_1.30.10
#> [16] htmltools_0.5.0     knitr_1.29
```




