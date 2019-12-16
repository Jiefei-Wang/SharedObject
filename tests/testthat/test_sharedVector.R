context("Shared vector validation")
gc()
set.seed(as.numeric(Sys.time()))

library(parallel)
cl = makeCluster(1)

n = 100
data = floor(abs(runif(n) * 2))
type = c(
    logical = as.logical,
    integer = as.integer,
    double = as.double,
    raw = as.raw
    #character=as.character
)
typeName = names(type)

for (i in seq_along(typeName)) {
    test_that(paste0("Testing basic subset for type ", typeName[i]), {
        curData = type[[i]](data)
        sv = share(curData)
        expect_equal(curData, sv)
        expect_equal(is.shared(sv),TRUE)
        expect_equal(is.shared(sv[1:2]),as.logical(getSharedObjectOptions("sharedSubset")))
    })

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

    test_that(paste0("Testing cluster export for type ", typeName[i]), {
        curData = type[[i]](data)
        sv = share(curData)
        clusterExport(cl, "sv", envir = environment())
        res = clusterEvalQ(cl, {
            sv
        })
        expect_equal(curData, res[[1]])
    })


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
            res = clusterEvalQ(cl, {
                sv[1] = as(1 - sv[1], typeof(sv))
                sv
            })
            expect_equal(0 + sv[1], 1 - data[1])
        }
    })
}


test_that("data frame", {
    newData = as.data.frame(matrix(data, n / 10, 10))
    x = share(newData)
    expect_equal(x, newData)
})


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




stopCluster(cl)
gc()
