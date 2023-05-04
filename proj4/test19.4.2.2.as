        lw 0 1 arr
        add 1 1 1
        lw 0 4 const
        nor 1 4 3
        sw 3 0 arr
        beq 1 4 label
        noop
        .fill 7
label   lw 0 1 arr
        sw 1 0 arr
        halt
arr     .fill 4
const   .fill 7
