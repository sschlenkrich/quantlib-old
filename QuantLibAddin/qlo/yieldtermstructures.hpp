/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2005, 2006, 2007 Eric Ehlers
 Copyright (C) 2006, 2007, 2009 Ferdinando Ametrano
 Copyright (C) 2005 Plamen Neykov
 Copyright (C) 2005 Aurelien Chanudet

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it
 under the terms of the QuantLib license.  You should have received a
 copy of the license along with this program; if not, please email
 <quantlib-dev@lists.sf.net>. The license is also available online at
 <http://quantlib.org/license.shtml>.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/

#ifndef qla_yieldtermstructures_hpp
#define qla_yieldtermstructures_hpp

#include <qlo/termstructures.hpp>
#include <qlo/optimization.hpp>


#include <ql/time/frequency.hpp>
#include <ql/compounding.hpp>
#include <ql/types.hpp>
#include <ql/math/optimization/method.hpp>
#include <ql/termstructures/yield/bondhelpers.hpp>

namespace QuantLib {
    class Calendar;
    class DayCounter;
    class Date;
    class Quote;
	class BlackVolTermStructure;

    template<class TS>
    class BootstrapHelper;

    typedef BootstrapHelper<YieldTermStructure> RateHelper;

    template <class T>
    class Handle;

}

namespace QuantLibAddin {
     
    class DiscountCurve : public YieldTermStructure {
      public:
        DiscountCurve(
            const boost::shared_ptr<ObjectHandler::ValueObject>& properties,
            const std::vector<QuantLib::Date>& dates,
            const std::vector<QuantLib::DiscountFactor>& dfs,
            const QuantLib::DayCounter& dayCounter,
            bool permanent);
    };

    class ZeroCurve : public YieldTermStructure {
      public:
        ZeroCurve(const boost::shared_ptr<ObjectHandler::ValueObject>& properties,
                  const std::vector<QuantLib::Date>& dates,
                  const std::vector<QuantLib::Rate>& zeroRates,
                  const QuantLib::DayCounter& dayCounter,
                  bool permanent);
    };

    class ForwardCurve : public YieldTermStructure {
      public:
        ForwardCurve(const boost::shared_ptr<ObjectHandler::ValueObject>& properties,
                     const std::vector<QuantLib::Date>& dates,
                     const std::vector<QuantLib::Rate>& forwardRates,
                     const QuantLib::DayCounter& dayCounter,
                     bool permanent);
    };

    class FlatForward : public YieldTermStructure {
      public:
        FlatForward(const boost::shared_ptr<ObjectHandler::ValueObject>& properties,
                    QuantLib::Natural nDays,
                    const QuantLib::Calendar& calendar,
                    const QuantLib::Handle<QuantLib::Quote>& forward,
                    const QuantLib::DayCounter& dayCounter,
                    QuantLib::Compounding compounding,
                    QuantLib::Frequency frequency,
                    bool permanent);
    };

    class ForwardSpreadedTermStructure : public YieldTermStructure {
      public:
        ForwardSpreadedTermStructure(
            const boost::shared_ptr<ObjectHandler::ValueObject>& properties,
            const QuantLib::Handle<QuantLib::YieldTermStructure>& hYTS,
            const QuantLib::Handle<QuantLib::Quote>& spread,
            bool permanent);
    };

    class ImpliedTermStructure : public YieldTermStructure {
      public:
        ImpliedTermStructure(
            const boost::shared_ptr<ObjectHandler::ValueObject>& properties,
            const QuantLib::Handle<QuantLib::YieldTermStructure>& hYTS,
            const QuantLib::Date& referenceDate,
            bool permanent);
    };

