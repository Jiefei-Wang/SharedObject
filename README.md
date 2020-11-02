# Introduction
`SharedObject` is designed for sharing data across many R workers. It allows multiple workers to read and write the same R object located in the same memory location. This feature is useful in parallel computing when a large R object needs to be read by all R workers. It has the potential to reduce the memory consumption and the overhead of data transmission. 


# Quick example
## Creating a shared object from an existing object
To share an R object, all you need to do is to call the `share` function with the object you want to share. In this example, we will create a 3-by-3 matrix `A1` and use the function `share` to make a shared object `A2`

```r
## Create data
A1 <- matrix(1:9, 3, 3)
## Create a shared object
A2 <- share(A1)
```
There is no visible difference between the matrix `A1` and the shared matrix `A2`. There is no need to change the existing code to work with the shared object. We can verify this through

```r
## Check the data
A1
#>      [,1] [,2] [,3]
#> [1,]    1    4    7
#> [2,]    2    5    8
#> [3,]    3    6    9
A2
#>      [,1] [,2] [,3]
#> [1,]    1    4    7
#> [2,]    2    5    8
#> [3,]    3    6    9

## Check if they are identical
identical(A1, A2)
#> [1] TRUE
```
Users can treat the shared object `A2` as a regular matrix and do operations on it as usual. The function `is.shared` can be used to check whether an object is shared.

```r
## Check if an object is shared
is.shared(A1)
#> [1] FALSE
is.shared(A2)
#> [1] TRUE
```
The object `A2` should work with any parallel package including `BiocParallel`. In this vignette we will simply use the `parallel` package to export the object `A2`.

```r
library(parallel)
## Create a cluster with only 1 worker
cl <- makeCluster(1)
clusterExport(cl, "A2")
## Check if the object is still a shared object
clusterEvalQ(cl, SharedObject::is.shared(A2))
#> [[1]]
#> [1] TRUE
stopCluster(cl)
```
When a shared object is exported to the other R workers, only the data ID along with some basic information of the shared object will be sent to the workers. We can see the exported data from the `serialize` function.

```r
## make a larger vector
x1 <- rep(0, 10000)
x2 <- share(x1)

## This is the actual data that will
## be sent to the other R workers
data1 <-serialize(x1, NULL)
data2 <-serialize(x2, NULL)

## Check the size of the data
length(data1)
#> [1] 80032
length(data2)
#> [1] 391
```
As we see from the example, the size of the shared object `x2` is significantly smaller than the size of the regular R object `x1`. When workers receive the shared object `x2`, they can get the data from the memory using the memory ID. Therefore, there is no memory allocation for the data of `x2` in the workers. 
## Creating a shared object from scratch
Analogy to the `vector` function in R, the shared object can also be made from scratch.

```r
SharedObject(mode = "integer", length = 6)
#> [1] 0 0 0 0 0 0
```
You can attach the attributes to `x` when creating the empty shared object. For example

```r
SharedObject(mode = "integer", length = 6, attrib = list(dim = c(2L, 3L)))
#>      [,1] [,2] [,3]
#> [1,]    0    0    0
#> [2,]    0    0    0
```
Please refer to `?SharedObject` for the details of the function.


## Properties of the shared object
There are several properties associated with the shared object, one can check them via

```r
## get a summary report
sharedObjectProperties(A2)
#> $dataId
#> [1] "28"
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
```
where `dataId` is the memory ID that will be used to find the shared memory, `length` and `totalSize` are pretty self-explained, `dataType` is the type ID of the R object, `ownData` determines whether the shared memory will be released after the shared object is freed in the current process. `copyOnWrite`, `sharedSubset` and `sharedCopy` control the procedures of data writing, subsetting and duplication. please see `Package options` and `Advanced topics` sections to see the meaning of the properties and how to use them properly.

