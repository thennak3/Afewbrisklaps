#include "offfile.h"
#include "vector.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

Object3D* readOFFFile(char * file)
{

    char buff[STRINGSIZE];
    Object3D* ofile;
    ofile = (Object3D*)malloc(sizeof(Object3D));

    int loadcolours = 0;

    FILE *infile;
    infile = fopen(file,"r");
    if(infile == NULL)
    {
        printf("%s: file couldn't be opened\n",file);
        ofile->nverts = 0;
        ofile->nfaces = 0;
        return ofile;
    }
    fscanf(infile, "%[^\n]", buff);

    printf("opened %s ok, first line is %s\n",file,buff);

    if(strcmp(buff,"COFF")!= 0)
    {
        ofile->hascolors = 0;
        if(strcmp(buff,"OFF")!= 0)
        {
            ofile->nverts = 0;
            ofile->nfaces = 0;
            return ofile;
        }
    }
    else
    {
        printf("Loading colors...\n");
        ofile->hascolors = 1;
        loadcolours = 1;
    }

    int ibuf1,ibuf2,ibuf3,ibuf4;
    float fbuf1,fbuf2,fbuf3;
    int col1,col2,col3,col4;
    //get the number of verts and faces + eat the zero
    fscanf(infile, " %d %d %d",&ibuf1,&ibuf2,&ibuf3);
    ofile->nverts = ibuf1;
    ofile->nfaces = ibuf2;


    printf("Read first line, values are %d and %d\n", ibuf1,ibuf2);

    //allocate space for vertices and faces
    ofile->vertices = malloc(sizeof(vector) * ofile->nverts);
    ofile->faces = malloc(sizeof(Face) * ofile->nfaces);
    ofile->normals = malloc(sizeof(vector) * ofile->nfaces);

    if(loadcolours)
        ofile->colors = malloc(sizeof(Color) * ofile->nverts);


    vector cog;
    float max;
    cog.x = 0;
    cog.y = 0;
    cog.z = 0;
    max = 0;
    for(int i = 0; i < ofile->nverts; i++)
    {
        //printf("going to read in values...\n");
        if(loadcolours)
            fscanf(infile, " %f %f %f %d %d %d %d", &fbuf1,&fbuf2,&fbuf3,&col1,&col2,&col3,&col4);
        else
            fscanf(infile, " %f %f %f", &fbuf1,&fbuf2,&fbuf3);
        //fprintf("Read line %d with values %f %f %f\n",i+1,fbuf1,fbuf2,fbuf3);

        ofile->vertices[i].x = fbuf1;
        ofile->vertices[i].y = fbuf2;
        ofile->vertices[i].z = fbuf3;

        cog.x += fbuf1;
        cog.y += fbuf2;
        cog.z += fbuf3;

        if(loadcolours)
        {
            printf("Adding colors from vert %d %d %d %d %d\n",i,col1,col2,col3,col4);
            ofile->colors[i][0] = (float)col1 * 0.0039215f;
            ofile->colors[i][1] = (float)col2 * 0.0039215f;
            ofile->colors[i][2] = (float)col3 * 0.0039215f;
            ofile->colors[i][3] = (float)col4 * 0.0039215f;
        }


    }

    cog.x = cog.x / ofile->nverts;
    cog.y = cog.y / ofile->nverts;
    cog.z = cog.z / ofile->nverts;

    //printf("%f %f %f\n",cog[0],cog[1],cog[2]);

    //printf("%f\n",max);

    ofile->positionAdjustment = cog;

    float mag = 0;
    //move and divide vertices
    for(int i = 0;i < ofile->nverts; i++)
    {
        //would be nicer if this was inside the vector class and simply a function but arrays are weird
        ofile->vertices[i].x = ofile->vertices[i].x - cog.x;
        ofile->vertices[i].y = ofile->vertices[i].y - cog.y;
        ofile->vertices[i].z = ofile->vertices[i].z - cog.z;

        mag = VectorMagnitude((vector)ofile->vertices[i]);
        if(mag > max)
            max = mag;

    }

    max = 1.0f / max;
    ofile->calcMax = max;

    printf("%s file scaler %f\n",file,max);
    for(int i = 0;i < ofile->nverts; i++)
    {
        ofile->vertices[i].x = ofile->vertices[i].x * max;
        ofile->vertices[i].y = ofile->vertices[i].y * max;
        ofile->vertices[i].z = ofile->vertices[i].z * max;
    }

    for(int i = 0;i<ofile->nfaces;i++)
    {
        fscanf(infile," %d %d %d %d",&ibuf1,&ibuf2,&ibuf3,&ibuf4);
        //for the time being we discard the first value, assumedly this will differentiate between quads and tri's in future
        //printf("Read line %d with values %d %d %d\n",i+1,ibuf2,ibuf3,ibuf4);

        ofile->faces[i][0] = ibuf2;
        ofile->faces[i][1] = ibuf3;
        ofile->faces[i][2] = ibuf4;



    }

    //scale and normalize model here

    //generate normals for surfaces and store
    for(int i = 0;i<ofile->nfaces;i++)
    {
        Face f;
        f[0] = ofile->faces[i][0];
        f[1] = ofile->faces[i][1];
        f[2] = ofile->faces[i][2];



        //vector r1 = VectorSubtract((vector)P3toV(ofile->vertices[f[1]]),(vector)P3toV(ofile->vertices[f[0]]));
        //vector r2 = VectorSubtract((vector)P3toV(ofile->vertices[f[2]]),(vector)P3toV(ofile->vertices[f[0]]));
        vector n = TriangleSurfaceNormal(ofile->vertices[f[0]],ofile->vertices[f[1]],ofile->vertices[f[2]]);
        //printf("%f %f %f\n",n.x,n.y,n.z);
        ofile->normals[i].x = n.x;
        ofile->normals[i].y = n.y;
        ofile->normals[i].z = n.z;
    }

    fclose(infile);
    return ofile;
}
