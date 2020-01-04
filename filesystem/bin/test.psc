float x
float y

push $20
set x

push $35
set y

equal:
	print "X is equal to Y!"
	end

greater:
	print "X is greater than Y!"
	end

less:
	print "X is less than Y!"
	end

loop:
	push x
	push y
	comp

	ce equal
	cgt greater
	clt less

	push $1
	add x

	end

push x
push $50
comp
glt loop

print "Done!"
