#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <ctime>

#define reset "\033[0m"
#define BOLDRED "\033[1m"
#define red "\033[31m"
#define green "\033[32m"
using namespace std;

// Custom exception class
class HotelException : public runtime_error {
public:
    HotelException(const string& message) : runtime_error(message) {}
};

// Base class for a person
class Person {
protected:
    string name;
    int age;
public:
    Person(string n, int a) : name(n), age(a) {}

    virtual void display() const {
        cout << "Name: " << name << ", Age: " << age << endl;
    }

    virtual ~Person() {}
};

// Derived class for a guest
class Guest : public Person {
private:
    int guestID;
public:
    Guest(string n, int a, int id) : Person(n, a), guestID(id) {}

    void display() const override {
        cout << "Guest ID: " << guestID << ", ";
        Person::display();
    }

    int getGuestID() const {
        return guestID;
    }
};

// Derived class for an employee
class Employee : public Person {
private:
    int employeeID;
    string position;
public:
    Employee(string n, int a, int id, string pos) : Person(n, a), employeeID(id), position(pos) {}

    void display() const override {
        cout << "Employee ID: " << employeeID << ", Position: " << position << ", ";
        Person::display();
    }

    int getEmployeeID() const {
        return employeeID;
    }

    void setPosition(string newPosition) {
        position = newPosition;
    }
};

// Class for a room
class Room {
private:
    int roomNumber;
    string roomType;
    bool isBooked;
    int guestID; // To store the guest ID of the current occupant
public:
    Room(int number, string type) : roomNumber(number), roomType(type), isBooked(false), guestID(-1) {}

    void bookRoom() {
        if (isBooked) {
            throw HotelException("Room is already booked! \n \n");
        }
        isBooked = true;
    }

    void checkoutRoom() {
        if (!isBooked) {
            throw HotelException("Room is already available, can't checkout from an unoccupied room! \n \n");
        }
        isBooked = false;
        guestID = -1; // Reset guest ID upon checkout
    }

    bool getBookingStatus() const {
        return isBooked;
    }

    int getRoomNumber() const {
        return roomNumber;
    }

    string getRoomType() const {
        return roomType;
    }

    int getGuestID() const {
        return guestID;
    }

    void assignGuest(int guestID) {
        this->guestID = guestID;
    }

    void display() const {
        cout << "Room Number: " << roomNumber << ", Room Type: " << roomType
             << ", Booking Status: " << (isBooked ? "Booked" : "Available") << endl;
    }
};

class Reservation {
private:
    Guest guest;
    Room* room;
    int duration; // Duration of stay in days

public:
    Reservation(Guest g, Room* r, int d) : guest(g), room(r), duration(d) {
        try {
            room->bookRoom(); // Attempt to book the room
            room->assignGuest(g.getGuestID()); // Assign guest ID to the room
        } catch (const HotelException& e) {
            // Rollback in case of exception
            throw(""); // Re-throw the exception to be caught in higher levels
        }
    }

    Guest getGuest() const {
        return guest;
    }

    Room* getRoom() const {
        return room;
    }

    void checkoutRoom() {
        room->checkoutRoom(); // Mark the room as available
    }

    void display() const {
        cout << "Reservation for: " << endl;
        guest.display();
        cout << "Room Details: " << endl;
        room->display();
        cout << "Duration: " << duration << " days" << endl;
    }
};

class FoodOrder {
private:
    Guest guest;
    string foodItem;
    int quantity;
    double price;

public:
    FoodOrder(Guest g, string item, int qty, double pr) : guest(g), foodItem(item), quantity(qty), price(pr) {}

    void placeOrder(int guestID) const {
        if (guestID != guest.getGuestID()) {
            throw invalid_argument("Error: Guest ID not found. Unable to place the order. \n \n");
        }
    }

    void display() const {
        cout << "Food Order for: " << endl;
        try {
            placeOrder(guest.getGuestID());
        } catch(const invalid_argument& e) {
            cout << e.what() << endl;
            return;
        }
        guest.display();
        cout << "Food Item: " << foodItem << ", Quantity: " << quantity << ", Price: Rs. " << price << endl;
        displayDayTime();
    }

    void displayDayTime() const {
        time_t now = time(0);
        tm* localTime = localtime(&now);
        cout << "Date: " << localTime->tm_mday << "-" << (localTime->tm_mon + 1) << "-" << (localTime->tm_year + 1900) << " ";
        cout << "Time: " << localTime->tm_hour << ":" << localTime->tm_min << ":" << localTime->tm_sec << "\n \n" << endl;
    }

