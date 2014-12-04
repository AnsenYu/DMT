#include "stdafx.h"
#include "possion.h"
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>



int possion(double Lambda)  /* 产生一个泊松分布的随机数，Lamda为总体平均数*/
{
        int k = 0;
        long double p = 1.0;
        long double l=exp(-Lambda);  /* 为了精度，才定义为long double的，exp(-Lambda)是接近0的小数*/
        //printf("%.15Lf\n",l);
        while (p>=l)
        {
                double u = U_Random();
                p *= u;
                k++;
        }
        return k-1;
}
 
double U_Random()   /* 产生一个0~1之间的随机数 */
{
        double f;
        //srand( (unsigned)time( NULL ) );
        f = (float)(rand() % 100);
        /* printf("%f\n",f); */
        return f/100;
}


int randomExponential(int expect)
{
	//期望是1/lambda
	double lambda = 1/((double)expect);
    double pV = 0.0;
    while(true)
    {
        pV = (double)rand()/(double)RAND_MAX;
        if (pV != 1)
        {
            break;
        }
    }
    pV = (-1.0/lambda)*log(1-pV);
    return (int)pV;
}