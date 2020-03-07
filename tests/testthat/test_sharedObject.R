context("Shared vector validation")
gc()
## For debugging in the same day....
# set.seed(as.numeric(Sys.Date()))

library(parallel)
cl = makeCluster(1)

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
        expect_equal(is.shared(so[1:2]),as.logical(getSharedObjectOptions("sharedSubset")))
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
            getSharedObjectProperty(sv)$ownData
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
        }
    })
    gc()
}


test_that("data frame", {
    newData <- as.data.frame(matrix(data, n / 10, 10))
    newData <- cbind(newData,last =letters[seq_len(n / 10)],stringsAsFactors =FALSE)
    expect_error({x = share(newData)})
    x = tryShare(newData)
    expect_equal(x, newData)
    expect_equal(as.logical(is.shared(x)), c(rep(TRUE,10),FALSE))
})

gc()
test_that("type check", {
    data = matrix(0, 2, 2)
    so = share(data)
    expect_equal(is.altrep(so), TRUE)
    expect_equal(is.shared(so), TRUE)
    expect_equal(is.altrep(data), FALSE)
    expect_equal(is.shared(data), FALSE)

    data = as.data.frame(data)
    so = share(data)
    expect_equal(is.altrep(so), FALSE)
    expect_equal(as.logical(is.shared(so)), rep(TRUE,2))
    expect_equal(is.altrep(data), FALSE)
    expect_equal(as.logical(is.shared(data)), rep(FALSE,2))
})
gc()

test_that("Shared Object Global options", {
    getSharedObjectOptions()
    getSharedObjectOptions("copyOnWrite")
    setSharedObjectOptions(
        copyOnWrite = FALSE,
        sharedSubset = FALSE,
        sharedCopy = FALSE
    )

    x = share(data)
    getSharedObjectProperty(x, NULL)
    expect_equal(getCopyOnWrite(x), FALSE)
    expect_equal(getSharedCopy(x), FALSE)
    expect_equal(getSharedSubset(x), FALSE)

    setCopyOnWrite(x, TRUE)
    setSharedCopy(x, FALSE)
    setSharedSubset(x, TRUE)
})
gc()



stopCluster(cl)
gc()
