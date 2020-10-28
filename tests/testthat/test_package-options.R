context("package options")
sharedObjectPkgOptions(minLength = 1)

test_that("Shared Object Global options", {
    old <- sharedObjectPkgOptions()
    sharedObjectPkgOptions(
        copyOnWrite = FALSE,
        sharedSubset = FALSE,
        sharedCopy = FALSE
    )

    x = share(runif(10))
    # sharedObjectProperties(x)
    expect_equal(getCopyOnWrite(x), FALSE)
    expect_equal(getSharedCopy(x), FALSE)
    expect_equal(getSharedSubset(x), FALSE)

    setCopyOnWrite(x, TRUE)
    setSharedCopy(x, FALSE)
    setSharedSubset(x, TRUE)
    expect_equal(getCopyOnWrite(x), TRUE)
    expect_equal(getSharedCopy(x), FALSE)
    expect_equal(getSharedSubset(x), TRUE)

    do.call(sharedObjectPkgOptions,old)
})
gc()

test_that("Option: mustWork", {
    old <- sharedObjectPkgOptions()
    ## Use the default setting, expect error
    sharedObjectPkgOptions(mustWork = TRUE)
    data <- list(a= 1:10,b=as.symbol("a"))
    expect_error(so <- share(data))
    expect_error(so <- tryShare(data), NA)


    ## Temporary overwrite the setting, expect no error
    expect_error(so <- share(data, mustWork = FALSE), NA)
    expect_error(so <- tryShare(data), NA)

    ## Overwrite global setting, expect no error
    sharedObjectPkgOptions(mustWork = FALSE)
    expect_error(so <- share(data), NA)
    expect_error(so <- tryShare(data), NA)

    ## Temporary overwrite the setting, expect error
    expect_error(so <- share(data, mustWork = TRUE))
    expect_error(so <- tryShare(data), NA)

    do.call(sharedObjectPkgOptions,old)
})

