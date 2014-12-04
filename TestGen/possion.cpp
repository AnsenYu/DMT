#include "stdafx.h"
#include "possion.h"
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>



int possion(double Lambda)  /* ����һ�����ɷֲ����������LamdaΪ����ƽ����*/
{
        int k = 0;
        long double p = 1.0;
        long double l=exp(-Lambda);  /* Ϊ�˾��ȣ��Ŷ���Ϊlong double�ģ�exp(-Lambda)�ǽӽ�0��С��*/
        //printf("%.15Lf\n",l);
        while (p>=l)
        {
                double u = U_Random();
                p *= u;
                k++;
        }
        return k-1;
}
 
double U_Random()   /* ����һ��0~1֮�������� */
{
        double f;
        //srand( (unsigned)time( NULL ) );
        f = (float)(rand() % 100);
        /* printf("%f\n",f); */
        return f/100;
}


int randomExponential(int expect)
{
	//������1/lambda
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