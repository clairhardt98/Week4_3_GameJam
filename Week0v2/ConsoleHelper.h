#pragma once
#include <cstdint>
#include "Core/HAL/PlatformType.h"


//class FWindowsPlatformTime
//{
//public:
//    static double GSecondsPerCycle; // 0
//    static bool bInitialized; // false
//
//    static void InitTiming()
//    {
//        if (!bInitialized)
//        {
//            bInitialized = true;
//
//            double Frequency = (double)GetFrequency();
//            if (Frequency <= 0.0)
//            {
//                Frequency = 1.0;
//            }
//
//            GSecondsPerCycle = 1.0 / Frequency;
//        }
//    }
//    static float GetSecondsPerCycle()
//    {
//        if (!bInitialized)
//        {
//            InitTiming();
//        }
//        return (float)GSecondsPerCycle;
//    }
//    static uint64 GetFrequency()
//    {
//        LARGE_INTEGER Frequency;
//        QueryPerformanceFrequency(&Frequency);
//        return Frequency.QuadPart;
//    }
//    static double ToMilliseconds(uint64 CycleDiff)
//    {
//        double Ms = static_cast<double>(CycleDiff)
//            * GetSecondsPerCycle()
//            * 1000.0;
//
//        return Ms;
//    }
//
//    static uint64 Cycles64()
//    {
//        LARGE_INTEGER CycleCount;
//        QueryPerformanceCounter(&CycleCount);
//        return (uint64)CycleCount.QuadPart;
//    }
//};
//
//struct TStatId
//{
//};

//typedef FWindowsPlatformTime FPlatformTime;

//class FScopeCycleCounter
//{
//public:
//    // 기본 생성자 추가
//    FScopeCycleCounter()
//        : StartCycles(FPlatformTime::Cycles64())
//    {
//    }
//
//    FScopeCycleCounter(TStatId StatId)
//        : StartCycles(FPlatformTime::Cycles64())
//        , UsedStatId(StatId)
//    {
//    }
//
//    ~FScopeCycleCounter()
//    {
//        Finish();
//    }
//
//    uint64 Finish()
//    {
//        const uint64 EndCycles = FPlatformTime::Cycles64();
//        const uint64 CycleDiff = EndCycles - StartCycles;
//        return CycleDiff;
//    }
//
//private:
//    uint64 StartCycles;
//    TStatId UsedStatId;
//};
