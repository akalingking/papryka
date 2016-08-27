
// @reference <https://www.elitetrader.com/et/threads/gbm-source-code-in-c-for-generating-time-series.297370/>
/**
GBM.cpp - Geom. Brownian Motion (GBM)
2016-01-23-Sa: v0.99: initial version
2016-01-26-Tu: v1.00: now t-distribution can be activated via the last param in ctor
Author: U.M. in Germany (user botpro at www.elitetrader.com)

What-it-does:
   Create timeseries data using Geom. Brownian Motion (GBM).
   Can generate bars of any size in time. By default it generates 30-sec bars (ie. 780 bars/day @ 23400 seconds/day).
   You can modify it easily to create OHLC-data (intraday and EOD) to be used in trading platforms like AmiBroker etc.

Compile using a C++11 conformant compiler like GNU g++:
   g++ -Wall -O2 -std=gnu++11 GBM.cpp -o GBM.exe
   
Run:
   Linux/Unix: ./GBM.exe >data.csv
   Windows:    GBM.exe >data.csv
   
Analyse:
   Import data.csv into Excel or LibreOffice-Calc and do some analysis (calcs, charts etc).

   Remember: the stock returns (ie. the logarithmic changes) are normally distributed, but the resulting timeseries
   is log-normally distributed because there are no negative stock prices.

   By default it uses the normal-distribution. t-distribution can be used optionally (see last param of the ctor).
   Using the default normal-distribution is the stochastically correct method for research.
   For the difference see [2], sections "Normally Distributed Model of Asset Returns" and "Leptokurtic Model of Asset Returns".

   The quality of the generated data can be verified with the following formula:
     ObservedAnnualVolaPct = BarVolaPct * sqrt(252 * nBarsPerDay)
   ie. in Excel/LibreOffice-Calc for the sample data the pgm creates do this:
     =STDEV(D2:D16381)*100*SQRT(252*780)
   It should give approximately the same VolaPct as was specified as the input volatility (ie. here 30).
   
Misc:
   - You can modify the code easily to create OHLC-data (intraday and EOD) to be used in trading platforms like AmiBroker etc.
   - It works with trading days instead of calendar days, and a year is defined as 252 trading days (can be chgd in ctor)
   - This code is a stripped down standalone usable version of my TCIntradaySpotGenerator
   
See also / References:
   [1] https://en.wikipedia.org/wiki/Geometric_Brownian_motion
   [2] https://mhittesdorf.wordpress.com/2013/12/29/introducing-quantlib-modeling-asset-prices-with-geometric-brownian-motion/
   [3] https://people.sc.fsu.edu/~jburkardt/cpp_src/brownian_motion_simulation/brownian_motion_simulation.html
   [4] http://www.javaquant.net/books/MCBook-1.2.pdf
   [5] http://investexcel.net/geometric-brownian-motion-excel
   [6] https://en.wikipedia.org/wiki/Volatility_(finance)
 */
#include <papryka/detail/types.h>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <random>
#include <chrono>
#include <cassert>

namespace {
    std::default_random_engine randgen(std::chrono::system_clock::now().time_since_epoch().count());
    std::normal_distribution<double> n_dist(0.0, 1.0);  // mu=0, s=1
    std::student_t_distribution<double> t_dist(5);      // 5 degrees of freedom
}

template <typename _T=double>
class GBM
{
public:
    typedef _T value_t;
    const size_t uDays, uDailyBars;
    const double dbSpot0, dbAnnDriftPct, dbAnnDividPct, dbAnnVolaPct, dbTradeDaysInYear;
    const bool fUseTdistribution;

private:
    // stochastic differential equation parameters
    double r, q, u, t, dt, sigma, SD, R;
    // data values
    value_t S_t, S_tPrev;
    // n-values generated    
    size_t cGen;        

public:
    /**
     * @brief GBM
     * @param AdbSpot0              starting price
     * @param AdbAnnVolaPct         volatility
     * @param AuDailyBars           def:780 is 30sec intraday bas
     * @param AdbAnnDriftPct        annual return
     * @param AdbAnnDividPct        annual dividend
     * @param AuDays                trading days
     * @param AdbTradeDaysInYear    fixed annual trading days
     * @param AfUseTdistribution    distribution type
     */
    GBM(const value_t& AdbSpot0, const double AdbAnnVolaPct = 0.30, const double AdbAnnDriftPct = 0.0, 
            const double AdbAnnDividPct = 0.0,  const size_t AuDailyBars = 780, const size_t AuDays = 252, 
            const double AdbTradeDaysInYear = 252.0, const bool AfUseTdistribution = false)
            : uDays(AuDays), uDailyBars(AuDailyBars), dbSpot0(AdbSpot0),
            dbAnnDriftPct(AdbAnnDriftPct), dbAnnDividPct(AdbAnnDividPct),
            dbAnnVolaPct(AdbAnnVolaPct), dbTradeDaysInYear(AdbTradeDaysInYear),
            fUseTdistribution(AfUseTdistribution)
    {
        assert(dbAnnDriftPct <= 1.0);
        r = dbAnnDriftPct; // average annual return
        assert(dbAnnDividPct <= 1.0);
        q = dbAnnDividPct; //  dividend yield
        u = r - q; // percentage drift, this is used as the mean return
        
        t = double(uDays) / dbTradeDaysInYear; // time steps
        dt = t / double(uDays * uDailyBars);
        assert(AdbAnnVolaPct <= 1.0);
        sigma = AdbAnnVolaPct;
        if (fUseTdistribution)
            sigma = sqrt(sigma * sigma * 3 / 5); // sigma scaled by reciprocal of Student T variance (v/(v-2))
        SD = sigma * sqrt(dt);
        R = (u - 0.5 * sigma * sigma) * dt; // Ito's lemma
        S_t = S_tPrev = dbSpot0;
        cGen = 0;
    }

    value_t generate()
    { 
        // convention: the very first spot is the initial spot
        S_tPrev = S_t;
        if (!cGen++) 
            return S_t;
        if (!fUseTdistribution)
            S_t *= exp(R + SD * n_dist(randgen)); // normal distribution (Gauss)
        else
            S_t *= exp(R + SD * t_dist(randgen)); // t-distribution ("fat tails")
        return S_t;
    }

    value_t get_cur() const { return S_t; }

    value_t get_prev() const { return S_tPrev; }
};

//int main()
//{
//    // define the input params to use in GBM:
//    const double dbSpot0 = 100; // start with this stock price
//    const double dbVolaPct = 30; // historic volatility
//    const size_t nBarsPerDay = 780; // ie. 30-sec bars @ 23400 trading seconds per day
//
//    GBM G(dbSpot0, dbVolaPct, nBarsPerDay, 0.0, 0.0, 252, 252.0, false);
//
//    // fprintf(stderr, "Using: %s\n", G.fUseTdistribution ? "t-distribution" : "normal-distribution");
//    // create bar data for 21 days (= 1 trading month) and print as CSV:
//    printf("Day,Bar,Spot,lnOfChg\n");
//    for (size_t d = 1; d <= 21; ++d)
//        for (size_t b = 1; b <= nBarsPerDay; ++b)
//        {
//            const double dbCur = G.generate();
//            const double dbPrev = G.get_prev();
//            printf("%zu,%zu,%.5f,%.10f\n", d, b, dbCur, log(dbCur / dbPrev));
//        }
//
//    return 0;
//}