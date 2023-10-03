//
//  perlin.h
//  hello_world
//
//  Created by Pablo Manuel Arjonilla Cobreros on 3/10/23.
//

#ifndef perlin_h
#define perlin_h

#include <stdio.h>

float noise2d(float x, float y);
float perlin2d(float x, float y, float freq, int depth);

#endif /* perlin_h */
