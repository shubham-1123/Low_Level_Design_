#include <iostream>
#include <map>
#include <chrono>

enum class VehicleType {
    kBike = 0,
    kCar,
    kTruck,
};

// Overload the ostream operator to get he string representation
std::ostream& operator<<(std::ostream& os, const VehicleType& type) {
    switch (type) {
        case VehicleType::kBike:   return os << "Bike";
        case VehicleType::kCar: return os << "Car";
        case VehicleType::kTruck:  return os << "Truck";
        default:           return os << "UnknownVehicleType";
    }
}

class Payment {
public:
    virtual double CalculateCost(double hours) = 0;
};

class BikePayment : public Payment {
public:
    double CalculateCost(double hours) {
        return hours * 1;
    }
};

class CarPayment : public Payment { 
public:
    double CalculateCost(double hours) {
        return hours * 2;
    }
};

class TruckPayment : public Payment {
public:
    double CalculateCost(double hours) {
        return hours * 5;
    }
};

class Vehicle {
protected:
    std::string numberPlate;
    Payment *payment;
    std::chrono::time_point<std::chrono::system_clock> parkTime;
public:
    virtual enum VehicleType GetType() = 0;
    virtual double CalculateCost(double hours) {
        return payment->CalculateCost(hours);
    }
    void SetParkedTime() {
        parkTime = std::chrono::system_clock::now();
    }
    std::chrono::time_point<std::chrono::system_clock> GetParkTime() {
        return parkTime;
    }
    std::string GetNumberPlate() {
        return numberPlate;
    }
};

class Bike : public Vehicle {
public:
    Bike(std::string numberPlate) {
        payment = new BikePayment();
        this->numberPlate = numberPlate;
    }
    enum VehicleType GetType() {
        return VehicleType::kBike;
    }
};

class Car : public Vehicle {
public:
    Car(std::string numberPlate) {
        payment = new CarPayment();
        this->numberPlate = numberPlate;
    }
    enum VehicleType GetType() {
        return VehicleType::kCar;
    }
};

class Truck : public Vehicle {
public:
    Truck(std::string numberPlate) {
        payment = new TruckPayment();
        this->numberPlate = numberPlate;
    }
    enum VehicleType GetType() {
        return VehicleType::kTruck;
    }
};

class ParkingLot {
private:
    std::map<int, std::map<int, std::map<std::string, Vehicle*>>> spots;
    int floors;
    int rows;
    int spotsPerRow;
public:
    ParkingLot(int floors, int rows, int sportsPerRow) : floors(floors), rows(rows), spotsPerRow(sportsPerRow) { }
    bool Park(Vehicle *vehicle, int floor, int row) {
        if (spots[floor][row].size() < spotsPerRow) {
            std::string numberPlate = vehicle->GetNumberPlate();
            spots[floor][row][numberPlate] = vehicle;
            std::cout << vehicle->GetType() << " parked successfully at floor " << floor << ", row " << row << "." << std::endl;
            return true;
        } else {
            std::cout << "This floor row is alreafy full. Please select another floor." << std::endl;
            return false;
        }
    }
    bool Leave(Vehicle * vehicle) {
        for (int i = 0; i < floors; i++) {
            for (int j = 0; j < rows; j++) {
                std::string numberPlate = vehicle->GetNumberPlate();
                if (spots[i][j].find(numberPlate) != spots[i][j].end()) {
                    Vehicle *ownerVehicle = spots[i][j][vehicle->GetNumberPlate()];
                    double hours = CalculateParkingHours(ownerVehicle);
                    double cost = ownerVehicle->CalculateCost(hours);
                    spots[i][j].erase(numberPlate);
                    std::cout << vehicle->GetType() << " left successfully. Total cost: " << cost << std::endl;
                    return true;
                }
            }
        }
        std::cout << vehicle->GetType() << " not found." << std::endl;
        return false;
    }
    double CalculateParkingHours(Vehicle* vehicle) {
        for (int i = 0; i < floors; i++) {
            for (int j = 0; j < rows; j++) {
                std::string numberPlate = vehicle->GetNumberPlate();
                if (spots[i][j].find(numberPlate) != spots[i][j].end()) {
                    Vehicle *ownerVehicle = spots[i][j][numberPlate];
                    // Get the current time
                    auto currentTime = std::chrono::system_clock::now();
                    auto parkTime = ownerVehicle->GetParkTime();
                    //auto duration = std::chrono::duration_cast<std::chrono::hours>(currentTime - parkTime);
                    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime - parkTime);
                    return duration.count();
                }
            }
        }
        return 0;
    }
    int AvailableSpots(int floor) {
        int count = 0;
        for (int r = 0; r < rows; r++) {
            count += spotsPerRow - (int)spots[floor][r].size();
        }
        return count;
    }
    void Clear() {
        for (auto& outer : spots) {
            outer.second.clear(); // Clear the inner map
        }
        spots.clear(); // Clear the outer map
    }
};

int main(int argc, char* argv[])
{
    ParkingLot parkinglot(3, 10, 20);
    Car car1("CC1234"), car2("CC3456");
    Bike bike1("BB0011"), bike2("BB1122");
    Truck tr1("TR9879");

    std::cout << "Available spots before on floor 0: " << parkinglot.AvailableSpots(0) << std::endl;
    car1.SetParkedTime();
    parkinglot.Park(&car1, 0, 0);
    std::cout << "Available spots after on floor 0: " << parkinglot.AvailableSpots(0) << std::endl;

    std::cout << "Available spots before on floor 0: " << parkinglot.AvailableSpots(0) << std::endl;
    car2.SetParkedTime();
    parkinglot.Park(&car2, 0, 0);
    std::cout << "Available spots after on floor 0: " << parkinglot.AvailableSpots(0) << std::endl;

    std::cout << "Available spots before on floor 1: " << parkinglot.AvailableSpots(1) << std::endl;
    bike1.SetParkedTime();
    parkinglot.Park(&bike1, 1, 0);
    std::cout << "Available spots after on floor 1: " << parkinglot.AvailableSpots(1) << std::endl;

    std::cout << "Available spots before on floor 1: " << parkinglot.AvailableSpots(1) << std::endl;
    tr1.SetParkedTime();
    parkinglot.Park(&tr1, 2, 9);
    std::cout << "Available spots after on floor 0: " << parkinglot.AvailableSpots(2) << std::endl;

    parkinglot.Leave(&car1);
    std::cout << "Available spots after car1 leave from floor 0: " << parkinglot.AvailableSpots(0) << std::endl;
    parkinglot.Leave(&bike2);
    std::cout << "Available spots after bike2 leave: " << parkinglot.AvailableSpots(0) << std::endl;
    parkinglot.Clear();
    return 0;
}
/*
Available spots before on floor 0: 200
Car parked successfully at floor 0, row 0.
Available spots after on floor 0: 199
Available spots before on floor 0: 199
Car parked successfully at floor 0, row 0.
Available spots after on floor 0: 198
Available spots before on floor 1: 200
Bike parked successfully at floor 1, row 0.
Available spots after on floor 1: 199
Available spots before on floor 1: 199
Truck parked successfully at floor 2, row 9.
Available spots after on floor 0: 199
Car left successfully. Total cost: 74000
Available spots after car1 leave from floor 0: 199
Bike not found.
Available spots after bike2 leave: 199
*/