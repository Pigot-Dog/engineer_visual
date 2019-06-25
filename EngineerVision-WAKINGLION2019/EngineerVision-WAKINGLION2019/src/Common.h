//-----------------------------------【常用库】--------------------------------------------
// brief：
//------------------------------------------------------------------------------------------------

#ifndef COMMON_H
#define COMMON_H

#include "Settings.h"

#define USE_UBUNTU

#ifdef USE_UBUNTU
#include <sys/time.h>
#define TIME_BEGIN() {timeval t_b, t_e; gettimeofday(&t_b, 0);
#define TIME_END(TAG) gettimeofday(&t_e, 0); printf("=== %s time: %lf s\n", TAG, (t_e.tv_sec - t_b.tv_sec) + (t_e.tv_usec - t_b.tv_usec)*1e-6); }
#endif

#define PI 3.1415926

#define math_max(a,b) ((a)>(b)?(a):(b))
#define math_min(a,b) ((a)<(b)?(a):(b))

#define POINT_DIST(p1,p2) std::sqrt((p1.x-p2.x)*(p1.x-p2.x)+(p1.y-p2.y)*(p1.y-p2.y))

typedef signed int	       s32;
typedef unsigned int	   u32;
typedef signed short	   s16;
typedef unsigned short	   u16;
typedef unsigned char	   u8;

#endif // COMMON_H
