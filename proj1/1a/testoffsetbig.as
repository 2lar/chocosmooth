        lw      0       1       33000   load register 1 with 33000 (numerical offset too great)
        lw      0       2       second  load register 2 with 2 symbolic
        add     0       2       3       add and store 2 into 3
        add     0       1       2       put reg1 into reg2
        add     0       3       1       put reg3 into reg1
        noop
end     halt
first   .fill   1
second  .fill   2