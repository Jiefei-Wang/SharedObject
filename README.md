# Note for developers
We plan to make a major update in Bioc 3.13, it includes performance boost and more general C++ level API design. The update should only have minor affect on users, but if you plan to develop a package based upon `SharedObject`, it is recommended to use the package in Bioc 3.13 for the old version will not be supported in the next release.

# Introduction
The `SharedObject` package is designed for sharing data across multiple R processes, where all processes can read the data located in the same memory location. This sharing mechanism has the potential to save the memory usage and reduce the overhead of data transmission in parallel computing. The use of the package arises from many data-science subjects such as high-throughput gene data analysis, in which case the data is very large and a parallel computing is desirable. Blindly exporting data to all R processes via functions such as `clusterExport` will duplicate the data for each process and it is obviously unnecessary if other processes just need to read the data. The `sharedObject` package can share the data without duplications and is able to reduce the time cost. A new set of R APIs called `ALTREP` is used to provide a seamless experience when sharing an object.

# Quick example
We first demonstrate the package with an example. In this example, we create a cluster with 4 cores and share an n-by-n matrix `A`, we use the function `share` to create a shared object `shared_A` and call the function `clusterExport` to export it:


```r
library(parallel)
## Initiate the cluster
cl <- makeCluster(1)
## create data
n <- 3
A <- matrix(runif(n^2), n, n)
## create a shared object
shared_A <- share(A)
#> allocating shared memory, key:Local\SO_X64_6, size:72
#> mapping shared memory, key:Local\SO_X64_6
## export the shared object
clusterExport(cl,"shared_A")

stopCluster(cl)
```
As the code shows above, the procedure of exporting a shared object to the other R processes is similar to the procedure of exporting a regular R object, except that we replace the matrix `A` with a shared object `shared_A`. Notably, there is no different between the matrix `A` and the shared object `shared_A`. The shared object `shared_A` is neither an S3 nor S4 object and it behaves exactly the same as the matrix `A`, so there is no need to change the existing code to work with the shared object. We can verify this through

```r
## check the data
A
#>           [,1]      [,2]      [,3]
#> [1,] 0.6771113 0.2224684 0.3200601
#> [2,] 0.8706540 0.2970336 0.9447201
#> [3,] 0.5249925 0.4580965 0.0557467
shared_A
#>           [,1]      [,2]      [,3]
#> [1,] 0.6771113 0.2224684 0.3200601
#> [2,] 0.8706540 0.2970336 0.9447201
#> [3,] 0.5249925 0.4580965 0.0557467
## check the class
class(A)
#> [1] "matrix" "array"
class(shared_A)
#> [1] "matrix" "array"
## idential
identical(A, shared_A)
#> [1] TRUE
```
Users can treate the shared object `shared_A` as a matrix and do operations on it as usual. For reducing the unnecessary creation of a shared object, the subset of a shared object is a regular R object. Users can verify this by calling `is.shared`

```r
## `shared_A` is a shared object
is.shared(shared_A)
#> [1] TRUE

## The subset of `shared_A` is not
is.shared(shared_A[1:2])
#> [1] FALSE
```
This behavior, however, can be altered via the argument `sharedSubset`. Therefore, if a shared object `shared_A` is made by `share(A, sharedSubset = TRUE)`, then all the subsets of the object `shared_A` will be shared objects automatically. 

# Supported data types and structures
For the basic R types, the package supports `raw`, `logical`, `integer`, `double` and `complex.` character cannot be shared for it closely relates to R's cache. For the containers, the package supports `list`, `pairlist` and `environment.` 

The support for the `S3` and `S4` system is available out-of-box. Therefore, there is no additional action required to share an S3/S4 object. However, if the S3/S4 class has a special design(e.g. on-disk data), the function `share` is an S4 generic, developers can define an S4 `share` method to support their own data structures.

Please note that sharing a container(e.g. `list`) will not share the container itself, but share each element in the container. Therefore, adding or replace an element in a shared list in one process will not implicitly change the shared list in the other processes. Since a data frame is fundamentally a list object, sharing a data frame will follow the same principle as sharing a list. 

When an object is not sharable, no error will be given and the same object will be returned. The argument `mustWork = TRUE` can be used if you want to make sure the return value is a shared object.

