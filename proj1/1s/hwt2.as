	    lw	    0       1       ten
	    lw	    0	    2	    one
	    lw	    0	    3	    neg1
start	add	    1	    3	    1
        beq	    1	    0	    done     
        add	    2	    2	    2
	    beq	    0	    0	    start
	    noop
done	halt
ten	    .fill	10
one	    .fill	1
neg1	.fill	-1
zero	.fill	0