Note that most properties in a shared object are not mutable, only `copyOnWrite`, `sharedSubset` and `sharedCopy` are allowed to be changed. The properties can be viewed by `getCopyOnWrite`, `getSharedSubset` and `getSharedCopy` and set via `setCopyOnWrite`, `setSharedSubset` and `setSharedCopy`.

```r
## get the individual properties
getCopyOnWrite(A2)
#> [1] TRUE
getSharedSubset(A2)
#> [1] FALSE
getSharedCopy(A2)
#> [1] FALSE

## set the individual properties
setCopyOnWrite(A2, FALSE)
setSharedSubset(A2, TRUE)
setSharedCopy(A2, TRUE)

## Check if the change has been made
getCopyOnWrite(A2)
#> [1] FALSE
getSharedSubset(A2)
#> [1] TRUE
getSharedCopy(A2)
#> [1] TRUE
```

# Supported data types and structures
For the basic R type, the package supports `raw`, `logical`, `integer`, `numeric`, `complex` and `character`. Note that sharing a character vector is beneficial only when there are a lot repetitions in the elements of the vector. Due to the complicated structure of the character vector, you are not allowed to set the value of a shared character vector to a value which haven't presented in the vector. Therefore, It is recommended to treat the shared character vector as read-only.

For the container, the package supports `list`, `pairlist` and `environment`. Sharing a container is equivalent to sharing all elements in the container, the container itself will not be shared. Therefore, adding or replacing an element in a shared container in one worker will not implicitly change the shared container in the other workers. Since a data frame is fundamentally a list object, sharing a data frame will follow the same principle. 

For the more complicated data structure like `S3` and `S4` class. They are available out-of-box. Therefore, there is no need to customize the `share` function to support an S3/S4 class. However, if the S3/S4 class has a special design(e.g. on-disk data), the function `share` is an S4 generic and developers are free to define their own `share` method.

When an object is not sharable, no error will be given and the same object will be returned. This should be a rare case as most data types are supported. The argument `mustWork = TRUE` can be used if you want to make sure the return value is a shared object.

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

# Package options
There are some options that can control the default behavior of a shared object, you can view them via

```r
sharedObjectPkgOptions()
#> $mustWork
#> [1] FALSE
#> 
#> $sharedAttributes
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
#> 
#> $minLength
#> [1] 3
```
As we have seen previously, the option `mustWork = FALSE` suppress the error message when the function `share` encounter a non-sharable object and force the function to return the same object. `sharedSubset` controls whether the subset of a shared object is still a shared object. `minLength` determines the minimum length of a shared object. An R object will not be shared if its length is less than the minimum length.

We will talk about the options `copyOnWrite` and `sharedCopy` in the advanced section, but for most users it is safe to ignore them. The global setting can be modified via `sharedObjectPkgOptions`

```r
## change the default setting
sharedObjectPkgOptions(mustWork = TRUE)

## Check if the change is made
sharedObjectPkgOptions("mustWork")
#> [1] TRUE

## Restore the default
sharedObjectPkgOptions(mustWork = FALSE)
```
Note that the package options can be temporary overwritten by providing named parameters to the function `share`. For example, you can overwrite the package `mustwork` via `share(x, mustWork = TRUE)`.

# Advanced topics
## Copy-On-Write
Since all workers are using shared objects located in the same memory location, a change made on a shared object in one worker can affect the value of the object in the other workers. To prevent users from changing the values of a shared object unintentionally, a shared object will duplicate itself if a change of its value is made. For example

```r
x1 <- share(1:4)
x2 <- x1

## x2 becames a regular R object after the change
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
When we change the value of `x2`, R will first duplicate the object `x2`, then applies the change. Therefore, although `x1` and `x2` share the same data, the change in `x2` will not affect the value of `x1`. This default behavior can be overwritten by the parameter `copyOnWrite`.

```r
x1 <- share(1:4, copyOnWrite = FALSE)
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
If copy-on-write is off, a change in the matrix `x2` causes a change in `x1`. This feature could be potentially useful to collect the results from workers. For example, you can pre-allocate an empty shared object with `copyOnWrite = FALSE` and let the workers write their results back to the shared object. This will avoid the need of sending the data from workers to the main process. However, due to the limitation of R, it is possible to change the value of a shared object unexpectedly. For example

