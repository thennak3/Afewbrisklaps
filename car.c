#include "car.h"
#include <math.h>
#include "maths.h"

#include <stdio.h>

//car physics cheat sheet http://www.asawicki.info/Mirror/Car%20Physics%20for%20Games/Car%20Physics%20for%20Games.html


//Below is based on tutorial by "Car Physics for Games" by Marco Monster, link is above. 

void do_physics( CAR *car, double delta_t )
{

    CARTYPE * ct = car->cartype;
	double sn = sin(car->heading); //
	double cs = cos(car->heading); //


	//x = x, y = z
	car->velocity_c.x = cs * car->velocity.x + sn * car->velocity.z; //
	car->velocity_c.z = cs * car->velocity.z - sn * car->velocity.x; //

	//calculate weight of car given acceleration

    double axleWeightFront = ct->mass * (car->axleWeightRatioFront * GRAVITY - ct->weightTransfer * car->acceleration_c.x * ct->cgheight / car->wheelbase); //
    double axleWeightRear  = ct->mass * (car->axleWeightRatioRear  * GRAVITY + ct->weightTransfer * car->acceleration_c.x * ct->cgheight / car->wheelbase); //

    //printf("axleWeightFront %f axleWeightRear %f\n",axleWeightFront,axleWeightRear);

    //calculate rotation rate for front / rear

    double rotationRateFront = ct->cgToFrontAxle * car->yawRate; //
    double rotationRateRear  = -ct->cgToRearAxle * car->yawRate; //

    //calculate slip angle, e.g. difference between where wheels are pointing and where it's going

    double slipAngleFront = atan2(car->velocity_c.z + rotationRateFront, fabs(car->velocity_c.x)) - SGN(car->velocity_c.x) * car->steerangle; //
    double slipAngleRear  = atan2(car->velocity_c.z + rotationRateRear, fabs(car->velocity_c.x)); //

    //printf("val 1 %f, val 2 %f\n",atan2(car->velocity_c.z + yawSpeedFront, fabs(car->velocity_c.x)),atan2(car->velocity_c.z + yawSpeedRear, fabs(car->velocity_c.x)));
    //printf("slipAngleFront %f slipAngleRear %f\n",slipAngleFront,slipAngleRear);

    //tyre grip,

    double tyreGripFront = ct->tireGrip * ((car->ontrack  == 1) ? 1.0 : 0.5 ); //
    double tyreGripRear = (ct->tireGrip * (1.0 - car->ebrake * (1.0 - ct->lockGrip))) * ((car->ontrack  == 1) ? 1.0 : 0.5 ); //

    //printf("tireGripFront %f tireGripRear %f\n", tireGripFront,tireGripRear);

    double frictionForceFront_cz = clamp((double)(-ct->cornerStiffnessFront) * slipAngleFront, tyreGripFront * -1.0, tyreGripFront) * axleWeightFront; //
    double frictionForceRear_cz = clamp((double)(-ct->cornerStiffnessRear) * slipAngleRear, tyreGripRear * -1.0 , tyreGripRear) * axleWeightRear; //

    //printf("val 3 %f, val 4 %f\n", clamp((double)(-ct->cornerStiffnessFront) * slipAngleFront, tireGripFront * -1.0, tireGripFront), clamp((double)(-ct->cornerStiffnessRear) * slipAngleRear, tireGripRear * -1.0 , tireGripRear));
    //printf("frictionForceFront_cz %f frictionForceRear_cz %f\n",frictionForceFront_cz,frictionForceRear_cz);

    double brake = MIN(car->brake * ct->brakeForce + car->ebrake * ct->eBrakeForce, ct->brakeForce); //
    double throttle = car->throttle * ct->engineForce; //

    double tractionForce_cx = (throttle - brake * SGN(car->velocity_c.x)) * ((car->ontrack  == 1) ? 1.0 : 0.4 ); //
    double tractionForce_cz = 0; //

    double dragForce_cx = -ct->rollResist * car->velocity_c.x - ct->airResist * car->velocity_c.x * fabs(car->velocity_c.x); //
    double dragForce_cz = -ct->rollResist * car->velocity_c.z - ct->airResist * car->velocity_c.z * fabs(car->velocity_c.z); //

    //printf("dragForce_cz %f\n",dragForce_cz);

    double totalForce_cx = dragForce_cx + tractionForce_cx; //
    double totalForce_cz = dragForce_cz + tractionForce_cz + cos(car->steerangle) * frictionForceFront_cz + frictionForceRear_cz; //

    //printf("totalForce_cz %f\n",totalForce_cz );

    car->acceleration_c.x = totalForce_cx / ct->mass;
    car->acceleration_c.z = totalForce_cz / ct->mass;

    //printf("acceleration_c.x %f  acceleration_c.z %f\n",car->acceleration_c.x,car->acceleration_c.z);

    car->acceleration.x = cs * car->acceleration_c.x - sn * car->acceleration_c.z;
    car->acceleration.z = sn * car->acceleration_c.x + cs * car->acceleration_c.z;



    car->velocity.x += car->acceleration.x * delta_t;
    car->velocity.z += car->acceleration.z * delta_t;

    car->absVel = VectorMagnitude(car->velocity);

    //printf("Velocity magniture %f\n",car->absVel);

    double angularTorque = (frictionForceFront_cz + tractionForce_cz) * ct->cgToFrontAxle - frictionForceRear_cz * ct->cgToRearAxle;

    if(fabs(car->absVel) < 0.5 && throttle == 0)
    {
        car->velocity.x = 0;
        car->velocity.z = 0;
        angularTorque = 0;
        car->yawRate = 0;
    }

    double angularAccel = angularTorque / car->interia;

    car->yawRate += angularAccel * delta_t;
    car->heading += car->yawRate * delta_t;

    car->position.x += car->velocity.x * delta_t;
    car->position.z += car->velocity.z * delta_t;

    //printf("car velocity x %f z %f\n",car->velocity.x,car->velocity.z);
}