```r
## the element `a` is sharable and `b` is not
x <- list(a = 1:3, b = letters[1:3])

## No error will be given, 
## but the element `b` is not shared
shared_x <- share(x)
#> allocating shared memory, key:Local\SO_X64_7, size:12
#> mapping shared memory, key:Local\SO_X64_7
#> allocating shared memory, key:Local\SO_X64_8, size:3
#> mapping shared memory, key:Local\SO_X64_8
is.shared(shared_x, depth = 1)
#> $a
#> [1] TRUE
#> 
#> $b
#> [1] TRUE

## Use the `mustWork` argument
## An error will be given for the non-sharable object `b`
tryCatch({
  shared_x <- share(x, mustWork = TRUE)
},
error=function(msg)print(msg)
)
#> allocating shared memory, key:Local\SO_X64_9, size:12
#> mapping shared memory, key:Local\SO_X64_9
#> allocating shared memory, key:Local\SO_X64_10, size:3
#> mapping shared memory, key:Local\SO_X64_10
```

# Check object information
In order to distinguish a shared object, the package provide `is.shared` function to identify a shared object


```r
## Check if an object is a shared object
## This works for both vector and data.frame
is.shared(A)
#> [1] FALSE
is.shared(shared_A)
#> [1] TRUE
```
By default, `is.shared` function returns a single logical value indicating whether the object is a shared object or contains any shared objects. If the object is a container(e.g. list), you can see the details by increasing the value of the `depth` argument.

```r
## A single logical is returned
is.shared(shared_x)
#> [1] TRUE
## Check each element in x
is.shared(shared_x, depth = 1)
#> $a
#> [1] TRUE
#> 
#> $b
#> [1] TRUE
```

There are several properties associated with the shared object, one can check them via

```r
## get a summary report
getSharedObjectProperty(shared_A)
#> Error in getSharedObjectProperty(shared_A): could not find function "getSharedObjectProperty"

## get the individual properties
getCopyOnWrite(shared_A)
#> [1] TRUE
getSharedSubset(shared_A)
#> [1] FALSE
getSharedCopy(shared_A)
#> [1] FALSE
```
Please see the advanced topic to see the meaning of the properties and how to set them in a proper way.

# Global options
There are some options that can control the default behavior of a shared object, you can view them via

```r
getSharedObjectOptions()
#> Error in getSharedObjectOptions(): could not find function "getSharedObjectOptions"
```
As beforementioned, the option `sharedSubset` controls whether the subset of a shared object is still a shared object. The option `mustWork` suppress the error message when the function `share` encounter a non-sharable object and force the function to return the same object. We will talk about the options `copyOnWrite` and `sharedCopy` in the advanced section, but for most users these two options should not be changed. The global setting can be modified via `setSharedObjectOptions`

```r
## change the default setting
setSharedObjectOptions(mustWork = TRUE)
#> Error in setSharedObjectOptions(mustWork = TRUE): could not find function "setSharedObjectOptions"
## Check if the change is made
getSharedObjectOptions("mustWork")
#> Error in getSharedObjectOptions("mustWork"): could not find function "getSharedObjectOptions"
```

# Advanced topic
## Copy-On-Write
Because all cores are using the shared object `shared_A` located in the same memory location, a reckless change made on the matrix `shared_A` on one process will immediately be broadcasted to the other process. To prevent users from changing the values of a shared object without awareness, a shared object will duplicate itself if a change of its value is made. Therefore, the code like


```r
shared_A2 <- shared_A
shared_A[1,1] <- 10

shared_A
#>            [,1]      [,2]      [,3]
#> [1,] 10.0000000 0.2224684 0.3200601
#> [2,]  0.8706540 0.2970336 0.9447201
#> [3,]  0.5249925 0.4580965 0.0557467
shared_A2
#>           [,1]      [,2]      [,3]
#> [1,] 0.6771113 0.2224684 0.3200601
#> [2,] 0.8706540 0.2970336 0.9447201
#> [3,] 0.5249925 0.4580965 0.0557467

## shared_A became a regular R object
is.shared(shared_A)
#> [1] FALSE
```
will result in a memory dulplication. The matrix `shared_A2` is not affected. This default behavior can be overwritten by passing an argument `copyOnWrite` to the function `share`. For example


