#include "vector.h"


#ifndef CAR_H
#define CAR_H

#define DELTA_T			0.01		/* time between integration steps in physics modelling */

#ifndef M_PI						/* if not in math.h, i.e. MSVC */
# define M_PI 3.1415926
#endif


typedef struct CARTYPE
{
	double	mass;			// in kg
	double   intertiascale;
	double   halfwidth;
    double   cgToFront;
    double   cgToRear;
    double   cgToFrontAxle;
    double   cgToRearAxle;
    double	cgheight;				// in m, height of CM from ground
    double   wheelRadius;
    double   wheelWidth;
    double   tireGrip;
    double   lockGrip;
    double   engineForce;
    double   brakeForce;
    double   eBrakeForce;
    double   weightTransfer;
    double   maxSteer;
    double   cornerStiffnessFront;
    double   cornerStiffnessRear;
    double   airResist;
    double   rollResist;

} CARTYPE;

typedef struct CAR
{
	CARTYPE	*cartype;			// pointer to static car data

	vector	position;		// position of car centre in world coordinates
	vector  velocity_c;
	vector	velocity;		// velocity vector of car in world coordinates


	double	slipAngleFront;
	double	heading;				// angle of car body orientation (in rads)
	double	angularvelocity;

	double   absVel;
	double   yawRate;

	vector acceleration_c;
	vector acceleration;

	double   steerangle;
	double	throttle;			// amount of throttle (input)
	double	brake;				// amount of braking (input)
	double   ebrake;

	double interia;
	double wheelbase;
	double axleWeightRatioFront;
	double axleWeightRatioRear;

	double movementscale;

	int ontrack;
} CAR;

#define GRAVITY 9.81


#define THROTTLE_ACCEL 1
#define THROTTLE_DECELL 3

#define MAX_STEERING_ANGLE 0.6



void do_physics( CAR *car, double delta_t );
void init_car_physics(CAR *car);
void car_input(CAR *car, double throttle, int ebrake, double steering, double delta);



#endif
