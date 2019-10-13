#include "vector.h"
#include <gl/freeglut.h>
#ifndef OFFFILE_H
#define OFFFILE_H

#define STRINGSIZE 1024


typedef int Face[3];
typedef float Color[4];
typedef struct{

    int nverts;
    int nfaces;
    vector* vertices;
    Face* faces;
    vector* normals;
    int hascolors;
    Color* colors;
    float calcMax;
    vector positionAdjustment;

} Object3D;


Object3D* readOFFFile(char * file);

#endif // OFFFILE_H

