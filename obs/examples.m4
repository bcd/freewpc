
defstruct(mystruct,byte,b1,word,w,byte,b2)

offsetof(mystruct, b1)
offsetof(mystruct, w)
offsetof(mystruct, b2)
sizeof(mystruct)


defstruct(aggregate,byte,x,mystruct,m,byte,b)
__offsetof_aggregate_x
__offsetof_aggregate_m
__offsetof_aggregate_b
sizeof(aggregate)


typedecl(x,mystruct)
deref(x,b2)

proc(switch_test)
   uses(a,b,x)
   definline(x, a)
   definline(x, a, y)
   local(max)
   lda   #sizeof(byte) * 10
   ldb   max
   cmpregconst(b, 250)
   return
endp


ifgt
   jsr x
   ifz
      jsr y
   endif
endif

setregconst(x, 10)
increg(x)
increg(a)



proc(stack_test)
	uses(a,b,x)
	local(byte,b0)
	endlocal

	lda	saved(a)
	ldb	saved(b)
	ldx	saved(x)
endp

