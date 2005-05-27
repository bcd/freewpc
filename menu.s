
.area ram

current_menu:		.blkw 1

current_item:		.blkw 1


.area sysrom

main_menu_title:
	.asciz	"MAIN MENU"
main_menu:
	.dw	main_menu_title	
	.dw	

proc(menu_init)
endp

