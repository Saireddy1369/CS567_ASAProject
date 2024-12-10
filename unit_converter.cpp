#include "unit_converter.h"
#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <limits>    // for std::numeric_limits
#include <cctype>    // for std::isdigit
#include <string>
#include <algorithm> // for std::find, std::transform

// Registers temperature conversions
void UnitConverter::registerTemperatureConversions() {
    conversionFunctions["CelsiusToFahrenheit"] = [](double c) { return (c * 9.0 / 5.0) + 32.0; };
    conversionFunctions["FahrenheitToCelsius"] = [](double f) { return (f - 32.0) * 5.0 / 9.0; };
    conversionFunctions["CelsiusToKelvin"] = [](double c) { return c + 273.15; };
    conversionFunctions["KelvinToCelsius"] = [](double k) { return k - 273.15; };
}

// Registers distance conversions
void UnitConverter::registerDistanceConversions() {
    conversionFunctions["KilometersToMiles"] = [](double km) { return km * 0.621371; };
    conversionFunctions["MilesToKilometers"] = [](double miles) { return miles / 0.621371; };
    conversionFunctions["MetersToFeet"] = [](double m) { return m * 3.28084; };
    conversionFunctions["FeetToMeters"] = [](double ft) { return ft / 3.28084; };
}

// Registers weight conversions
void UnitConverter::registerWeightConversions() {
    conversionFunctions["KilogramsToPounds"] = [](double kg) { return kg * 2.20462; };
    conversionFunctions["PoundsToKilograms"] = [](double lb) { return lb / 2.20462; };
    conversionFunctions["GramsToOunces"] = [](double g) { return g * 0.035274; };
    conversionFunctions["OuncesToGrams"] = [](double oz) { return oz / 0.035274; };
}

// Registers volume conversions
void UnitConverter::registerVolumeConversions() {
    conversionFunctions["LitersToGallons"] = [](double l) { return l * 0.264172; };
    conversionFunctions["GallonsToLiters"] = [](double gal) { return gal / 0.264172; };
    conversionFunctions["MillilitersToFluidOunces"] = [](double ml) { return ml * 0.033814; };
    conversionFunctions["FluidOuncesToMilliliters"] = [](double fl_oz) { return fl_oz / 0.033814; };
}

// Central registration of all conversions
void UnitConverter::registerConversionFunctions() {
    registerTemperatureConversions();
    registerDistanceConversions();
    registerWeightConversions();
    registerVolumeConversions();
}

UnitConverter::UnitConverter() {
    registerConversionFunctions();
}

double UnitConverter::convert(const std::string& conversionType, double value) const {
    // Validate input ranges for certain categories
    if (conversionType.find("Celsius") != std::string::npos || conversionType.find("Fahrenheit") != std::string::npos || conversionType.find("Kelvin") != std::string::npos) {
        // Check not below absolute zero
        double cVal = (conversionType.find("Fahrenheit") != std::string::npos) ?
                        ((value - 32.0) * 5.0 / 9.0) :
                        ((conversionType.find("Kelvin") != std::string::npos) ?
                            (value - 273.15) : value);
        if (cVal < -273.15) {
            throw std::invalid_argument("Temperature value below absolute zero is not valid.");
        }
    }

    // Distance should not be negative
    if (conversionType.find("Kilometers") != std::string::npos || conversionType.find("Miles") != std::string::npos ||
        conversionType.find("Meters") != std::string::npos || conversionType.find("Feet") != std::string::npos) {
        if (value < 0) throw std::invalid_argument("Negative distance values are not valid.");
    }

    // Weight should not be negative
    if (conversionType.find("Kilograms") != std::string::npos || conversionType.find("Pounds") != std::string::npos ||
        conversionType.find("Grams") != std::string::npos || conversionType.find("Ounces") != std::string::npos) {
        if (value < 0) throw std::invalid_argument("Negative weight values are not valid.");
    }

    // Volume should not be negative
    if (conversionType.find("Liters") != std::string::npos || conversionType.find("Gallons") != std::string::npos ||
        conversionType.find("Milliliters") != std::string::npos || conversionType.find("FluidOunces") != std::string::npos) {
        if (value < 0) throw std::invalid_argument("Negative volume values are not valid.");
    }

    // Clamp excessively large values
    if (value > 1e6) value = 1e6;
    else if (value < -1e6) value = -1e6;

    auto it = conversionFunctions.find(conversionType);
    if (it != conversionFunctions.end()) {
        return it->second(value);
    } else {
        throw std::invalid_argument("Invalid conversion type: " + conversionType);
    }
}

// Helper function to safely read a double value
bool safeReadDouble(double &val) {
    std::cin >> val;
    if(std::cin.fail()) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return false;
    }
    return true;
}

