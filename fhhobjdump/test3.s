main3:
        add $t0,$zero,$zero     #00004020
        add $t1,$zero,$zero     #00004820
        addi $t2,$zero,20       #200a0014
loop1:  lw $t3,0x10($t1)        #8d2b0010
        add $t0,$t0,$t3         #010b4020
        addi $t1,$t1,4          #21290004
        addi $t2,$t2,-1         #214affff
        beq $t2,$zero,loop2     #11400001
        j loop1                 #08000003
loop2:
        sw $t3,0x30($zero)      #ac0b0030
