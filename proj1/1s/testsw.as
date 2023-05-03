        lw      0       1       first
        lw      0       2       second
        lw      0       3       third
        lw      0       5       fifth
        lw      0       4       fourth
        noop
        sw      0       1       1
        sw      0       2       3
        sw      0       3       3
        sw      0       6       5
        sw      0       5       4
        sw      0       4       2
        halt
first   .fill   1
second  .fill   2
third   .fill   3
fourth  .fill   4
fifth   .fill   5