    int getGuestID() const {
        return guest.getGuestID();
    }
};

// Class for a hotel
class Hotel {
private:
    string hotelName;
    vector<Room*> rooms; // Composition relationship: Hotel owns Rooms and manages their lifetime
    vector<Employee*> employees; // Aggregation relationship: Hotel uses Employees but does not own them
    vector<Reservation*> reservations; // Composition relationship: Hotel owns Reservations and manages their lifetime
    vector<FoodOrder*> foodOrders; // Aggregation relationship: Hotel uses FoodOrders but does not own them

public:
    Hotel(string name) : hotelName(name) {}

    ~Hotel() {
        for (auto room : rooms) {
            delete room;
        }
        for (auto reservation : reservations) {
            delete reservation;
        }
        for (auto foodOrder : foodOrders) {
            delete foodOrder;
        }
    }

    void addRoom(int roomNumber, string roomType) {
        rooms.push_back(new Room(roomNumber, roomType));
    }

    void addEmployee(Employee* employee) {
        employees.push_back(employee);
    }

    void makeReservation(Guest guest, int roomNumber, int duration) {
        Room* room = findRoom(roomNumber); // Find the room with the given room number

        if (room) {
            try {
                reservations.push_back(new Reservation(guest, room, duration));
                cout << green << "Reservation successful!\n \n" <<reset << endl;
            } catch (const HotelException& e) {
                cout << e.what() << endl;
            }
        } else {
            throw HotelException("Room not found! \n \n");
        }
    }

    void checkoutRoom(int roomNumber) {
        auto it = find_if(rooms.begin(), rooms.end(), [roomNumber](const Room* room) { return room->getRoomNumber() == roomNumber; });
        if (it != rooms.end()) {
            try {
                (*it)->checkoutRoom();
                cout << green<<"Room checked out successfully! \n \n" <<reset<< endl;
            } catch (const HotelException& e) {
                cout << e.what() << endl;
            }
        } else {
            throw HotelException("Room not found! \n \n");
        }
    }

    void updateEmployeePosition(int employeeID, string newPosition) {
        auto it = find_if(employees.begin(), employees.end(), [employeeID](const Employee* employee) { return employee->getEmployeeID() == employeeID; });
        if (it != employees.end()) {
            (*it)->setPosition(newPosition);
            cout << green<<"Employee position updated successfully! \n \n" <<reset<< endl;
        } else {
            throw HotelException("Employee not found! \n \n");
        }
    }

    void addFoodOrder(Guest guest, string foodItem, int quantity, double price) {
        bool guestFound = false;
        for (auto room : rooms) {
            if (room->getBookingStatus() && room->getGuestID() == guest.getGuestID()) {
                guestFound = true;
                break;
            }
        }
        if (!guestFound) {
            throw HotelException("Guest Id not found in any booked room. Unable to place food order. \n \n");
        }

        foodOrders.push_back(new FoodOrder(guest, foodItem, quantity, price));
        cout << green<<"Food order placed successfully! \n \n" <<reset<< endl;
    }

    void display() const {
        cout << "\nHotel: " << hotelName << endl;
        displayRooms();
        displayEmployees();
        displayReservations();
        displayFoodOrders();
    }

    void displayRooms() const {
        cout << "\nRooms:" << endl;
        for (const auto room : rooms) {
            room->display();
        }
    }

    void displayEmployees() const {
        cout << "\nEmployees:" << endl;
        for (const auto employee : employees) {
            employee->display();
        }
    }

    void displayReservations() const {
        cout << "\nReservations:" << endl;
        for (const auto reservation : reservations) {
            try {
                reservation->display();
            } catch (const HotelException& e) {
                cout << e.what() << endl;
            }
        }
    }

    void displayFoodOrders() const {
        cout << "\nFood Orders:" << endl;
        for (const auto foodOrder : foodOrders) {
            try {
                foodOrder->display();
            } catch (const HotelException& e) {
                cout << e.what() << endl;
            }
        }
    }

private:
    Room* findRoom(int roomNumber) {
        auto it = find_if(rooms.begin(), rooms.end(), [roomNumber](const Room* room) { return room->getRoomNumber() == roomNumber; });
        if (it != rooms.end()) {
            return *it;
        }
        return nullptr;
    }

};

// Helper function to get integer input
int getIntInput() {
    int value;
    cout << "Enter a number: ";
    cin >> value;
    if (cin.fail()) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        throw HotelException("Invalid input. Please enter a valid number.");
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return value;
}

// Helper function to get double input
double getDoubleInput() {
    double value;
    cout << "Enter a number: ";
    cin >> value;
    if (cin.fail()) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        throw HotelException("Invalid input. Please enter a valid number. \n \n");
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return value;
}

