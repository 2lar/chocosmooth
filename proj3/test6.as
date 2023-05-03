        lw 0 1 arr
        add 1 1 1
        lw 0 2 arr2
        nor 1 2 3
        sw 3 0 arr
        lw 0 4 arr2
        sw 4 0 arr
        noop
        halt
arr     .fill 0
arr2    .fill 1
