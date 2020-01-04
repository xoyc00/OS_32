p:
	push $256
	push $256
	push "Test"
	makewindow

	push $32
	push 432
	push $32
	wclear

	push $12
	push $12
	push "Hello, Windowed World!"
	push $255
	push $255
	push $255
	wprint

	end

main:
	call p

	end

call main
