loop    lw 0 1 var1
        lw 0 2 var2
        beq 1 2 label
        sw 0 1 var3
        noop
        beq 0 0 loop
        halt
label sw 0 2 var3
        halt
var1   .fill 5
var2   .fill 5
var3   .fill 0
