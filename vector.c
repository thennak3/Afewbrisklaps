#include "vector.h"
#include <math.h>

vector CreateEmtpyVector()
{
    vector newvec;
    newvec.x = 0;
    newvec.y = 0;
    newvec.z = 0;
}

vector VectorMultiplyByScaler (vector vec, float scaler)
{
    vector retv = (vector){.x = vec.x * scaler, .y = vec.y * scaler, .z = vec.z * scaler};
    return retv;
}

double VectorDotProduct(vector vec1, vector vec2)
{
    double retv =  vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z;
    return retv;
}

vector VectorCrossProduct(vector vec1, vector vec2)
{
    vector retv = (vector){.x = (vec1.y*vec2.z)-(vec1.z*vec2.y), .y = (vec1.z*vec2.x) - (vec1.x-vec2.z), .z = (vec1.x*vec2.y) - (vec1.y*vec2.x)};
    return retv;
}

vector VectorSubtract(vector vec1, vector vec2)
{
    vector retv = (vector){.x = vec1.x - vec2.x, .y = vec1.y - vec2.y, .z = vec1.z - vec2.z};
    return retv;
}

vector TriangleSurfaceNormal(vector point1, vector point2, vector point3)
{
    //vector u = VectorSubtract(point2,point1);
    //vector v = VectorSubtract(point3,point1);

    //Normals are still acting weird.
    vector u = {.x = point2.x - point1.x, .y = point2.y - point1.y, .z = point2.z - point1.z};
    vector v = {.x = point3.x - point1.x, .y = point3.y - point1.y, .z = point3.z - point1.z};
    vector retv = VectorCrossProduct(u,v);
    retv = VectorNormalize(retv);
    return retv;

}

vector VectorNormalize(vector vec)
{
    float sum = 0;
    sum = VectorMagnitude(vec);
    vector retv;
    retv.x = vec.x / sum;
    retv.y = vec.y / sum;
    retv.z = vec.z / sum;
    return retv;


}

double VectorMagnitude(vector point)
{
    double val = sqrt(pow(point.x,2) + pow(point.y,2) + pow(point.z,2));
    return val;
}
//https://open.gl/transformations
vector VectorRotateYAxis(vector point, double angle)
{
    vector retv = (vector){.x = cos(angle) * point.x + sin(angle)*point.z, .y = point.y, .z = -sin(angle) * point.x + cos(angle) * point.z};
    return retv;
}

vector VectorAdd(vector vec1, vector vec2)
{
    vector retv = (vector){.x = vec1.x + vec2.x, .y = vec1.y + vec2.y, .z = vec1.z + vec2.z};
    return retv;
}
