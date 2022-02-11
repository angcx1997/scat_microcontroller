/*
 * speed_limiter.h
 *
 *  Created on: 10 Feb 2022
 *      Author: ray
 */

#ifndef INC_SPEED_LIMITER_H_
#define INC_SPEED_LIMITER_H_

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "stm32f4xx.h"
//user need to input the following

#define CMD_VEL_TIMEOUT 0.1  //Unit:s
#define FREQUENCY 1000

typedef struct{
	float min_vel; 			/*!< Motor minimum velocity >*/
	float max_vel; 			/*!< Motor maximum speed >*/
	float min_acc; 			/*!< Motor minimum acceleration >*/
	float max_acc;			/*!< Motor maximum acceleration >*/
	float min_jerk; 		/*!< Motor minimum jerk >*/
	float max_jerk;			/*!< Motor maximum jerk >*/
}speedConfig;

typedef struct{
	float v;						/*!< Motor current velocity >*/
	float v0;						/*!< Previous velocity >*/
	float v1;						/*!< Previous velocity to v0>*/
	float dt;						/*!< Interval between each call of speed limit function>*/
	uint32_t curr_t;				/*!< Store current time tick>*/
	uint32_t last_t;				/*!< Store previous time tick>*/
	speedConfig* speed_config;		/*!< Store speed configuration set by user>*/
	uint8_t exponential_mapping;   	/*!< Enable or Disable exponential mapping for better control over lower velocity>*/
}limiter_t;

//User configurable output speed characteristic
extern speedConfig linear_speed_config;
extern speedConfig angular_speed_config;
extern limiter_t linear_limit;
extern limiter_t angular_limit;

/**
 * \brief Initialize speed limiter struct
 * \param [in]      limiter pointer to struct user want to limit speed
 * \param [in]      speed_config user desired configuration
 * \return Limiting factor (1.0 if none)
 */
void SL_Init(limiter_t* limiter, speedConfig* speed_config);

/**
     * \brief Limit the velocity and acceleration
     * \param [in, out] v  Velocity [m/s]
     * \param [in]   limiter pointer to struct that need to be limit
     * \return Limiting factor (1.0 if none)
     */
float SL_Limit(limiter_t* limiter);



#endif /* INC_SPEED_LIMITER_H_ */
