context("Share basic types")

test_that("basic types",{
    listData <- list(a=runif(10), b=letters[1:4])
    #define NILSXP	     0	  /* nil = NULL */
    x <- share(NULL)
    expect_false(is.shared(x))

    #define SYMSXP	     1	  /* symbols */
    x <- share(as.symbol("a"))
    expect_false(is.shared(x))

    #define LISTSXP	     2	  /* lists of dotted pairs */
    x <- share(as.pairlist(listData))
    expect_equal(is.shared(x,depth = 1), list(a=TRUE,b=FALSE))

    #define CLOSXP	     3	  /* closures */
    x <- share(function(x)x)
    expect_false(is.shared(x))

    #define ENVSXP	     4	  /* environments */
    x <- share(as.environment(listData))
    expect_equal(is.shared(x), TRUE)

    #define LANGSXP	     6	  /* language constructs (special lists) */
    x <- share(call("sin", pi))
    expect_false(is.shared(x))

    #define LGLSXP	    10	  /* logical vectors */
    x <- share(c(TRUE,FALSE))
    expect_true(is.shared(x))

    #define INTSXP	    13	  /* integer vectors */
    x <- share(1L)
    expect_true(is.shared(x))

    #define REALSXP	    14	  /* real variables */
    x <- share(1.0)
    expect_true(is.shared(x))

    #define CPLXSXP	    15	  /* complex variables */
    x <- share(complex(1))
    expect_true(is.shared(x))

    #define STRSXP	    16	  /* string vectors */
    x <- share("a")
    expect_false(is.shared(x))

    #define VECSXP	    19	  /* generic vectors */
    x <- share(listData)
    expect_equal(is.shared(x,depth = 1), list(a=TRUE,b=FALSE))

    #define EXPRSXP	    20	  /* expressions vectors */
    x <- share(expression(1 + 0:9))
    expect_false(is.shared(x))

    #define RAWSXP      24    /* raw bytes */
    x <- share(as.raw(1:10))
    expect_true(is.shared(x))

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

