context("Share basic types")
sharedObjectPkgOptions(minLength = 1)

test_that("basic types",{
    #define NILSXP	     0	  /* nil = NULL */
    value <- NULL
    x <- share(value)
    expect_false(is.shared(x))
    expect_identical(x, value)

    #define SYMSXP	     1	  /* symbols */
    value <- as.symbol("a")
    x <- share(value)
    expect_false(is.shared(x))
    expect_identical(x, value)

    #define LISTSXP	     2	  /* lists of dotted pairs */
    listData <- list(a=runif(10), b=letters[1:10])
    value <- as.pairlist(listData)
    x <- share(value)
    expect_equal(is.shared(x,depth = 1), list(a=TRUE,b=TRUE))
    expect_identical(x, value)

    #define CLOSXP	     3	  /* closures */
    value <- function(x)x
    x <- share(value)
    expect_false(is.shared(x))
    expect_identical(x, value)

    #define ENVSXP	     4	  /* environments */
    value <- as.environment(listData)
    x <- share(value)
    expect_true(is.shared(x))
    expect_identical(x, value)

    #define LANGSXP	     6	  /* language constructs (special lists) */
    value <- call("sin", pi)
    x <- share(value)
    expect_false(is.shared(x))
    expect_identical(x, value)

    #define LGLSXP	    10	  /* logical vectors */
    value <- c(TRUE,FALSE)
    x <- share(value)
    expect_true(is.shared(x))
    expect_identical(x, value)

    #define INTSXP	    13	  /* integer vectors */
    value <- 1L
    x <- share(value)
    expect_true(is.shared(x))
    expect_identical(x, value)

    #define REALSXP	    14	  /* real variables */
    value <- 1.0
    x <- share(value)
    expect_true(is.shared(x))
    expect_identical(x, value)

    #define CPLXSXP	    15	  /* complex variables */
    value <- complex(1)
    x <- share(value)
    expect_true(is.shared(x))
    expect_identical(x, value)

    #define STRSXP	    16	  /* string vectors */
    value <- "a"
    x <- share(value)
    expect_true(is.shared(x))
    expect_identical(x, value)

    #define VECSXP	    19	  /* generic vectors */
    value <- listData
    x <- share(value)
    expect_equal(is.shared(x,depth = 1), list(a=TRUE,b=TRUE))
    expect_identical(x, value)

    #define EXPRSXP	    20	  /* expressions vectors */
    value <- expression(1 + 0:9)
    x <- share(value)
    expect_false(is.shared(x))
    expect_identical(x, value)

    #define RAWSXP      24    /* raw bytes */
    value <- as.raw(1:10)
    x <- share(value)
    expect_true(is.shared(x))
    expect_identical(x, value)

    ## These types will not be supported/tested
    #define PROMSXP	     5	  /* promises: [un]evaluated closure arguments */
    #define SPECIALSXP   7	  /* special forms */
    #define BUILTINSXP   8	  /* builtin non-special forms */
    #define CHARSXP	     9	  /* "scalar" string type (internal only)*/
    #define DOTSXP	    17	  /* dot-dot-dot object */
    #define ANYSXP	    18	  /* make "any" args work.*/
    #define BCODESXP    21    /* byte code */
    #define EXTPTRSXP   22    /* external pointer */
    #define WEAKREFSXP  23    /* weak reference */
    #define S4SXP       25    /* S4, non-vector */
    #define NEWSXP      30    /* fresh node created in new page */
    #define FREESXP     31    /* node released by GC */
    #define FUNSXP      99    /* Closure or Builtin or Special */
})

gc()
