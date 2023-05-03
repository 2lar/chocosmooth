    lw      0       1       var1
    lw      0       2       var2
    add     1       2       3
    sw      3       0       var3
    lw      0       4       var4
    add     3       4       5
    sw      5       0       var5
    lw      0       6       var6
    nor     5       6       7
    sw      7       0       var7
    halt
var1    .fill   1
var2    .fill   2
var3    .fill   0
var4    .fill   3
var5    .fill   0
var6    .fill   4
var7    .fill   0