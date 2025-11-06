#include "BspGen.h"
#include "Math.h"

#include <iostream>

#define PLANE_EPSILON 0.01
#define FLOAT_MAX 999999999
#define SPLIT_BALANCE 0.8f
#define MAX_WINDING 32