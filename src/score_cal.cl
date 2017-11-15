__kernel void score_cal(                             
   __global int* a,                      
   __global int* b,                      
   __global int* c,
   __global int* d,                      
   __global int* e,                      
   __global int* f,                   
   __global int* g,
   __global int* h,
   const unsigned int count)               
{                                          
   int i = get_global_id(0);               
   if(i < count)  {
       h[i] = a[i]*5 + b[i]*2 + c[i] + d[i]*5 +e[i]*500 + f[i]*1000 +g[i]*1000;                 
   }
}    