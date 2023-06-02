        sw      0       1       first
        sw      0       1       second
        sw      0       1       Third
        sw      0       1       fourth  
        lw      0       5       fifth
        add     1       1       1
        add     1       5       1
        nor     1       1       2
        noop
        noop
branch  beq     2       3       here
        nor     1       1       0
here    add     1       1       1
        halt
first   .fill   -7233
second  .fill   3
Third   .fill   -5000
fourth  .fill   1
fifth   .fill   5