#pragma once

#include <string>

namespace HoloJs {
class PerformaceTracker {
   public:
    PerformaceTracker(PCWSTR metricName, unsigned int threshold) : m_metricName(metricName), m_threshold(threshold)
    {
        QueryPerformanceFrequency(&m_timerFrequencyPerMs);
        m_timerFrequencyPerMs.QuadPart /= 1000;
    }

    inline void start()
    {
#ifdef _DEBUG
        QueryPerformanceCounter(&m_startTimestamp);
#endif
    }

    inline void end()
    {
#ifdef _DEBUG
        if (m_startTimestamp.QuadPart == 0) {
            return;
        }

        LARGE_INTEGER endTimestamp;
        QueryPerformanceCounter(&endTimestamp);

        const auto elapsed = (endTimestamp.QuadPart - m_startTimestamp.QuadPart) / m_timerFrequencyPerMs.QuadPart;

        if (elapsed > m_threshold) {
            m_message = m_metricName;
            m_message.append(L" : ");
            m_message.append(std::to_wstring(elapsed));
            m_message.append(L"\r\n");

            OutputDebugString(m_message.c_str());
        }
#endif
    }

   private:
    LARGE_INTEGER m_startTimestamp;
    LARGE_INTEGER m_timerFrequencyPerMs;

    std::wstring m_metricName;
    std::wstring m_message;
    unsigned int m_threshold;
};
}  // namespace HoloJs
