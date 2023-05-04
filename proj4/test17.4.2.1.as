        lw      0       1       16      ; Load value from memory location '16' into R1
        lw      0       2       20      ; Load value from memory location '20' into R2
        lw      0       3       24      ; Load value from memory location '24' into R3
        lw      0       4       28      ; Load value from memory location '28' into R4
        add     1       2       1       ; Add value in R2 to R1, store result in R1
        add     2       3       2       ; Add value in R3 to R2, store result in R2
        nor     3       3       3       ; Perform bitwise NOR operation on R3, store result in R3
        nor     4       4       4       ; Perform bitwise NOR operation on R4, store result in R4
        sw      0       1       17      ; Store value in R1 to memory location '17'
        sw      0       2       21      ; Store value in R2 to memory location '21'
        sw      0       3       25      ; Store value in R3 to memory location '25'
        sw      0       4       29      ; Store value in R4 to memory location '29'
        noop                            ; No operation
        noop                            ; No operation
        noop                            ; No operation
        halt                            ; Halt the CPU
16      .fill   4               ; Variable '16' with value 4
20      .fill   2               ; Variable '20' with value 2
24      .fill   2               ; Variable '24' with value 2
28      .fill   0               ; Variable '28' with value 0
17      .fill   0               ; Placeholder for result of R1 after add instruction
21      .fill   0               ; Placeholder for result of R2 after add instruction
25      .fill   0               ; Placeholder for result of R3 after nor instruction
29      .fill   0               ; Placeholder for result of R4 after nor instruction
