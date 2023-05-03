        lw      0       1       first   load register 1 with 1 symbolic
        lw      0       2       third  load register 2 with 2 symbolic
        add     0       2       3       add and store 2 into 3
        add     0       1       2       put reg1 into reg2
        add     0       3       1       put reg3 into reg1
        noop
end     halt
first   .fill   1
second  .fill   2