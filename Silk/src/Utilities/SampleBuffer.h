#pragma once
#include <Math/Math.h>
#include <vector>
using namespace std;

namespace Silk
{
    class SampleBuffer
    {
        public:
            SampleBuffer() : m_SampleCount(10.0f)  { }
            ~SampleBuffer() { }
        
            void SetSampleCount(i32 Count) { m_SampleCount = Count; }
            void Clear() { m_Samples.clear(); m_SamplesTotal = 0.0f; m_Max = FLT_MIN; }
            void AddSample(Scalar Sample);
            Scalar GetAverage() const { return m_SamplesTotal / Scalar(m_Samples.size()); }
            i32 GetSampleCount() const { return (i32)m_Samples.size(); }
            Scalar GetSample(i32 Idx) const { return m_Samples[Idx]; }
        
            vector<Scalar> m_Samples;
            Scalar m_SamplesTotal;
            Scalar m_SampleCount;
            Scalar m_Max;
    };
};

