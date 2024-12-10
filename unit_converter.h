#ifndef UNIT_CONVERTER_H
#define UNIT_CONVERTER_H

#include <string>
#include <map>
#include <functional>

class UnitConverter {
private:
    std::map<std::string, std::function<double(double)>> conversionFunctions;

    // Private methods for registering each category of conversions
    void registerTemperatureConversions();
    void registerDistanceConversions();
    void registerWeightConversions();
    void registerVolumeConversions();

    // Central method to register all conversions
    void registerConversionFunctions();

public:
    UnitConverter();
    double convert(const std::string& conversionType, double value) const;
};

// Conversion utility functions
void convertTemperature(const UnitConverter& converter);
void convertDistance(const UnitConverter& converter);
void convertWeight(const UnitConverter& converter);
void convertVolume(const UnitConverter& converter);
void displayMenu();

#endif // UNIT_CONVERTER_H
