context("package options")

test_that("Shared Object Global options", {
    old <- getSharedObjectOptions()
    setSharedObjectOptions(
        copyOnWrite = FALSE,
        sharedSubset = FALSE,
        sharedCopy = FALSE
    )

    x = share(runif(10))
    getSharedObjectProperty(x, NULL)
    expect_equal(getCopyOnWrite(x), FALSE)
    expect_equal(getSharedCopy(x), FALSE)
    expect_equal(getSharedSubset(x), FALSE)

    setCopyOnWrite(x, TRUE)
    setSharedCopy(x, FALSE)
    setSharedSubset(x, TRUE)

    do.call(setSharedObjectOptions,old)
})
gc()

test_that("Option: mustWork", {
    old <- getSharedObjectOptions()
    ## Use the default setting, expect error
    setSharedObjectOptions(mustWork = TRUE)
    data <- list(a= 1:10,b="a")
    expect_error(so <- share(data))
    expect_error(so <- tryShare(data), NA)


    ## Temporary overwrite the setting, expect no error
    expect_error(so <- share(data, mustWork = FALSE), NA)
    expect_error(so <- tryShare(data), NA)

    ## Overwrite global setting, expect no error
    setSharedObjectOptions(mustWork = FALSE)
    expect_error(so <- share(data), NA)
    expect_error(so <- tryShare(data), NA)


    ## Temporary overwrite the setting, expect error
    expect_error(so <- share(data, mustWork = TRUE))
    expect_error(so <- tryShare(data), NA)

    do.call(setSharedObjectOptions,old)
})