```r
shared_A <- share(A, copyOnWrite=FALSE)
#> allocating shared memory, key:Local\SO_X64_11, size:72
#> mapping shared memory, key:Local\SO_X64_11
shared_A2 <- shared_A
shared_A[1,1] <- 10
#> mapping shared memory, key:Local\SO_X64_11

shared_A
#>            [,1]      [,2]      [,3]
#> [1,] 10.0000000 0.2224684 0.3200601
#> [2,]  0.8706540 0.2970336 0.9447201
#> [3,]  0.5249925 0.4580965 0.0557467
shared_A2
#>            [,1]      [,2]      [,3]
#> [1,] 10.0000000 0.2224684 0.3200601
#> [2,]  0.8706540 0.2970336 0.9447201
#> [3,]  0.5249925 0.4580965 0.0557467
```
A change in the matrix `shared_A` cause a change in `shared_A2`. This feature could be potentially useful to return the result from each R process without additional memory allocation, so `shared_A` can be both the initial data and the final result. However, due to the limitation of R, it is possible to change the value of a shared object unexpectly. For example


```r
shared_A <- share(A, copyOnWrite = FALSE)
#> allocating shared memory, key:Local\SO_X64_12, size:72
#> mapping shared memory, key:Local\SO_X64_12
shared_A
#>           [,1]      [,2]      [,3]
#> [1,] 0.6771113 0.2224684 0.3200601
#> [2,] 0.8706540 0.2970336 0.9447201
#> [3,] 0.5249925 0.4580965 0.0557467
-shared_A
#> mapping shared memory, key:Local\SO_X64_12
#>            [,1]       [,2]       [,3]
#> [1,] -0.6771113 -0.2224684 -0.3200601
#> [2,] -0.8706540 -0.2970336 -0.9447201
#> [3,] -0.5249925 -0.4580965 -0.0557467
shared_A
#>            [,1]       [,2]       [,3]
#> [1,] -0.6771113 -0.2224684 -0.3200601
#> [2,] -0.8706540 -0.2970336 -0.9447201
#> [3,] -0.5249925 -0.4580965 -0.0557467
```
The above example shows an unexpected result when the copy-on-write feature is off. Simply calling an unary function can change the values of a shared object. Therefore, for the safty of the naive users, it is recommended to use the default setting. For the sophisticated R users, the copy-on-write feature of an object can be altered via `setCopyOnwrite` funtion. The old value will be invisibly returned by the function.


```r
shared_A <- share(A, copyOnWrite = FALSE)
#> allocating shared memory, key:Local\SO_X64_13, size:72
#> mapping shared memory, key:Local\SO_X64_13
shared_A2 <- shared_A
## change the value of shared_A
shared_A[1,1] <- 10
#> mapping shared memory, key:Local\SO_X64_13
## Both shared_A and shared_A2 are affected
shared_A
#>            [,1]      [,2]      [,3]
#> [1,] 10.0000000 0.2224684 0.3200601
#> [2,]  0.8706540 0.2970336 0.9447201
#> [3,]  0.5249925 0.4580965 0.0557467
shared_A2
#>            [,1]      [,2]      [,3]
#> [1,] 10.0000000 0.2224684 0.3200601
#> [2,]  0.8706540 0.2970336 0.9447201
#> [3,]  0.5249925 0.4580965 0.0557467

## Enable copy-on-write
setCopyOnWrite(shared_A, TRUE)
## shared_A is now independent with shared_A2
shared_A[1,2] <- 10
shared_A
#>            [,1]       [,2]      [,3]
#> [1,] 10.0000000 10.0000000 0.3200601
#> [2,]  0.8706540  0.2970336 0.9447201
#> [3,]  0.5249925  0.4580965 0.0557467
shared_A2
#>            [,1]      [,2]      [,3]
#> [1,] 10.0000000 0.2224684 0.3200601
#> [2,]  0.8706540 0.2970336 0.9447201
#> [3,]  0.5249925 0.4580965 0.0557467
```
These flexibilities provide us a way to do safe operations during the computation and return the results without memory duplications.

### Warning
If a high-precision value is assigned to a low-precision shared object(E.g. assigning a numeric value to an integer shared object), an implicit type conversion will be triggered for correctly storing the change. The resulting object would be a regular R object, not a shared object. Therefore, the change will not be broadcasted even if the copy-on-write feature is off. Users should be caution with the data type that a shared object is using.

