        lw      0       1       data1    // Load data1 to reg1
        lw      0       2       data2    // Load data2 to reg2
        lw      0       3       data3    // Load data3 to reg3
        lw      0       4       data4    // Load data4 to reg4
        lw      0       5       data5    // Load data5 to reg5
        lw      0       6       data6    // Load data6 to reg6
        sw      0       1       data1    // Store reg1 to data1
        sw      0       2       data2    // Store reg2 to data2
        sw      0       3       data3    // Store reg3 to data3
        sw      0       4       data4    // Store reg4 to data4
        sw      0       5       data5    // Store reg5 to data5
        sw      0       6       data6    // Store reg6 to data6
        add     1       2       3        // Add reg1 with reg2, store result in reg3
        lw      0       6       data9    // Load data9 to reg6
        halt
data1    .fill    100    ; Data1 value
data2    .fill    200    ; Data2 value
data3    .fill    300    ; Data3 value
data4    .fill    400    ; Data4 value
data5    .fill    500    ; Data5 value
data6    .fill    600    ; Data6 value
data9    .fill    900    ; Data9 value