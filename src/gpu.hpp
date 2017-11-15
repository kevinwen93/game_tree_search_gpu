#ifndef GPU_HPP
#define GPU_HPP

#define __CL_ENABLE_EXCEPTIONS
//#include "err_code.h"
#include "cl.hpp"
#include "util.hpp"
//#include "device_picker.hpp"

#include "external.h"


//#include "heuristicEvaluation.h"

#ifndef DEVICE
#define DEVICE CL_DEVICE_TYPE_DEFAULT
#endif

std::vector<int> GPUcaller(int size);
#endif