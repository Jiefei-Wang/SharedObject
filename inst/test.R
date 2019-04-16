y=10

makeActiveBinding("x",function(){
  rm(x, envir =parent.frame() )
  assign("x",y,envir = parent.frame())
},env=environment()
)


x=matrix(1:10)
x=data.frame(a=10)


devtools::load_all(".")
x=1:10
.Call(C_createSharedMemory,x,40.0,"test")
p=.Call(C_readSharedMemory,"test")
set_xptr_tag(p,x[1])
.Call(C_getValue_32,p,as.integer(c(1,2,3)))


x
