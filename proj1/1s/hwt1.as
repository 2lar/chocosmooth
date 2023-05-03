        noop    2   0   2 
        lw      1   3   512     
loop    beq     1   5   end 
        jalr    3   6   3
        beq     0   0   loop 
end     halt                    
        add      4   7   4