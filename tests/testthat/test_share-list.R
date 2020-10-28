context("Share list objects")
sharedObjectPkgOptions(minLength = 1)

test_that("list", {
    myData <- list(a = runif(10), b = letters[1:4], d = sample(10,5))
    expect_error({x <- share(myData, mustWork = TRUE)}, NA)
    expect_equal(x, myData)
    expect_equal(is.shared(x), TRUE)
    expect_equal(is.shared(x, depth = 1), list(a=TRUE,b=TRUE,d=TRUE))
})


test_that("data frame", {
    myData <- as.data.frame(matrix(runif(100), 10, 10))
    myData <- cbind(myData,last =letters[seq_len(10)],stringsAsFactors =FALSE)
    expect_error({x <- share(myData, mustWork = TRUE)}, NA)
    expect_equal(x, myData)
    expect_equal(is.shared(x), TRUE)
    expect_equal(as.logical(is.shared(x,depth = 1)), rep(TRUE,11))
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
    expect_equal(as.logical(is.shared(so, depth = 1)), rep(TRUE,2))
    expect_equal(is.altrep(data), FALSE)
    expect_equal(as.logical(is.shared(data, depth = 1)), rep(FALSE,2))
})


gc()
