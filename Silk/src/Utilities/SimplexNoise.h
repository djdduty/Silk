/* 
 * Made by Eliot Eshelman of http://www.6by9.net/
 * ( http://www.6by9.net/simplex-noise-for-c-and-python/ )
 */
#pragma once
#include <System/SilkTypes.h>

namespace Silk
{
    f32 octave_noise_2d(const f32 octaves,const f32 persistence,const f32 scale,const f32 x,const f32 y);
    f32 octave_noise_3d(const f32 octaves,const f32 persistence,const f32 scale,const f32 x,const f32 y,const f32 z);
    f32 octave_noise_4d(const f32 octaves,const f32 persistence,const f32 scale,const f32 x,const f32 y,const f32 z,const f32 w);

    // Scaled Multi-octave Simplex noise
    // The result will be between the two parameters passed.
    f32 scaled_octave_noise_2d(const f32 octaves,const f32 persistence,const f32 scale,const f32 loBound,const f32 hiBound,const f32 x,const f32 y);
    f32 scaled_octave_noise_3d(const f32 octaves,const f32 persistence,const f32 scale,const f32 loBound,const f32 hiBound,const f32 x,const f32 y,const f32 z);
    f32 scaled_octave_noise_4d(const f32 octaves,const f32 persistence,const f32 scale,const f32 loBound,const f32 hiBound,const f32 x,const f32 y,const f32 z,const f32 w);

    // Scaled Raw Simplex noise
    // The result will be between the two parameters passed.
    f32 scaled_raw_noise_2d(const f32 loBound,const f32 hiBound,const f32 x,const f32 y);
    f32 scaled_raw_noise_3d(const f32 loBound,const f32 hiBound,const f32 x,const f32 y,const f32 z);
    f32 scaled_raw_noise_4d(const f32 loBound,const f32 hiBound,const f32 x,const f32 y,const f32 z,const f32 w);
};

