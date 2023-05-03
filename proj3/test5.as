        lw 0 1 data
        add 1 1 1
        sw 1 0 result
        lw 0 2 data2
        nor 1 2 3
        sw 3 0 result2
        noop
        halt
data    .fill 4
data2   .fill 6
result  .fill 0
result2 .fill 0
