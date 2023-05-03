subOne  lw          0       2       neg1       ; $2 = -1
        add         1       2       1          ; $1 = $1 - 1
        jalr        7       6                  ; Jump back to where we were called from (main.as)
neg1    .fill       -1
SubAdr  .fill       subOne                     ; Define where our function definition starts