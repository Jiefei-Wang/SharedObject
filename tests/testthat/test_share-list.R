context("Share list objects")

test_that("list", {
    myData <- list(a = runif(10), b = letters[1:4], d = sample(10,5))
    expect_error({x = share(myData, mustWork = TRUE)})
    x = share(myData)
    expect_equal(x, myData)
    expect_equal(is.shared(x), TRUE)
    expect_equal(is.shared(x, depth = 1), list(a=TRUE,b=FALSE,d=TRUE))
})


test_that("data frame", {
    myData <- as.data.frame(matrix(runif(100), 10, 10))
    myData <- cbind(myData,last =letters[seq_len(10)],stringsAsFactors =FALSE)
    expect_error({x = share(myData, mustWork = TRUE)})
    x = share(myData)
    expect_equal(x, myData)
    expect_equal(is.shared(x), TRUE)
    expect_equal(as.logical(is.shared(x,depth = 1)), c(rep(TRUE,10),FALSE))
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

test_that("nonsharable list-related objects", {
    x <- list(a = "a")
    x2 <- share(x)
    expect_true(C_isSameObject(x,x2))

    attr(x, "test") = "a"
    attr(x, "test1") = 1
    x2 <- share(x)
    expect_true(C_isSameObject(attr(x, "test"),attr(x2, "test")))
    expect_false(C_isSameObject(attr(x, "test1"),attr(x2, "test1")))
    expect_true(is.shared(attr(x2, "test1")))
})


gc()