```r
x <- share(1:4, copyOnWrite = FALSE)
x
#> [1] 1 2 3 4
-x
#> [1] -1 -2 -3 -4
x
#> [1] -1 -2 -3 -4
```
The above example shows a surprising result when the copy-on-write feature is off. Simply calling an unary function can change the values of a shared object. Therefore, users must use this feature with caution. The copy-on-write feature of an object can be set via the `setCopyOnwrite` function or the `copyOnWrite` parameter in the `share` function.


```r
## Create x1 with copy-on-write off
x1 <- share(1:4, copyOnWrite = FALSE)
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
This flexibility provides a way to do safe operations during the computation and return the results without memory duplication.

### Warning
If a high-precision value is assigned to a low-precision shared object(E.g. assigning a numeric value to an integer shared object), an implicit type conversion will be triggered for correctly storing the change. The resulting object would be a regular R object, not a shared object. Therefore, the change will not be broadcasted even if the copy-on-write feature is off. Users should be cautious with the data type that a shared object is using.

## Shared copy
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

## Listing the shared object 
You can list the ID of the shared object you have created via

```r
listSharedObjects()
#>    Id  size
#> 1  28  4096
#> 2  29 81920
#> 3  32  4096
#> 4  34  4096
#> 5  35  4096
#> 6  36  4096
#> 7  37  4096
#> 8  38  4096
#> 9  39  4096
#> 10 40  4096
```
Getting a list of shared object should have a rare use case, but it can be useful if you have a memory leaking problem and a shared memory can be manually released by `freeSharedMemory(ID)`.

# Developing package based upon SharedObject
The package offers three levels of API to help the package developers to build their own shared object. 

## user API
The simplest and recommended way to make your own shared object is to define an S4 function `share` in your own package, where you can rely on the existing `share` functions to quickly add the support for an S4 class which is not provided by `SharedObject`. We recommend to use this method to build your package for the developers do not have to bother with the memory management. The package will automatically free the shared object after use.

## R's shared memory API
It is a common request to have a low level control to the shared memory. To achieve that, the package exports some low-level R API for the developers who want to have a fine control of their shared objects. These functions are `allocateSharedMemory`, `mapSharedMemory`, `unmapSharedMemory`, `freeSharedMemory`, `hasSharedMemory` and `getSharedMemorySize`. Note that developers are responsible for freeing the shared memory after use. Please see the function documentation for more information

## C++ shared memory API
For the most sophisticated package developers, it might be more comfortable to use the C++ API rather than the R API. All the R functions in `SharedObject` are based upon its C++ API. Here is the instruction on show how to use the `SharedObject` C++ API in your package. 

### Step 1
For using the C++ API, you must add `SharedObject` to the LinkingTo field of the DESCRIPTION file, e.g.,
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
#> [1] SharedObject_1.5.3 testthat_2.3.2    
#> 
#> loaded via a namespace (and not attached):
#>  [1] Rcpp_1.0.5          rstudioapi_0.11     knitr_1.29          magrittr_1.5        BiocGenerics_0.35.4
#>  [6] pkgload_1.1.0       R6_2.4.1            rlang_0.4.7         fansi_0.4.1         stringr_1.4.0      
#> [11] tools_4.1.0         xfun_0.16           cli_2.0.2           withr_2.2.0         htmltools_0.5.0    
#> [16] yaml_2.2.1          assertthat_0.2.1    rprojroot_1.3-2     digest_0.6.25       crayon_1.3.4       
#> [21] BiocManager_1.30.10 glue_1.4.2          evaluate_0.14       rmarkdown_2.3       stringi_1.4.6      
#> [26] compiler_4.1.0      desc_1.2.0          backports_1.1.9     BiocStyle_2.17.0
```




