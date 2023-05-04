        lw      0       1       ten     ; Load value from memory location 'ten' into R1
        add     1       1       2       ; Add R1 and 2, store result in R2
        add     1       2       1       ; Add R1 and R2, store result in R2
        add     2       1       2       ; Add R2 and 1, store result in R2
        add     2       2       2       ; Add R2 and 2, store result in R2
        beq     0       0       next  ; Branch to label 'next' if R0 is equal to 0
        add     1       1       1       ; Add R1 and 1, store result in R1
next    sw      0       2       mark     ; Store value in R2 to memory location 'mark'
        halt                            ; Halt the CPU
ten     .fill   10              ; Variable 'ten' with value 10
mark    .fill   470             ; Variable 'mark' with value 470
        
