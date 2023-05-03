        lw      0       1       in1     load 1 into reg1
        lw      0       2       in2     load 10 into reg1
        lw      0       3       neg1    load -1 into reg3
        lw      0       4       five    load 5 into reg4
        jalr    7       5
        add     0       1       2
        halt
start   beq     2       1       7       use jalr
        beq     2       4       4       take the offset
        add     2       3       2       decrement reg3
        beq     0       0       start   return to loop
        add     1       1       1       double reg1       
        add     2       4       2       reg10 in reg2
        beq     0       0       start
        jalr    5       7
in1     .fill   1
in2     .fill   2
neg1    .fill   -1
five    .fill   5
