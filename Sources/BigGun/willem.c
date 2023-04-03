 __noinline void * copy_32x4(void *destparam, void *srcparam, int size)
{
        int *dest=destparam;
        int *src=srcparam;
        register int A,B,C,D;

        size=size/16;
        for (; size; size--) {
                A = *src++;
                B = *src++;
                C = *src++;
                D = *src++;
                *dest++ = A;
                *dest++ = B;
                *dest++ = C;
                *dest++ = D;
        }
        return destparam;
}

 __noinline void * write_32x4(void *destparam, void *srcparam, size_t size)
{
        int *src=srcparam;
        int A=(int)(long)destparam;
        int B=(int)(long)destparam+1;
        int C=(int)(long)destparam+2;
        int D=(int)(long)destparam+3;
        size=size/16;
        for (; size; size--) {
                *src++=A;
                *src++=B;
                *src++=C;
                *src++=D;
        }
        return destparam;
}
 __noinline int read_32x4(void *destparam, void *srcparam, size_t size)
{
        int *src=srcparam;
        int A=0,B=0,C=0,D=0;
        size=size/16;
        for (; size; size--) {
                A += *src++;
                B += *src++;
                C += *src++;
                D += *src++;
        }
        return (A+B+C+D);
}

