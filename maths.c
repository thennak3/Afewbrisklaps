#include "maths.h"

double clamp(double val, double mininum, double maximum)
{
    if(val<mininum)
        return mininum;
    if(val > maximum)
        return maximum;
    return val;
}

double MAX(double x, double y)
{
    if(x > y)
        return x;
    return y;
}
double MIN(double x, double y)
{
    if(x < y)
        return x;
    return y;
}
double SGN(double x)
{
    if(x < 0)
        return -1.0;
    if(x > 0)
        return 1.0;
    return 0.0;
}
