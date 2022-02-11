/*
 * speed_limiter.c
 *
 *  Created on: 10 Feb 2022
 *      Author: ray
 */

#include "speed_limiter.h"
#define MAX(x,y) (((x) > (y)) ? (x) : (y))
#define MIN(x,y) (((x) < (y)) ? (x) : (y))

/*********************************************************************
 * @brief          	- Clamp x between min and max
 *
 * @param[in]     	- x		input value
 * @param[in]     	- min 	min value allowed
 * @param[in]     	- max 	max value allowed
 *
 * @return            - None
 *
 * @Note              -
 */
static float clamp(float x, float min, float max);

/**
 * \brief Limit the velocity
 * \param [in, out] v Velocity [m/s]
 * \param [in]      v_min min allowable velocity [m/s]
 * \param [in]      v_max max allowable velocity [m/s]
 * \return Limiting factor (1.0 if none)
 *
 */
static float limit_velocity(float* v, float v_min, float v_max);

/**
 * \brief Limit the acceleration
 * \param [in, out] v  Velocity [m/s]
 * \param [in]      v0 Previous velocity [m/s]
 * \param [in]      dt Time step [s]
 * \param [in]      a_min min allowable acceleration [m/s2]
 * \param [in]      a_max max allowable acceleration [m/s2]
 * \return Limiting factor (1.0 if none)
 */
static float limit_acceleration(float* v, float v0, float dt, float a_min, float a_max);

/**
 * \brief Limit the jerk
 * \param [in, out] v  Velocity [m/s]
 * \param [in]      v0 Previous velocity to v  [m/s]
 * \param [in]      v1 Previous velocity to v0 [m/s]
 * \param [in]      j_min min allowable jerk [m/s3]
 * \param [in]      j_max max allowable jerk [m/s3]
 * \param [in]      dt Time step [s]
 * \return Limiting factor (1.0 if none)
 * \see http://en.wikipedia.org/wiki/Jerk_%28physics%29#Motion_control
 */
static float limit_jerk(float* v, float v0, float v1, float dt, float j_min, float j_max);

void SL_Init(limiter_t* limiter, speedConfig* speed_config)
{
	memset(limiter, 0, sizeof(*limiter));
	limiter->speed_config = speed_config;
	//TODO: Test effect of exponential mapping
}

float SL_Limit(limiter_t* limiter)
{
	if(limiter->dt == 0 && limiter->last_t == 0){
		limiter->last_t = limiter->curr_t;
		return 1.0;
	}

	limiter->dt = (float)(limiter->curr_t - limiter->last_t)/FREQUENCY;

	if(limiter->dt > CMD_VEL_TIMEOUT){
		speedConfig dummy = *(limiter->speed_config);
		SL_Init(limiter, &dummy);
		return 1.0;
	}

	if (limiter->exponential_mapping == 1){
		float norm = (limiter->v > 0) ? limiter->speed_config->max_vel : fabs(limiter->speed_config->min_vel);
		float x = limiter->v / norm;
		x = x * x * x;
		limiter->v *= x;
	}

	float tmp = limiter->v;
	float* v = &limiter->v;
	float v0 = limiter->v0;
	float v1 = limiter->v1;
	limit_jerk(v, v0, v1, limiter->dt,limiter->speed_config->min_jerk, limiter->speed_config->max_jerk);
	limit_acceleration(v, v0, limiter->dt,limiter->speed_config->min_acc, limiter->speed_config->max_acc);
	limit_velocity(v,limiter->speed_config->min_vel, limiter->speed_config->max_vel);

	limiter->last_t = limiter->curr_t;

	limiter->v0 = limiter->v;
	limiter->v1 = limiter->v0;
	return tmp != 0.0 ? limiter->v / tmp : 1.0;
}



static float limit_velocity(float* v, float v_min, float v_max)
{
	const float tmp = *v;
	*v = clamp(*v, v_min, v_max);

	return tmp != 0.0 ? *v / tmp : 1.0;
}

static float limit_acceleration(float* v, float v0, float dt, float a_min, float a_max)
{
	float tmp = *v;

	float dv_min = a_min * dt;
	float dv_max = a_max * dt;

	float dv = clamp(*v - v0, dv_min, dv_max);

	*v = v0 + dv;

	return tmp != 0.0 ? *v / tmp : 1.0;
}

static float limit_jerk(float* v, float v0, float v1, float dt, float j_min, float j_max)
{
	float tmp = *v;

	float dv  = *v  - v0;
	float dv0 = v0 - v1;

	float dt2 = 2. * dt * dt;

	float da_min = j_min * dt2;
	float da_max = j_max * dt2;

	float da = clamp(dv - dv0, da_min, da_max);

	*v = v0 + dv0 + da;

	return tmp != 0.0 ? *v / tmp : 1.0;
}




static float clamp(float x, float min, float max)
{
  return MIN(MAX(min, x), max);
}
