		lw	0	2	mcand
		lw	0	3	mplier
		lw	0	4	its
		lw	0	5	one		disp
start	nor	3	3	3
		nor	5	5	5
		nor	3	5	7
		beq	7	0	even
		add	1	2	1
even	add	2	2	2		if even cause leftshift and add displace
		lw	0	6	neg1
		lw	0	3	mplier
		nor	5	5	5
		add 5	5	5
		add	4	6	4
		beq	0	4	done
		beq	0	0	start
done	halt
mcand	.fill	6203
mplier	.fill	1429
neg1	.fill	-1
one		.fill	1
its		.fill	16
