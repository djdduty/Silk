#pragma once
#include <Math/Math.h>
#include <vector>
using namespace std;

namespace Silk
{
    class SampleBuffer
    {
        public:
			SampleBuffer() : m_SamplesTotal(0.0f), m_SampleCount(10.0f), m_Max(FLT_MIN) { }
            ~SampleBuffer() { }
        
            void SetSampleCount(i32 Count) { m_SampleCount = Count; }
            void Clear() { m_Samples.clear(); m_SamplesTotal = 0.0f; m_Max = FLT_MIN; }
            void AddSample(Scalar Sample);
            
            Scalar GetAverage() const { if(m_Samples.size() == 0) { return 0.0f; } return m_SamplesTotal / Scalar(m_Samples.size()); }
            Scalar GetMin() const;
            Scalar GetMax() const { return m_Max; }
            i32 GetSampleCount() const { return (i32)m_Samples.size(); }
            Scalar GetSample(i32 Idx) const { return m_Samples[Idx]; }
        
            vector<Scalar> m_Samples;
            Scalar m_SamplesTotal;
            Scalar m_SampleCount;
            Scalar m_Max;
    };
};

