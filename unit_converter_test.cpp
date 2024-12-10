#include <deepstate/DeepState.hpp>
#include <cmath>
#include <cstring> // for strcmp
#include <limits>
#include <sstream> // for std::istringstream
#include <vector>
#include <iostream> // Added to ensure ::std::cin is defined
#include "unit_converter.h"

using namespace deepstate;

// Define a custom ASSERT_NEAR macro for DeepState
#define ASSERT_NEAR(val1, val2, tol) \
  DeepState_Assert(std::fabs((val1) - (val2)) <= (tol))

TEST(UnitConverter, ValidConversions) {
    UnitConverter converter;

    // Temperature
    ASSERT_EQ(converter.convert("CelsiusToFahrenheit", 0.0), 32.0);
    ASSERT_EQ(converter.convert("FahrenheitToCelsius", 32.0), 0.0);
    ASSERT_EQ(converter.convert("CelsiusToKelvin", 0.0), 273.15);
    ASSERT_NEAR(converter.convert("KelvinToCelsius", 273.15), 0.0, 1e-9);

    // Distance
    ASSERT_EQ(converter.convert("KilometersToMiles", 1.0), 0.621371);
    ASSERT_NEAR(converter.convert("MilesToKilometers", 0.621371), 1.0, 1e-9);
    ASSERT_NEAR(converter.convert("MetersToFeet", 1.0), 3.28084, 1e-5);
    ASSERT_NEAR(converter.convert("FeetToMeters", 3.28084), 1.0, 1e-5);

    // Weight
    ASSERT_NEAR(converter.convert("KilogramsToPounds", 1.0), 2.20462, 1e-5);
    ASSERT_NEAR(converter.convert("PoundsToKilograms", 2.20462), 1.0, 1e-5);
    ASSERT_NEAR(converter.convert("GramsToOunces", 100.0), 3.5274, 1e-4);
    ASSERT_NEAR(converter.convert("OuncesToGrams", 3.5274), 100.0, 1e-2);

    // Volume
    ASSERT_NEAR(converter.convert("LitersToGallons", 1.0), 0.264172, 1e-6);
    ASSERT_NEAR(converter.convert("GallonsToLiters", 1.0), 3.78541178, 1e-6);
    ASSERT_NEAR(converter.convert("MillilitersToFluidOunces", 100.0), 3.3814, 1e-4);
    ASSERT_NEAR(converter.convert("FluidOuncesToMilliliters", 3.3814), 100.0, 1e-1);
}

TEST(UnitConverter, ZeroAndNearZeroValues) {
    UnitConverter converter;

    // Zero values
    ASSERT_EQ(converter.convert("CelsiusToFahrenheit", 0.0), 32.0);
    ASSERT_EQ(converter.convert("KilogramsToPounds", 0.0), 0.0);
    ASSERT_EQ(converter.convert("LitersToGallons", 0.0), 0.0);

    // Very small positive values
    ASSERT_NEAR(converter.convert("CelsiusToKelvin", 1e-9), 273.150000001, 1e-9);
    ASSERT_NEAR(converter.convert("MetersToFeet", 1e-9), 3.28084e-9, 1e-15);
    ASSERT_NEAR(converter.convert("MillilitersToFluidOunces", 1e-9), 3.3814e-11, 1e-17);
}

TEST(UnitConverter, JustAboveAbsoluteZero) {
    UnitConverter converter;
    double result = converter.convert("CelsiusToFahrenheit", -273.14);
    ASSERT(!std::isnan(result));
}

