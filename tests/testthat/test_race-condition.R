context("Race condition")
library(parallel)

test_that("sharing objects at the same time", {
    cl <- makeCluster(2)
    clusterEvalQ(cl, {
        library(SharedObject)
    })
    expect_error({
        clusterEvalQ(cl, {
            for(i in 1:100000){
                a <- share(123)
                if(i%%100==0){
                    gc()
                }
            }
        })
    },NA)
    stopCluster(cl)
})
