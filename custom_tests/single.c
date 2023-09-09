int f(int a, int b, int c, int d, int e, int f, int g, int h, int i, int j, int k, int l)
{
    int y = j + k + l;
    int z = 5;
    return z;
}

/*
f:
        addi    sp,sp,-64
        sw      s0,60(sp)
        addi    s0,sp,64
        sw      a0,-36(s0)
        sw      a1,-40(s0)
        sw      a2,-44(s0)
        sw      a3,-48(s0)
        sw      a4,-52(s0)
        sw      a5,-56(s0)
        sw      a6,-60(s0)
        sw      a7,-64(s0)
        lw      a4,4(s0)
        lw      a5,8(s0)
        add     a5,a4,a5
        lw      a4,12(s0)
        add     a5,a4,a5
        sw      a5,-20(s0)
        li      a5,5
        sw      a5,-24(s0)
        lw      a5,-24(s0)
        mv      a0,a5
        lw      s0,60(sp)
        addi    sp,sp,64
        jr      ra

*/