## Shared subset and shared copy
The options `sharedSubset` controls whether to create a shared object when subsetting a shared object. `sharedCopy` determines if the duplication of a shared object is still a shared object(Recall that after changing the value of the variable `shared_A`, it is no longer a shared object, this is due to `sharedCopy = FALSE`). For performance consideration, the default settings are `sharedSubset =  FALSE` and `sharedCopy=FALSE`, but they can be temporary overwritten via:

```r
shared_A <- share(A, sharedSubset = TRUE, sharedCopy = TRUE)
#> allocating shared memory, key:Local\SO_X64_14, size:72
#> mapping shared memory, key:Local\SO_X64_14
getSharedObjectProperty(shared_A, property = c("sharedSubset", "sharedCopy"))
#> Error in getSharedObjectProperty(shared_A, property = c("sharedSubset", : could not find function "getSharedObjectProperty"

## Changing the value of `shared_A` will not 
## result in a regular R object
shared_A2 <- shared_A
shared_A[1,1] <- 10
#> allocating shared memory, key:Local\SO_X64_15, size:72
#> mapping shared memory, key:Local\SO_X64_15
is.shared(shared_A)
#> [1] TRUE
```
Please note that `sharedCopy` is only available when `copyOnWrite = TRUE`.

# Developing package based upon SharedObject
The package offers three levels of APIs to help the package developers to build their own shared memory object. 

## user API
The simplest and recommended way to make your own shared object is to define an S4 function `share` in your own package, where you can rely on the existing `share` functions to quickly add the support of an S4 class which is not provided by `SharedObject`. We recommend to use this method to build your package for the simple reason that the developers do not have to control the lifecycle of the shared memory. The package will automatically destroy the shared memory after usage.

## R memory management APIs
It is a common request to have a low level control to the shared memory. To achieve that, the package exports some R APIs for the developers who want to have a fine control of their shared objects. These functions are `allocateSharedMemory`, `allocateNamedSharedMemory`, `mapSharedMemory`, `unmapSharedMemory`, `freeSharedMemory`, `hasSharedMemory` and `getSharedMemorySize`. Note that developers are responsible for destroying the shared memory after usage. Please see the function documentation for more infomation

## C++ memory management APIs
For the most sophisticated package developers, it might be more comfortable to use the C++ APIs rather than R APIs. All the R APIs mentioned in the previous section can be found at C++ level. Here I will borrow the instruction from `Rhtslib` package with slight changes to show how to use the C++ APIs in your package. 

### Step 1
For using the C++ APIS, you must add `SharedObject` to the LinkingTo field of the DESCRIPTION file, e.g.,
```
LinkingTo: SharedObject
```
### Step 2
In C++ code files, include the header of the shared object `#include "SharedObject/sharedMemory.h"`.

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
You can find short descriptions on how to use the C++ API in the header file.

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
#> [1] SharedObject_1.3.21 testthat_2.3.2     
#> 
#> loaded via a namespace (and not attached):
#>  [1] xfun_0.16           remotes_2.2.0       purrr_0.3.4         usethis_1.6.1      
#>  [5] htmltools_0.5.0     yaml_2.2.1          rlang_0.4.7         pkgbuild_1.1.0     
#>  [9] glue_1.4.2          withr_2.2.0         BiocGenerics_0.35.4 sessioninfo_1.1.1  
#> [13] stringr_1.4.0       commonmark_1.7      devtools_2.3.1      evaluate_0.14      
#> [17] memoise_1.1.0       knitr_1.29          callr_3.4.3         ps_1.3.4           
#> [21] fansi_0.4.1         Rcpp_1.0.5          backports_1.1.9     desc_1.2.0         
#> [25] pkgload_1.1.0       fs_1.5.0            digest_0.6.25       stringi_1.4.6      
#> [29] processx_3.4.4      rprojroot_1.3-2     BH_1.72.0-3         cli_2.0.2          
#> [33] tools_4.1.0         magrittr_1.5        crayon_1.3.4        ellipsis_0.3.1     
#> [37] xml2_1.3.2          prettyunits_1.1.1   assertthat_0.2.1    rmarkdown_2.3      
#> [41] roxygen2_7.1.1      rstudioapi_0.11     R6_2.4.1            compiler_4.1.0
```




