#include <stdio.h>

float f(float x)
{ 
    return x*x; 
}

float Trap(float a, float b, int n, float h)
{
    float integral, x;
    int i;
    integral = (f(a) + f(b))/2.0;
    x = a;
    for (i=1; i<=n-1; i++)
    {
        x += h;
        integral += f(x);
    }
    return integral*h;
}

int main(int argc, char** argv)
{
    float h, a=0.0, b=1.0, integral=0, local_a, local_b;
    int i, n=1000, nproc=10, local_n=n/nproc;
    h = (b-a)/n;
    for(i=0; i<nproc; i++)
    {
        local_a = a + i*local_n*h;
        local_b = a + (i+1)*local_n*h;
        integral += Trap(local_a, local_b, local_n, h);
    }
    printf("With n = %d trapezoids\n", n);
    printf("The integral from %f to %f is approx %f\n", 
         a, b, integral);
    return 0;
}