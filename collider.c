#include "collider.h"
#include "maths.h"
#include "vector.h"
#include <float.h>



int CheckCollision(vector * col1, int vq1, vector * col2, int vq2)
{

    if(vq1 == 0 || vq2 == 0)
        return 0;

    for(int i = 0; i < vq1;i++)
    {
        vector current;
        current.x = col1[i].x;
        current.z = col1[i].z;
        current.y = 0;

        vector next;
        next.x = col1[i+1 % vq1].x;
        next.z = col1[i+1 % vq1].x;
        next.y = 0;

        //printf("x %f y %f z %f\n",next.x,next.y,next.z);


        vector edge = VectorSubtract(next,current);

        vector axis;
        axis.x = -edge.z;
        axis.z = edge.x;
        axis.y = 0;

        double maxa = -DBL_MAX;
        double mina = DBL_MAX;

        double maxb = -DBL_MAX;
        double minb = DBL_MAX;

        for(int j = 0;j<vq1;j++)
        {
            float dota = VectorDotProduct(axis,col1[j]);
            if(dota < mina)
                mina = dota;
            if(dota > maxa)
                maxa = dota;
        }

        for(int j = 0;j<vq2;j++)
        {
            float dotb = VectorDotProduct(axis,col2[j]);
            if(dotb < minb)
                minb = dotb;
            if(dotb > maxb)
                maxb = dotb;
        }

        if(maxa< minb || mina > maxb)
            return 0;


    }

    return 1;

}