    class InterpolatedYieldCurve : public YieldTermStructure {
      public:
        // placeholders for QuantLib types of the same name.
        enum Traits { Discount,
                      ForwardRate,
                      ZeroYield
        };
        enum Interpolator { BackwardFlat,
                            ForwardFlat,
                            Linear,
                            LogLinear,
                            CubicNaturalSpline,
                            LogCubicNaturalSpline,
                            MonotonicCubicNaturalSpline,
                            MonotonicLogCubicNaturalSpline,
                            KrugerCubic,
                            KrugerLogCubic,
                            FritschButlandCubic,
                            FritschButlandLogCubic,
                            Parabolic,
                            LogParabolic,
                            MonotonicParabolic,
                            MonotonicLogParabolic
        };
        InterpolatedYieldCurve(
            const boost::shared_ptr<ObjectHandler::ValueObject>& properties,
            const std::vector<QuantLib::Date>& dates,
            const std::vector<QuantLib::Real>& data,
            const QuantLib::Calendar& calendar,
            const QuantLib::DayCounter& dayCounter,
            const std::vector<QuantLib::Handle<QuantLib::Quote> >& jumps,
            const std::vector<QuantLib::Date>& jumpDates,
            const std::string& traitsID,
            const std::string& interpolatorID,
            bool permanent);
        const std::vector<QuantLib::Time>& times() const;
        const std::vector<QuantLib::Date>& dates() const;
        const std::vector<QuantLib::Real>& data() const;
        const std::vector<QuantLib::Time>& jumpTimes() const;
        const std::vector<QuantLib::Date>& jumpDates() const;
      protected:
        InterpolatedYieldCurve(
            const boost::shared_ptr<ObjectHandler::ValueObject>& properties,
            const std::string& traitsID,
            const std::string& interpolatorID,
            bool permanent);
        std::string traitsID_, interpolatorID_;
    };


    // A pair indicating a combination of Traits / Interpolator.
    typedef std::pair<InterpolatedYieldCurve::Traits, InterpolatedYieldCurve::Interpolator> InterpolatedYieldCurvePair;

    // Stream operator to write a InterpolatedYieldCurvePair to a stream - for logging / error handling.
    std::ostream &operator<<(std::ostream &out,
                             InterpolatedYieldCurvePair tokenPair);

	class FittedBondDiscountCurve : public YieldTermStructure {
	public:
		FittedBondDiscountCurve(
			const boost::shared_ptr<ObjectHandler::ValueObject>&         properties,
			const QuantLib::Natural                                      settlementDays,
			const QuantLib::Calendar&                                    calendar,
			const std::vector<boost::shared_ptr<QuantLib::BondHelper> >& bondHelpers,
			const QuantLib::DayCounter&                                  dayCounter,
			const std::string&                                           fittingMethodID,
			const std::vector<QuantLib::Real>&                           weights,              // QuantLib::Array(),
			const boost::shared_ptr<QuantLib::OptimizationMethod>&       optimizationMethod,   // boost::shared_ptr<QuantLib::OptimizationMethod>(),
			const QuantLib::Real                                         accuracy,             // 1.0e-10,
			const QuantLib::Size                                         maxEvaluations,       // 10000,
			const std::vector<QuantLib::Real>&                           guess,                // QuantLib::Array(),
			const QuantLib::Real                                         simplexLambda,        // 1.0 (legacy),
			const QuantLib::Size                                         maxStationaryStateIterations, // 100,
			const QuantLib::Handle<QuantLib::YieldTermStructure>&        baseCurve,            // QuantLib::Handle<QuantLib::YieldTermStructure>(),
			const std::vector<QuantLib::Time>&                           knots,
			bool                                                         permanent);
	};

	class QuantoTermStructure : public YieldTermStructure {
	public:
		QuantoTermStructure(
			const boost::shared_ptr<ObjectHandler::ValueObject>&        properties,
			const QuantLib::Handle<QuantLib::YieldTermStructure>&       underlyingDividendTS,
			const QuantLib::Handle<QuantLib::YieldTermStructure>&       riskFreeTS,
			const QuantLib::Handle<QuantLib::YieldTermStructure>&       foreignRiskFreeTS,
			const QuantLib::Handle<QuantLib::BlackVolTermStructure>&    underlyingBlackVolTS,
			const QuantLib::Real                                        underlyingStrike,
			const QuantLib::Handle<QuantLib::BlackVolTermStructure>&    exchRateBlackVolTS,
			const QuantLib::Real                                        exchRateATMlevel,
			const QuantLib::Real                                        underlyingExchRateCorrelation,
			bool permanent);
	};

}

#endif
