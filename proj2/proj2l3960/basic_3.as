        lw 0 1 var1
        lw 0 2 var2
        beq 1 2 labe
        sw 0 1 var3
        halt
        lw 1 3 var4
        add 2 2 2
        lw 1 3 3
        nor 3 3 6
        add 6 6 2
labe sw 0 2 var3
        halt
var1   .fill 10
var2   .fill 5
var3   .fill 0
var4   .fill -10