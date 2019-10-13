#include <gl/freeglut.h>
#include <math.h>

#ifndef VECTOR_H
#define VECTOR_H

#define P3toV(value) { .x = value[0], .y = value[1], .z = value[2] }

struct Vector{
    GLfloat x;
    GLfloat y;
    GLfloat z;
};

typedef struct Vector vector;

vector CreateEmtpyVector();

vector VectorMultiplyByScaler (vector vec, GLfloat scaler);

double VectorDotProduct (vector vec1, vector vec2);

vector VectorCrossProduct( vector vec1, vector vec2);

vector VectorSubtract(vector vec1, vector vec2);

vector VectorAdd(vector vec1, vector vec2);

vector TriangleSurfaceNormal(vector point1, vector point2, vector point3);

double VectorMagnitude(vector point);

vector VectorRotateYAxis(vector point, double angle);

vector VectorNormalize(vector vec);
#endif // VECTOR_H
