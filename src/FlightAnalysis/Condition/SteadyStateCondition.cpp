/*
 * SteadyStateCondition.cpp
 *
 *  Created on: Feb 26, 2021
 *      Author: Mirco Theile
 */
#include <cpsCore/Utilities/TimeProvider/ITimeProvider.h>
#include <uavAP/FlightAnalysis/Condition/Relational.h>
#include <iostream>

#include "uavAP/FlightAnalysis/Condition/SteadyStateCondition.h"
#include "uavAP/FlightControl/SensingActuationIO/ISensingIO.h"


void
SteadyStateCondition::initialize()
{
	if (!checkIsSetAll())
	{
		CPSLOG_ERROR << "Something is missing.";
		return;
	}

	auto tp = get<ITimeProvider>();
	firstSteadyState_ = tp->now();

}

bool
SteadyStateCondition::evaluate()
{
	auto sens = get<ISensingIO>();
	auto tp = get<ITimeProvider>();

	auto sd = sens->getSensorData();

	bool inSteadyState = true;
	for (const auto& [sensor, tolerance]: params.sensorValues())
	{
		auto val = enumAccess<FloatingType>(sd, sensor);
		auto it = lastReadings_.find(sensor);
		if (it == lastReadings_.end())
		{
			//Last reading not recorded
			lastReadings_.insert(std::make_pair(sensor, val));
			inSteadyState = false;
			continue;
		}

		if (!evaluateRelationalTolerance<FloatingType, Relational::EQUAL>(val, it->second, tolerance))
		{
			it->second = val;
			inSteadyState = false;
		}

	}
	if (!inSteadyState)
	{
		firstSteadyState_ = tp->now();
	}

	return std::chrono::duration_cast<Milliseconds>(tp->now() - firstSteadyState_).count() >= params.duration();
}

void
SteadyStateCondition::printInfo()
{
	std::cout << "Steady State Condition: " << params.duration() << " ms" << std::endl;
	std::cout << "Sensor tolerances:" << std::endl;
	for (const auto& [sensor, tolerance] : params.sensorValues())
		std::cout << EnumMap<SensorEnum>::convert(sensor) << ": " << tolerance << std::endl;
}
