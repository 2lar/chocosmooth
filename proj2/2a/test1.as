        lw      0       1       input   load register 1 with input 9 - symbolic
        lw      0       2       aux     load register 2 with aux input (11) - symbolic
        add     0       1       3       load register 3 with 1 - numeric
        add     1       3       4       add reg 1 and reg 2 and store into reg 4
        nor     2       2       5       take nor of 2 itself and store into 5
        sw      5       3       0       store
        lw      0       6       neg1    load neg1 into reg6
start   beq     4       3       done
        beq     4       2       inc
        add     4       6       4
inc     add     3       3       3
done    halt                            end of program
input   .fill   9
aux     .fill   11
neg1    .fill   -1
stAddr  .fill   start                   will contain the address of start (7)