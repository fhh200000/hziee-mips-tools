main:
    add $a0,$zero,$zero    #00002020
    addi $a1,$zero,64      #20050014
    addi $a2,$zero,10      #2406000a
    jal bankmove
final_loop:
    j final_loop
bankmove:
    add $t0,$a0,$zero      #00804020
    add $t1,$a1,$zero      #00a04820
    add $t2,$a2,$zero      #00c05020
loop1:
    lw $t3,0($t0)          #8d0b0000
    sw $t3,0($t1)          #ad2b0000
    addi $t0,$t0,4         #21080004
    addi $t1,$t1,4         #21290004
    addi $t2,$t2,-1        #214affff
    bne $t2,$zero,loop1    #1540fffa
    jr $ra
