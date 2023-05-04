        lw      0       1       var1    // Load var1 to reg1
        lw      0       2       var2    // Load var2 to reg2
        add     1       2       3       // Add reg1 and reg2, store result in reg3
        sw      0       3       var3    // Store reg3 to var3
        lw      0       4       var4    // Load var4 to reg4
        lw      0       5       var5    // Load var5 to reg5
        nor     4       5       6       // Bitwise NOR reg4 and reg5, store result in reg6
        sw      0       6       var6    // Store reg6 to var6
        halt                           // Halt the program
var1    .fill   2             // Initialize var1 with value 2
var2    .fill   3             // Initialize var2 with value 3
var3    .fill   0             // Initialize var3 with value 0
var4    .fill   4             // Initialize var4 with value 4
var5    .fill   5             // Initialize var5 with value 5
var6    .fill   0             // Initialize var6 with value 0

