        lw      0       1       input   load register 1 with input 1 - symbolic
        lw      0       2       aux     load register 2 with aux input (15) - symbolic
loop    beq     0       1       end     make a loop that increases until input reaches aux
        add     1       2       2       add two if still in the loop
        beq     0       0       loop    go back to beginning of loop since 0 == 0
        noop
end     halt                            end of program
input   .fill   1
aux     .fill   15
