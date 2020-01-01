float a
push $40
set a

push a
call print

float b
push $40
set b

push b
call print

push $10
add a
push a
call print

push b
call print
