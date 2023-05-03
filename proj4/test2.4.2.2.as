        lw 0 1 arr
        add 1 1 1
        lw 0 2 const
        nor 1 2 3
        sw 3 0 arr
        beq 1 2 label
        noop
        .fill 5
label   lw 0 1 arr
        sw 1 0 arr
        halt
arr     .fill 2
const   .fill 5