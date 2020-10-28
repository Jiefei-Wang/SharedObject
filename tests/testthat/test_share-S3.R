context("Share S3 objects")
sharedObjectPkgOptions(minLength = 1)

test_that("S3 atomic object", {
    myData <- runif(10)
    class(myData) <- "testClass"

    x = share(myData)
    expect_equal(x, myData)
    expect_equal(is.shared(x, showAttributes = TRUE),
                 structure(TRUE, classShared = FALSE))

    ## Pretending that a vector is a list
    ## if a user provides this kind of confusing object,
    ## We still need to dispatch the share method to the right
    ## function
    myData1 <- structure(myData,class="list")
    x = share(myData1)
    expect_equal(x, myData1)
    expect_equal(is.shared(x, showAttributes = TRUE),
                 structure(TRUE, classShared = FALSE))
})

test_that("S3 list object", {
    myData <- list(a = runif(10), b = letters[1:4], d = sample(10,5))
    class(myData) <- "testClass"

    expect_error({x = share(myData, mustWork = TRUE)}, NA)
    expect_equal(x, myData)
    expect_equal(is.shared(x, depth = 1), list(a=TRUE,b=TRUE,d=TRUE))

    ## Pretending that a list is a matrix
    ## if a user provides this kind of confusing object,
    ## We still need to dispatch the share method to the right
    ## function
    myData1 <- structure(myData, class="matrix")
    x = share(myData1)
    expect_equal(x, myData1)
    expect_equal(is.shared(x, depth = 1), list(a=TRUE,b=TRUE,d=TRUE))
})

test_that("S3 complicated object", {
    x <- lm(mpg~cyl, mtcars)
    x2 <- share(x)
    expect_equal(
        is.shared(x2,depth = 1, showAttributes = TRUE),
        structure(list(coefficients = TRUE, residuals = TRUE, effects = TRUE,
                       rank = TRUE, fitted.values = TRUE, assign = TRUE, qr = TRUE,
                       df.residual = TRUE, xlevels = FALSE, call = FALSE, terms = FALSE,
                       model = TRUE), namesShared = FALSE, classShared = FALSE))

})
gc()