// Helper function to get string input
string getStringInput() {
    string value;
    cout << "Enter a string: ";
    getline(cin, value);
    if (value.empty()) {
        throw HotelException("Invalid input. Please enter a non-empty string. \n \n");
    }
    return value;
}

int main() {
    Hotel hotel("Hotel Management System");
    while (true) {
        cout <<"-------------------------------------" <<endl;
        cout <<"|    Hotel Management System        |" <<endl;
        cout <<"-------------------------------------" <<endl;
        //cout << "\nBVB Hotel Management System" << endl;
        cout << "1. Add Room" << endl;
        cout << "2. Add Employee" << endl;
        cout << "3. Make Reservation" << endl;
        cout << "4. Checkout Room" << endl;
        cout << "5. Update Employee Position" << endl;
        cout << "6. Add Food Order" << endl;
        cout << "7. Display Hotel Details" << endl;
        cout << "8. Display Rooms" << endl;
        cout << "9. Display Employees" << endl;
        cout << "10. Display Reservations" << endl;
        cout << "11. Display Food Orders" << endl;
        cout << "12. Exit" << endl;
        cout << "Enter your choice: ";
        try {
            int choice = getIntInput();

            switch (choice) {
                case 1: {
                    int roomNumber;
                    string roomType;
                    cout << "Enter room number: ";
                    roomNumber = getIntInput();
                    cout << "Enter room type: ";
                    roomType = getStringInput();
                    hotel.addRoom(roomNumber, roomType);
                    cout << "Room Added Successfully!!" << endl;
                    break;
                }
                case 2: {
                    string name, position;
                    int age, employeeID;
                    cout << "Enter employee name: ";
                    name = getStringInput();
                    cout << "Enter employee age: ";
                    age = getIntInput();
                    cout << "Enter employee ID: ";
                    employeeID = getIntInput();
                    cout << "Enter employee position: ";
                    position = getStringInput();
                    hotel.addEmployee(new Employee(name, age, employeeID, position));
                    break;
                }
                case 3: {
                    string name;
                    int age, guestID, roomNumber, duration;
                    cout << "Enter guest name: ";
                    name = getStringInput();
                    cout << "Enter guest age: ";
                    age = getIntInput();
                    cout << "Enter guest ID: ";
                    guestID = getIntInput();
                    cout << "Enter room number: ";
                    roomNumber = getIntInput();
                    cout << "Enter duration of stay (days): ";
                    duration = getIntInput();
                    hotel.makeReservation(Guest(name, age, guestID), roomNumber, duration);
                    break;
                }
                case 4: {
                    int roomNumber;
                    cout << "Enter room number: ";
                    roomNumber = getIntInput();
                    hotel.checkoutRoom(roomNumber);
                    break;
                }
                case 5: {
                    int employeeID;
                    string newPosition;
                    cout << "Enter employee ID: ";
                    employeeID = getIntInput();
                    cout << "Enter new position: ";
                    newPosition = getStringInput();
                    hotel.updateEmployeePosition(employeeID, newPosition);
                    break;
                }
                case 6: {
                    string foodItem;
                    int guestID, quantity;
                    double price;
                    cout << "Enter guest ID: ";
                    guestID = getIntInput();
                    cout << "Enter food item: ";
                    foodItem = getStringInput();
                    cout << "Enter quantity: ";
                    quantity = getIntInput();
                    cout << "Enter price: ";
                    price = getDoubleInput();
                    try {
                        hotel.addFoodOrder(Guest("", 0, guestID), foodItem, quantity, price);
                    } catch (const HotelException& e) {
                        cout << e.what() << endl;
                    }
                    break;
                }
                case 7: {
                    hotel.display();
                    break;
                }
                case 8: {
                    hotel.displayRooms();
                    break;
                }
                case 9: {
                    hotel.displayEmployees();
                    break;
                }
                case 10: {
                    hotel.displayReservations();
                    break;
                }
                case 11: {
                    hotel.displayFoodOrders();
                    break;
                }
                case 12: {
                    cout << BOLDRED << "Exiting the system. Goodbye!" << reset << endl;
                    return 0;
                }
                default: {
                    cout << red << "Invalid choice. Please try again." << reset << endl;
                }
            }
        } catch (const HotelException& e) {
            cout << e.what() << endl;
        } catch (const invalid_argument& e) {
            cout << e.what() << endl;
        } catch (...) {
            cout << red << "An unknown error occurred." << reset << endl;
        }
    }

    return 0;
}


//Thank You
