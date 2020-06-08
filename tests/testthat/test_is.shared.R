context("is.shared function")



test_that("is.shared atomic", {
    x <- structure(runif(10), a = 1:10, b = letters[1:5], d=list(a1= 1,b1=letters[1:5]))
    x1 <- share(x)
    expect_equal(is.shared(x1),TRUE)
    expect_equal(is.shared(x1,recursive = TRUE),TRUE)
    expect_equal(is.shared(x1,showAttributes = TRUE),
                 structure(TRUE, sharedAttributes = list(a=TRUE,b=FALSE,d=TRUE)))
    expect_equal(is.shared(x1,recursive = TRUE,showAttributes = TRUE),
                 is.shared(x1,showAttributes = TRUE))
})

test_that("is.shared list", {
    x <- list(a = 1:10, b = letters[1:5],d=list(a1= 1,b1=letters[1:5]))
    x1 <- tryShare(x)
    expect_equal(is.shared(x1),list(a=TRUE,b=FALSE,d=TRUE))
    expect_equal(is.shared(x1,recursive = TRUE),
                 list(a=TRUE,b=FALSE,d=list(a1=TRUE,b1=FALSE)))
    expect_equal(is.shared(x1,showAttributes = TRUE),
                 structure(list(a=TRUE,b=FALSE,d=TRUE),
                           sharedAttributes = list(names = FALSE)))
    expect_equal(is.shared(x1,recursive = TRUE,showAttributes = TRUE),
                 structure(list(a=TRUE,b=FALSE,d=list(a1=TRUE,b1=FALSE)),
                           sharedAttributes = list(names = FALSE)))
})



test_that("is.shared S4", {
    .myClass <- setClass("myClass", slots = c(a= "numeric",b="character", d= "list"),
                         contains = "numeric")
    x <- .myClass(runif(10), a = 1:10, b = letters[1:5], d = list(a1= 1,b1=letters[1:5]))
    x1 <- share(x)
    expect_equal(is.shared(x1), list(.Data= TRUE, a= TRUE, b = FALSE, d= TRUE))
    expect_equal(is.shared(x1,recursive = TRUE),
                 list(.Data= TRUE, a= TRUE, b = FALSE, d= list(a1=TRUE,b1=FALSE)))
    expect_equal(is.shared(x1,showAttributes = TRUE),
                 is.shared(x1))
    expect_equal(is.shared(x1,recursive = TRUE, showAttributes=TRUE),
                 list(.Data= TRUE, a= TRUE, b = FALSE, d= list(a1=TRUE,b1=FALSE)))
    removeClass("myClass")
})