TEST(UnitConverter, InvalidInputs) {
    UnitConverter converter;

    // Below absolute zero
    try {
        converter.convert("CelsiusToKelvin", -300.0);
        DeepState_Fail();
    } catch (const std::invalid_argument& e) {
        DeepState_Assert(strcmp(e.what(), "Temperature value below absolute zero is not valid.") == 0);
    }

    // Negative distance
    try {
        converter.convert("KilometersToMiles", -10.0);
        DeepState_Fail();
    } catch (const std::invalid_argument& e) {
        DeepState_Assert(strcmp(e.what(), "Negative distance values are not valid.") == 0);
    }

    // Negative weight
    try {
        converter.convert("KilogramsToPounds", -5.0);
        DeepState_Fail();
    } catch (const std::invalid_argument& e) {
        DeepState_Assert(strcmp(e.what(), "Negative weight values are not valid.") == 0);
    }

    // Negative volume
    try {
        converter.convert("LitersToGallons", -1.0);
        DeepState_Fail();
    } catch (const std::invalid_argument& e) {
        DeepState_Assert(strcmp(e.what(), "Negative volume values are not valid.") == 0);
    }
}

TEST(UnitConverter, ClampingValues) {
    UnitConverter converter;

    // Large positive temperature
    {
        double result = converter.convert("CelsiusToFahrenheit", 1e7);
        double expected = converter.convert("CelsiusToFahrenheit", 1e6);
        ASSERT_EQ(result, expected);
    }

    // Distance clamping
    {
        double result = converter.convert("KilometersToMiles", 1e8);
        double expected = converter.convert("KilometersToMiles", 1e6);
        ASSERT_EQ(result, expected);
    }

    // Volume clamping
    {
        double result = converter.convert("LitersToGallons", 1e9);
        double expected = converter.convert("LitersToGallons", 1e6);
        ASSERT_EQ(result, expected);
    }

    // Valid non-clamped value
    try {
        double result = converter.convert("CelsiusToFahrenheit", 100.0);
        ASSERT_EQ(result, 212.0);
    } catch (...) {
        DeepState_Fail();
    }
}

TEST(UnitConverter, InvalidConversionType) {
    UnitConverter converter;

    try {
        converter.convert("InvalidType", 100.0);
        DeepState_Fail();
    } catch (const std::invalid_argument& e) {
        DeepState_Assert(strcmp(e.what(), "Invalid conversion type: InvalidType") == 0);
    }
}

TEST(UnitConverter, InvalidConversionTypeInCategories) {
    UnitConverter converter;

    // Invalid temperature conversion
    try {
        converter.convert("KelvinToFahrenheits", 300.0);
        DeepState_Fail();
    } catch (const std::invalid_argument& e) {
        ASSERT(strstr(e.what(), "Invalid conversion type") != nullptr);
    }

    // Invalid distance conversion
    try {
        converter.convert("MetersToYards", 10.0);
        DeepState_Fail();
    } catch (const std::invalid_argument& e) {
        ASSERT(strstr(e.what(), "Invalid conversion type") != nullptr);
    }

    // Invalid weight conversion
    try {
        converter.convert("KilogramsToStones", 10.0);
        DeepState_Fail();
    } catch (const std::invalid_argument& e) {
        ASSERT(strstr(e.what(), "Invalid conversion type") != nullptr);
    }

    // Invalid volume conversion
    try {
        converter.convert("LitersToCups", 1.0);
        DeepState_Fail();
    } catch (const std::invalid_argument& e) {
        ASSERT(strstr(e.what(), "Invalid conversion type") != nullptr);
    }
}

TEST(UnitConverter, FuzzTestAllCategories) {
    UnitConverter converter;

    static const char* const allConversions[] = {
        // Temperature
        "CelsiusToFahrenheit", "FahrenheitToCelsius", "CelsiusToKelvin", "KelvinToCelsius",
        // Distance
        "KilometersToMiles", "MilesToKilometers", "MetersToFeet", "FeetToMeters",
        // Weight
        "KilogramsToPounds", "PoundsToKilograms", "GramsToOunces", "OuncesToGrams",
        // Volume
        "LitersToGallons", "GallonsToLiters", "MillilitersToFluidOunces", "FluidOuncesToMilliliters"
    };

    for (auto &type : allConversions) {
        try {
            double value = DeepState_Double();
            double result = converter.convert(type, value);
            ASSERT(!std::isnan(result));
            ASSERT(!std::isinf(result));
        } catch (const std::invalid_argument& e) {
            ASSERT(strlen(e.what()) > 0);
        }
    }
}

