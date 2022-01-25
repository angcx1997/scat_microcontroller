/*
 * wave_lookup.h
 *
 *  Created on: 20 Jan 2022
 *      Author: ray
 */

#ifndef INC_WAVE_LOOKUP_H_
#define INC_WAVE_LOOKUP_H_

#include <math.h>
#include <stdio.h>


#define SINE_ARR_SIZE 40
#define FOURIER_ARR_SIZE 94


extern const int sine_table[40];
extern const int fourier_table[94];

float sin1(float phase, uint32_t i);
float fourier(float phase, uint32_t i);

#endif /* INC_WAVE_LOOKUP_H_ */
