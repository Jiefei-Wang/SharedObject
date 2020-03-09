# SharedObject
## For users
This package is designed for sharing objects across multiple R
processes. Users can call `share` function to create a shared
object. shared objects behave the same as R vectors(AKA atomic),
users should not be able to see any notable changes after converting a
vector into a shared object. Shared objects can be exported through
the export function from a parallel package. There is no memory
dulplication during the variable export. Therefore, all R processes
will share a common data. Changing values of a shared Object is
supported but it should be used with caution for it will force R 
to duplicate the data.

### Example
```
## Create data and the shared object
x <- runif(10)
so <- share(x)

## Compare differences
x
so

## There is no differences between `x` and `so` 
## except that the data of `so` is in the shared memory space.
identical(x, so)

## methods to check if an object is a shared object
is.shared(x)
is.shared(so)

## Examine the internal data structure
.Internal(inspect(x))
.Internal(inspect(so))

## Export the shared object
## Only the infomation of the shared object is exported.
library(parallel)
cl <- makeCluster(1)
clusterExport(cl, "so")
clusterEvalQ(cl, so)
stopCluster(cl)
```

## For package developers
The package provide three levels of APIs to help the package developers to build their own shared memory object without worrying too much about the C++ implementation. Developers are refered to the package vignette to see the details.