TEST(UnitConverter, InteractiveFunctions) {
    UnitConverter converter;

    // convertTemperature with valid input
    {
        std::string input_data = "100\n1\n"; 
        std::istringstream input_stream(input_data);
        std::streambuf* orig_cin = ::std::cin.rdbuf(input_stream.rdbuf());
        convertTemperature(converter); 
        ::std::cin.rdbuf(orig_cin);
    }

    // convertTemperature with invalid double input
    {
        std::string input_data = "abc\n";
        std::istringstream input_stream(input_data);
        std::streambuf* orig_cin = ::std::cin.rdbuf(input_stream.rdbuf());
        convertTemperature(converter); 
        ::std::cin.rdbuf(orig_cin);
    }

    // convertTemperature with invalid menu choice
    {
        std::string input_data = "100\n99\n"; 
        std::istringstream input_stream(input_data);
        std::streambuf* orig_cin = ::std::cin.rdbuf(input_stream.rdbuf());
        convertTemperature(converter); 
        ::std::cin.rdbuf(orig_cin);
    }

    // convertDistance with valid input
    {
        std::string input_data = "10\n1\n"; 
        std::istringstream input_stream(input_data);
        std::streambuf* orig_cin = ::std::cin.rdbuf(input_stream.rdbuf());
        convertDistance(converter); 
        ::std::cin.rdbuf(orig_cin);
    }

    // convertDistance invalid double
    {
        std::string input_data = "abc\n"; 
        std::istringstream input_stream(input_data);
        std::streambuf* orig_cin = ::std::cin.rdbuf(input_stream.rdbuf());
        convertDistance(converter);
        ::std::cin.rdbuf(orig_cin);
    }

    // convertDistance invalid menu choice
    {
        std::string input_data = "10\n99\n";
        std::istringstream input_stream(input_data);
        std::streambuf* orig_cin = ::std::cin.rdbuf(input_stream.rdbuf());
        convertDistance(converter);
        ::std::cin.rdbuf(orig_cin);
    }

    // convertWeight with valid input
    {
        std::string input_data = "10\n1\n"; 
        std::istringstream input_stream(input_data);
        std::streambuf* orig_cin = ::std::cin.rdbuf(input_stream.rdbuf());
        convertWeight(converter); 
        ::std::cin.rdbuf(orig_cin);
    }

    // convertWeight invalid double
    {
        std::string input_data = "abc\n";
        std::istringstream input_stream(input_data);
        std::streambuf* orig_cin = ::std::cin.rdbuf(input_stream.rdbuf());
        convertWeight(converter);
        ::std::cin.rdbuf(orig_cin);
    }

    // convertWeight invalid menu choice
    {
        std::string input_data = "10\n99\n";
        std::istringstream input_stream(input_data);
        std::streambuf* orig_cin = ::std::cin.rdbuf(input_stream.rdbuf());
        convertWeight(converter);
        ::std::cin.rdbuf(orig_cin);
    }

    // convertVolume with valid input
    {
        std::string input_data = "10\n1\n";
        std::istringstream input_stream(input_data);
        std::streambuf* orig_cin = ::std::cin.rdbuf(input_stream.rdbuf());
        convertVolume(converter);
        ::std::cin.rdbuf(orig_cin);
    }

    // convertVolume invalid double
    {
        std::string input_data = "abc\n";
        std::istringstream input_stream(input_data);
        std::streambuf* orig_cin = ::std::cin.rdbuf(input_stream.rdbuf());
        convertVolume(converter);
        ::std::cin.rdbuf(orig_cin);
    }

    // convertVolume invalid menu choice
    {
        std::string input_data = "10\n99\n";
        std::istringstream input_stream(input_data);
        std::streambuf* orig_cin = ::std::cin.rdbuf(input_stream.rdbuf());
        convertVolume(converter);
        ::std::cin.rdbuf(orig_cin);
    }
}
