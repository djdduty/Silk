#include <Utilities/SampleBuffer.h>
#include <math.h>

namespace Silk
{
    void SampleBuffer::AddSample(Scalar Sample)
    {
        if(isnan(Sample) || isinf(Sample)) return;
        
        m_Samples.push_back(Sample);
        m_SamplesTotal += Sample;
        if(m_Samples.size() > m_SampleCount)
        {
            int Dif = (m_Samples.size() - m_SampleCount);
            for(i32 i = 0;i < Dif;i++) m_SamplesTotal -= m_Samples[i];
            m_Samples.erase(m_Samples.begin(),m_Samples.begin() + Dif);
            m_Max = FLT_MIN;
            for(i32 i = 0;i < m_Samples.size();i++)
            {
                Scalar s = m_Samples[i];
                if(s > m_Max) m_Max = s;
            }
        }
        else if(m_Max < Sample) m_Max = Sample;
    }
    Scalar SampleBuffer::GetMin() const
    {
        if(m_Samples.size() == 0) return 0.0f;
        
        Scalar Min = m_Samples[0];
        for(i32 i = 1;i < m_Samples.size();i++)
        {
            if(m_Samples[i] < Min) Min = m_Samples[i];
        }
        return Min;
    }
};
