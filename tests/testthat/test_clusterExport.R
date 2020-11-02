context("cluster export")
sharedObjectPkgOptions(minLength = 1)
library(parallel)
cl = makeCluster(1)
clusterEvalQ(cl, {
    library(SharedObject)
})
n = 100
data = floor(abs(runif(n) * 2))
type = c(
    logical = as.logical,
    integer = as.integer,
    double = as.double,
    raw = as.raw
)
typeName = names(type)

for (i in seq_along(typeName)) {
    test_that(paste0("Testing basic subset for type ", typeName[i]), {
        curData = type[[i]](data)
        so = share(curData)
        expect_equal(curData, so)
        expect_equal(is.shared(so),TRUE)
        expect_equal(is.shared(so[1:2]), sharedObjectPkgOptions("sharedSubset"))
    })
    gc()
    test_that(paste0("Testing duplicate for type ", typeName[i]), {
        if (i <= 3) {
            curData = type[[i]](data)
            sv = share(curData, copyOnWrite  = FALSE)
            sv2 = sv
            sv2[1] = type[[i]](1 - sv[1])
            expect_equal(sv, sv2)

            sv = share(curData, copyOnWrite = TRUE)
            sv2 = sv
            sv2[1] = type[[i]](1 - sv2[1])
            expect_false(curData[1] == sv2[1])
        }else{
            expect_true(TRUE)
        }
    })
    gc()
    test_that(paste0("Testing cluster export for type ", typeName[i]), {
        curData = type[[i]](data)
        sv = share(curData)
        clusterExport(cl, "sv", envir = environment())
        res = clusterEvalQ(cl, {
            sv
        })
        expect_equal(curData, res[[1]])
        ## ownership
        res = clusterEvalQ(cl, {
            sharedObjectProperties(sv)$ownData
        })
        expect_equal(res[[1]], FALSE)
    })

    gc()
    test_that(paste0(
        "Testing copy-on-write for cluster export for type ",
        typeName[i]
    ),
    {
        if (i <= 3) {
            curData = type[[i]](data)
            sv = share(curData)
            setCopyOnWrite(sv, FALSE)
            clusterExport(cl, "sv", envir = environment())

            ## CopyOnWrite property
            res <- clusterEvalQ(cl, {
                getCopyOnWrite(sv)
            })
            expect_equal(res[[1]], FALSE)

            ## Test behavior
            res <- clusterEvalQ(cl, {
                sv[1] <- as(1 - sv[1], typeof(sv))
                sv
            })
            expect_equal(0 + sv[1], 1 - data[1])
        }else{
            expect_true(TRUE)
        }
    })
    gc()
}
stopCluster(cl)
gc()
