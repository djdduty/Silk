/*
 * Written by gamernb of stackoverflow.com
 * http://stackoverflow.com/users/314005/gamernb
 *
 * Aligned and modified slightly by Michael DeCicco of the Shire
 */

#pragma once
#include <System/SilkTypes.h>

namespace Silk
{
    class PerlinNoise
    {
        public:
            PerlinNoise();
            PerlinNoise(f32 Persistence,f32 Frequency,f32 Amplitude,i32 Octaves,i32 Seed);
        
            f32 GetNoise(f32 x,f32 y) const;

              f32 Persistence() const { return m_Persistence; }
              f32 Frequency()   const { return m_Frequency;   }
              f32 Amplitude()   const { return m_Amplitude;   }
              i32 Octaves()     const { return m_Octaves;     }
              i32 RandomSeed()  const { return m_Seed;  }

              void SetParameters(f32 Persistence,f32 Frequency,f32 Amplitude,i32 Octaves,i32 Randomseed);

              void SetPersistence(f32 Persistence) { m_Persistence = Persistence; }
              void SetFrequency  (f32 Frequency  ) { m_Frequency   = Frequency  ; }
              void SetAmplitude  (f32 Amplitude  ) { m_Amplitude   = Amplitude  ; }
              void SetOctaves    (i32 Octaves    ) { m_Octaves     = Octaves    ; }
              void SetRandomSeed (i32 Seed       ) { m_Seed        = Seed       ; }

        private:
            f32 Total(f32 i,f32 j) const;
            f32 GetValue(f32 x,f32 y) const;
            f32 i32erpolate(f32 x,f32 y,f32 a) const;
            f32 Noise(i32 x,i32 y) const;

            f32 m_Persistence;
            f32 m_Frequency;
            f32 m_Amplitude;
            i32 m_Octaves;
            i32 m_Seed;
    };
};

