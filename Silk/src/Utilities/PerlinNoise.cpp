#include <Utilities/PerlinNoise.h>

namespace Silk
{
    PerlinNoise::PerlinNoise()
    {
        m_Persistence = 0;
        m_Frequency = 0;
        m_Amplitude  = 0;
        m_Octaves = 0;
        m_Seed = 0;
    }

    PerlinNoise::PerlinNoise(f32 Persistence,f32 Frequency,f32 Amplitude,i32 Octaves,i32 Seed)
    {
        m_Persistence = Persistence;
        m_Frequency = Frequency;
        m_Amplitude  = Amplitude;
        m_Octaves = Octaves;
        m_Seed = 2 + Seed * Seed;
    }

    void PerlinNoise::SetParameters(f32 Persistence,f32 Frequency,f32 Amplitude,i32 Octaves,i32 Seed)
    {
        m_Persistence = Persistence;
        m_Frequency = Frequency;
        m_Amplitude  = Amplitude;
        m_Octaves = Octaves;
        m_Seed = 2 + Seed * Seed;
    }

    f32 PerlinNoise::GetNoise(f32 x,f32 y) const
    {
        return m_Amplitude * Total(x,y);
    }

    f32 PerlinNoise::Total(f32 i,f32 j) const
    {
        //properties of one octave (changing each loop)
        f32 t = 0.0f;
        f32 Amplitude = 1;
        f32 freq = m_Frequency;

        for(i32 k = 0; k < m_Octaves; k++) 
        {
            t += GetValue(j * freq + m_Seed,i * freq + m_Seed) * Amplitude;
            Amplitude *= m_Persistence;
            freq *= 2;
        }

        return t;
    }

    f32 PerlinNoise::GetValue(f32 x,f32 y) const
    {
        i32 Xi32 = (i32)x;
        i32 Yi32 = (i32)y;
        f32 Xfrac = x - Xi32;
        f32 Yfrac = y - Yi32;

        //noise values
        f32 n01 = Noise(Xi32-1,Yi32-1);
        f32 n02 = Noise(Xi32+1,Yi32-1);
        f32 n03 = Noise(Xi32-1,Yi32+1);
        f32 n04 = Noise(Xi32+1,Yi32+1);
        f32 n05 = Noise(Xi32-1,Yi32);
        f32 n06 = Noise(Xi32+1,Yi32);
        f32 n07 = Noise(Xi32,Yi32-1);
        f32 n08 = Noise(Xi32,Yi32+1);
        f32 n09 = Noise(Xi32,Yi32);

        f32 n12 = Noise(Xi32+2,Yi32-1);
        f32 n14 = Noise(Xi32+2,Yi32+1);
        f32 n16 = Noise(Xi32+2,Yi32);

        f32 n23 = Noise(Xi32-1,Yi32+2);
        f32 n24 = Noise(Xi32+1,Yi32+2);
        f32 n28 = Noise(Xi32,Yi32+2);

        f32 n34 = Noise(Xi32+2,Yi32+2);

        //find the noise values of the four corners
        f32 x0y0 = 0.0625*(n01+n02+n03+n04) + 0.125*(n05+n06+n07+n08) + 0.25*(n09);  
        f32 x1y0 = 0.0625*(n07+n12+n08+n14) + 0.125*(n09+n16+n02+n04) + 0.25*(n06);  
        f32 x0y1 = 0.0625*(n05+n06+n23+n24) + 0.125*(n03+n04+n09+n28) + 0.25*(n08);  
        f32 x1y1 = 0.0625*(n09+n16+n28+n34) + 0.125*(n08+n14+n06+n24) + 0.25*(n04);  

        //i32erpolate between those values according to the x and y fractions
        f32 v1 = i32erpolate(x0y0,x1y0,Xfrac); //i32erpolate in x direction (y)
        f32 v2 = i32erpolate(x0y1,x1y1,Xfrac); //i32erpolate in x direction (y+1)
        f32 fin = i32erpolate(v1,v2,Yfrac);  //i32erpolate in y direction

        return fin;
    }

    f32 PerlinNoise::i32erpolate(f32 x,f32 y,f32 a) const
    {
        f32 negA = 1.0 - a;
        f32 negASqr = negA * negA;
        f32 fac1 = 3.0 * (negASqr) - 2.0 * (negASqr * negA);
        f32 aSqr = a * a;
        f32 fac2 = 3.0 * aSqr - 2.0 * (aSqr * a);

        return x * fac1 + y * fac2; //add the weighted factors
    }

    f32 PerlinNoise::Noise(i32 x,i32 y) const
    {
        i32 n = x + y * 57;
        n = (n << 13) ^ n;
        i32 t = (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff;
        return 1.0 - f32(t) * 0.931322574615478515625e-9;/// 1073741824.0);
    }
};
