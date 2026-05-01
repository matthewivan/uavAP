//
// Created by mirco on 09.03.21.
//

#ifndef UAVAP_SINUSSIGNALGENERATOR_H
#define UAVAP_SINUSSIGNALGENERATOR_H

#include <cpsCore/cps_object>
#include "uavAP/FlightAnalysis/SignalGenerator/ISignalGenerator.h"
#include "uavAP/FlightAnalysis/SignalGenerator/PeriodicSignalParams.h"

class SineSignalGenerator
		: public ISignalGenerator,
		  public AggregatableObject<ITimeProvider>,
		  public ConfigurableObject<PeriodicSignalParams>
{
public:

	static constexpr auto typeId = "sine";

	void
	initialize() override;

	FloatingType
	getValue() override;

private:

	TimePoint init_;

};

#endif //UAVAP_SINUSSIGNALGENERATOR_H
