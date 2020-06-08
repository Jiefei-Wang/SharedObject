context("Unshare function")

#
#
# n <- 100
#
#
# test_that("unshare Atomic", {
#     a <- share(runif(n))
#     b <- unshare(a)
#     expect_equal(a,b)
#     expect_true(all(as.logical(is.shared(a))))
#     expect_false(any(as.logical(is.shared(b))))
# })
#
#
#
# test_that("unshare list", {
#     a <- share(list(runif(n),runif(n)))
#     b <- unshare(a)
#     expect_equal(a,b)
#     expect_true(all(as.logical(is.shared(a))))
#     expect_false(any(as.logical(is.shared(b))))
# })
#
# test_that("unshare class", {
#     setClass("testClass",representation = list(a="vector",b="character"))
#     x <- new("testClass",a=runif(10),b=sample(letters,5))
#
#     x_shr <- share(x,autoS4Conversion=TRUE, mustWork = TRUE)
#
#     y <-unshare(x_shr)
#     expect_equal(x_shr,y)
#     expect_false(any(unlist(is.shared(y))))
#     removeClass("testClass")
# })

