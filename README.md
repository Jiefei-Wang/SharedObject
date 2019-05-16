# SharedObject
This package is designed for sharing object across multiple R processes. Users can call `sharedObject` function to create a shared object. shared objects behaves the same as R vectors(AKA automic), users should not be able to experience any notable changes after converting a vector to a shared object. Shared objects can be exported through `clusterExport` function in `Parallel` package. There is no memory dulplication during the variable export. Therefore, all R processes will share a common data. Changing values of a shared Object is supported but it should be used with caution.

# Example
```
#Create data and the shared object
x=runif(10)
so=sharedObject(x)

#Compare differences
x
so

#There is no differences between x and so except that so is in the shared memory space.
is.vector(x)
is.vector(so)

#methods to check if an object is a shared object
is.altrep(x)
is.vector(so)

.Internal(inspect(x))
.Internal(inspect(so))

#Export the shared object
library(parallel)
cl=makeCluster(1)
clusterExport(cl,"so")
clusterEvalQ(cl,so)
```
