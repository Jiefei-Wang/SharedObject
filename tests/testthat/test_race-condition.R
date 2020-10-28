context("Race condition")
library(parallel)

test_that("sharing objects at the same time", {
    cl <- makeCluster(2)
    clusterEvalQ(cl, {
        library(SharedObject)
        sharedObjectPkgOptions(minLength = 1)
    })
    expect_error({
        clusterEvalQ(cl, {
            for(i in 1:10000){
                a <- share(1:10)
            }
        })
    },NA)
    stopCluster(cl)
})
