//
// Created by mirco on 25.02.21.
//

#include <cpsCore/Framework/StaticFactory.h>
#include "uavAP/FlightAnalysis/ManeuverPlanner/Maneuver.h"
#include "uavAP/FlightAnalysis/Condition/ConditionFactory.h"
#include "uavAP/FlightAnalysis/SignalGenerator/SignalGeneratorFactory.h"
// !This has to be included such that every forward declaration in the conditions is complete for aggregation
#include "uavAP/FlightAnalysis/FlightAnalysisHelper.h"

bool
Maneuver::initialize(const Aggregator& aggregator)
{
	transition_ = ConditionFactory::create(params.transition());
	if (!transition_)
	{
		CPSLOG_WARN << "Transition cannot be created from configuration. Let's hope this is intentional.";
	}
	else
	{
		if (auto aggregatable = std::dynamic_pointer_cast<IAggregatableObject>(transition_))
			aggregatable->notifyAggregationOnUpdate(aggregator);
		transition_->initialize();
	}

	for (const auto& p : params.waveforms())
	{
		auto signalGenerator = SignalGeneratorFactory::create(p.second);
		if (!signalGenerator)
		{
			CPSLOG_ERROR << "signalGenerator cannot be created from configuration";
			return false;
		}
		if (auto aggregatable = std::dynamic_pointer_cast<IAggregatableObject>(signalGenerator))
			aggregatable->notifyAggregationOnUpdate(aggregator);
		signalGenerator->initialize();
		waveforms_.insert(std::make_pair(p.first, signalGenerator));
	}

	return true;
}

std::map<std::string, FloatingType>
Maneuver::getOverrides() const
{
	auto overrides = params.overrides();

	for (const auto&[id,waveform] : waveforms_)
	{
		overrides.insert(std::make_pair(id, waveform->getValue()));
		// overrides[id] = waveform->getValue(); // generated values replace static values for the same override id
	}

	return overrides;
}


bool
Maneuver::inTransition()
{
	if (!transition_)
		return false;
	return transition_->evaluate();
}

void
Maneuver::printInfo()
{
	std::cout << std::endl;
	std::cout << "Overrides: " << std::endl;
	for (const auto& it : params.overrides())
	{
		std::cout << it.first << ": " << it.second << std::endl;
	}
	std::cout << "Maintains: " << std::endl;
	for (const auto& it : params.maintains())
	{
		std::cout << it << std::endl;
	}
	if (params.advancedControl())
	{
		const auto& advanced = *params.advancedControl();
		std::cout << "Advanced Control: " << std::endl;
		std::cout << "throws_control: "
				  << EnumMap<ThrowsControl>::convert(advanced.throwsSelection()) << std::endl;
		std::cout << "camber_control: "
				  << EnumMap<CamberControl>::convert(advanced.camberSelection()) << std::endl;
		std::cout << "special_control: "
				  << EnumMap<SpecialControl>::convert(advanced.specialSelection()) << std::endl;
		std::cout << "camber_value: " << advanced.camberValue() << std::endl;
		std::cout << "special_value: " << advanced.specialValue() << std::endl;
	}

	std::cout << std::endl;
	if (!transition_)
		std::cout << "No transition" << std::endl;
	else
	{
		std::cout << "Transition:" << std::endl;
		transition_->printInfo();
	}
	std::cout << std::endl;

}

bool
Maneuver::isTimeVarying() const
{
	return !waveforms_.empty();
}

Maneuver::Maintains
Maneuver::getMaintains() const
{
	return params.maintains();
}

Optional<AdvancedControl>
Maneuver::getAdvancedControl() const
{
	return params.advancedControl();
}