void init_car_physics(CAR *car)
{
    car->interia = car->cartype->mass * car->cartype->intertiascale;
    car->wheelbase = car->cartype->cgToFrontAxle + car->cartype->cgToRearAxle;
    car->axleWeightRatioFront = car->cartype->cgToRearAxle / car->wheelbase;
    car->axleWeightRatioRear = car->cartype->cgToFrontAxle / car->wheelbase;
}


//process car input
void car_input(CAR *car, double throttle, int ebrake, double steering, double delta)
{

    car->ebrake = (double)ebrake;
    if(throttle > 0) //throttle on, going forward
    {
        car->brake = 0;
        if(car->throttle < 1)
            car->throttle += THROTTLE_ACCEL * delta;
        if(car->throttle > 1)
            car->throttle = 1;
    }
    else if(throttle == 0) // accelerator off, simulate lifting foot off
    {
        car->brake = 0;
        if(car->throttle > 0)
        {
            car->throttle -= THROTTLE_DECELL * delta;
            if(car->throttle < 0)
                car->throttle = 0;
        }
        else if(car->throttle < 0)
        {
            car->throttle += THROTTLE_DECELL * delta;
            if(car->throttle > 0)
                car->throttle = 0;
        }
    }
    else if(throttle < 0) //going backwards
    {
        car->brake = 1;
        car->throttle = 0;
    }


    if(steering == 0)
    {
        //move back towards 0 within 1/3rd of a second
        if(car->steerangle < 0)
        {
            car->steerangle += MAX_STEERING_ANGLE * delta * 3.0f;
            if(car->steerangle > 0)
                car->steerangle = 0;
        }
        else if(car->steerangle > 0)
        {
            car->steerangle -= MAX_STEERING_ANGLE * delta * 3.0f;
            if(car->steerangle < 0)
                            car->steerangle = 0;
        }
    }
    else
    {
        //get to max steering angle within a second, steering angle may go above max but that's ok
        if(car->steerangle * steering < MAX_STEERING_ANGLE )
        {
            //left steering should produce a negative result as we're multiplying by the sign of the steering value
            car->steerangle += MAX_STEERING_ANGLE * delta * steering * 1.5f;
        }
    }

    //printf("throttle %f  steering %f  brake %f  ebrake %f\n",car->throttle, car->steerangle ,car->brake, car->ebrake);

}




