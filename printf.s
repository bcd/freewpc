
.area ram

;;; The low-level function to be used for printf()
printf_driver::   .blkw 1

;;; The destination address of the sprintf() call
sprintf_dest::    .blkw 1


.area sysrom


proc(serial_putchar)
endp


proc(sprintf_putchar)
   uses(x)
   ldx   sprintf_dest
   sta   ,x+
   stx   sprintf_dest
endp


proc(ll_printf_value)
endp


proc(ll_printf_error)
endp


proc(ll_printf) ; X = pointer to format string
   uses(a,x,y)
   ldy   printf_driver

   lda   ,x+
   switch(a)
      case('%')
         lda   ,x+
         switch(a)
            case('%')
               jsr   ,y
            endcase

            case(0)
               jsr   ll_printf_error
            endcase

            case('a')
               clra
               ldb   saved(a)
               bra   ll_printf_format_value
            endcase

            case('b')
               clra
               ldb   saved(b)
               bra   ll_printf_format_value
            endcase

            case('x')
               ldd   saved(x)

ll_printf_format_value:
               jsr   ll_printf_value
            endcase

         endswitch
      endcase

      case(0)
         return
      endcase

      default
         jsr   ,y
   endswitch
endp


proc(sprintf) ; Y = dest buffer, X = pointer to format string
   uses(y)
   sty   sprintf_dest
   ldy   #sprintf_putchar
   sty   printf_driver
   jmp   ll_printf
endp


proc(printf) ; X = pointer to format string
   uses(y)
   ldy   #serial_putchar
   sty   printf_driver
   jmp   ll_printf
endp

