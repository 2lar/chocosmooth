        lw      0       1       label    // Load label to reg1
        noop                           // NOOP instruction
        noop                           // NOOP instruction
        noop                           // NOOP instruction
        add     1       1       3       // Add reg1 with itself, store result in reg3
        noop                           // NOOP instruction
        noop                           // NOOP instruction
        lw      3       1       label    // Load label to reg1
        halt                           // Halt the program
label    .fill   12345           // Initialize label with value 12345
