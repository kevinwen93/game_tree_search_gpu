#include "gpu.hpp"
//#include "heuristicEvaluation.h"

/*std:: vector<int> mobility_buffer;
std:: vector<int> potMobility_buffer;
std:: vector<int> makeup_buffer;
std:: vector<int> naivety_buffer;
std:: vector<int> parity_buffer;
std:: vector<int> corners_buffer;
std:: vector<int> stability_buffer;
std:: vector<int> value_list;*/


std::vector<int> GPUcaller(int size){
    std::vector<int>  value_list(size);

    cl::Buffer output_buffer;
    cl::Buffer mobility_gpu;
    cl::Buffer potMobility_gpu;
    cl::Buffer makeup_gpu;
    cl::Buffer naivety_gpu;
    cl::Buffer parity_gpu;
    cl::Buffer corners_gpu;
    cl::Buffer stability_gpu;

    cl::Context context(DEVICE);

    cl::Program program(context, util::loadProgram("/home/kevin/Desktop/Rochester/458/project/opensource_reversi/delivery/gpu-version/src/score_cal.cl"), true);

    cl::CommandQueue queue(context);
   
    cl::make_kernel<cl::Buffer, cl::Buffer, cl::Buffer, cl::Buffer, cl::Buffer, cl::Buffer, cl::Buffer, cl::Buffer, int> score_cal(program, "score_cal");

    mobility_gpu = cl::Buffer(context, mobility_buffer.begin(), mobility_buffer.end(), true);
    potMobility_gpu = cl::Buffer(context, potMobility_buffer.begin(), potMobility_buffer.end(), true);
    makeup_gpu = cl::Buffer(context, makeup_buffer.begin(), makeup_buffer.end(), true);
    naivety_gpu = cl::Buffer(context, naivety_buffer.begin(), naivety_buffer.end(), true);
    parity_gpu = cl::Buffer(context, parity_buffer.begin(), parity_buffer.end(), true);
    corners_gpu = cl::Buffer(context, corners_buffer.begin(), corners_buffer.end(), true);
    stability_gpu = cl::Buffer(context, stability_buffer.begin(), stability_buffer.end(), true);

    output_buffer = cl::Buffer(context, CL_MEM_WRITE_ONLY, sizeof(int) * size);

    score_cal(
    cl::EnqueueArgs(
        queue,
        cl::NDRange(size)), 
    mobility_gpu,
    potMobility_gpu,
    makeup_gpu,
    naivety_gpu,
    parity_gpu,
    corners_gpu,
    stability_gpu,
    output_buffer,
    size);

    cl::copy(queue, output_buffer, value_list.begin(), value_list.end());
    
    return value_list;

}