// Conversion Functions for user interaction
void convertTemperature(const UnitConverter& converter) {
    double value;
    std::string type;

    std::cout << "Enter temperature value: ";
    if(!safeReadDouble(value)) {
        std::cerr << "Invalid input. Please enter a numeric value.\n";
        return;
    }
    std::cout << "Choose conversion type:\n";
    std::cout << "1. CelsiusToFahrenheit\n";
    std::cout << "2. FahrenheitToCelsius\n";
    std::cout << "3. CelsiusToKelvin\n";
    std::cout << "4. KelvinToCelsius\n";
    std::cout << "Enter choice: ";
    int choice;
    std::cin >> choice;

    switch(choice) {
        case 1: type = "CelsiusToFahrenheit"; break;
        case 2: type = "FahrenheitToCelsius"; break;
        case 3: type = "CelsiusToKelvin"; break;
        case 4: type = "KelvinToCelsius"; break;
        default: 
            std::cerr << "Invalid conversion selection.\n";
            return;
    }

    try {
        double result = converter.convert(type, value);
        std::cout << "Converted value: " << std::fixed << std::setprecision(2) << result << "\n";
    } catch (const std::invalid_argument& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }
}

void convertDistance(const UnitConverter& converter) {
    double value;
    std::string type;

    std::cout << "Enter distance value: ";
    if(!safeReadDouble(value)) {
        std::cerr << "Invalid input. Please enter a numeric value.\n";
        return;
    }
    std::cout << "Choose conversion type:\n";
    std::cout << "1. KilometersToMiles\n";
    std::cout << "2. MilesToKilometers\n";
    std::cout << "3. MetersToFeet\n";
    std::cout << "4. FeetToMeters\n";
    std::cout << "Enter choice: ";
    int choice;
    std::cin >> choice;

    switch(choice) {
        case 1: type = "KilometersToMiles"; break;
        case 2: type = "MilesToKilometers"; break;
        case 3: type = "MetersToFeet"; break;
        case 4: type = "FeetToMeters"; break;
        default: 
            std::cerr << "Invalid conversion selection.\n";
            return;
    }

    try {
        double result = converter.convert(type, value);
        std::cout << "Converted value: " << std::fixed << std::setprecision(2) << result << "\n";
    } catch (const std::invalid_argument& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }
}

void convertWeight(const UnitConverter& converter) {
    double value;
    std::string type;

    std::cout << "Enter weight value: ";
    if(!safeReadDouble(value)) {
        std::cerr << "Invalid input. Please enter a numeric value.\n";
        return;
    }
    std::cout << "Choose conversion type:\n";
    std::cout << "1. KilogramsToPounds\n";
    std::cout << "2. PoundsToKilograms\n";
    std::cout << "3. GramsToOunces\n";
    std::cout << "4. OuncesToGrams\n";
    std::cout << "Enter choice: ";
    int choice;
    std::cin >> choice;

    switch(choice) {
        case 1: type = "KilogramsToPounds"; break;
        case 2: type = "PoundsToKilograms"; break;
        case 3: type = "GramsToOunces"; break;
        case 4: type = "OuncesToGrams"; break;
        default:
            std::cerr << "Invalid conversion selection.\n";
            return;
    }

    try {
        double result = converter.convert(type, value);
        std::cout << "Converted value: " << std::fixed << std::setprecision(2) << result << "\n";
    } catch (const std::invalid_argument& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }
}

// Convert Volume category
void convertVolume(const UnitConverter& converter) {
    double value;
    std::string type;

    std::cout << "Enter volume value: ";
    if(!safeReadDouble(value)) {
        std::cerr << "Invalid input. Please enter a numeric value.\n";
        return;
    }
    std::cout << "Choose conversion type:\n";
    std::cout << "1. LitersToGallons\n";
    std::cout << "2. GallonsToLiters\n";
    std::cout << "3. MillilitersToFluidOunces\n";
    std::cout << "4. FluidOuncesToMilliliters\n";
    std::cout << "Enter choice: ";
    int choice;
    std::cin >> choice;

    switch(choice) {
        case 1: type = "LitersToGallons"; break;
        case 2: type = "GallonsToLiters"; break;
        case 3: type = "MillilitersToFluidOunces"; break;
        case 4: type = "FluidOuncesToMilliliters"; break;
        default:
            std::cerr << "Invalid conversion selection.\n";
            return;
    }

    try {
        double result = converter.convert(type, value);
        std::cout << "Converted value: " << std::fixed << std::setprecision(2) << result << "\n";
    } catch (const std::invalid_argument& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }
}

void displayMenu() {
    std::cout << "\nUnit Converter\n";
    std::cout << "1. Convert Temperature\n";
    std::cout << "2. Convert Distance\n";
    std::cout << "3. Convert Weight\n";
    std::cout << "4. Convert Volume\n";
    std::cout << "5. Exit\n";
    std::cout << "Choose an option: ";
}

#ifndef UNIT_TEST
int main() {
    UnitConverter converter;
    int choice;

    do {
        displayMenu();
        std::cin >> choice;

        if(std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cerr << "Invalid input. Please enter a number corresponding to the menu option.\n";
            continue;
        }

        switch (choice) {
        case 1:
            convertTemperature(converter);
            break;
        case 2:
            convertDistance(converter);
            break;
        case 3:
            convertWeight(converter);
            break;
        case 4:
            convertVolume(converter);
            break;
        case 5:
            std::cout << "Exiting...\n";
            return 0;
        default:
            std::cerr << "Invalid option. Please try again.\n";
        }
    } while (choice != 5);

    return 0;
}
#endif
