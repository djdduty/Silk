#include <Utilities/SampleBuffer.h>

namespace Silk
{
    void SampleBuffer::AddSample(Scalar Sample)
    {
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
};
