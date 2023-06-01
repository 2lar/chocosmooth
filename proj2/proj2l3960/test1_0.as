        lw      0   1   max
        lw      0   2   Thefun
        lw      1   5   initia
start   jalr    1   2
        beq     1   5   done
        beq     0   0   start
done    halt
max     .fill   5
initia .fill   1
Sub     .fill 0