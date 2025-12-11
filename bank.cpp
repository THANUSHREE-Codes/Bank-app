// FINAL — FULLY FIXED BANK MANAGEMENT SYSTEM
// Includes ALL required OOP features with NO compilation errors.

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include <stdexcept>

using namespace std;

// =========================== TEMPLATE (Requirement 9) ===========================
template <typename T>
T maxValue(T a, T b) {
    return (a > b) ? a : b;
}

// ====================== ABSTRACT CLASS (Requirement 6) ==========================
class AccountBase {
public:
    virtual void display() const = 0; 
    virtual ~AccountBase() {}
};

// ====================== PERSON CLASS (virtual base) =============================
class Person : virtual public AccountBase {
protected:
    string name;

public:
    Person(string n = "Unknown") : name(n) {}
};

// ====================== BANK BASE CLASS (virtual) ===============================
class BankBase : virtual public AccountBase {
protected:
    int accNumber;

public:
    BankBase(int acc = 0) : accNumber(acc) {}
};

// ====================== BANK ACCOUNT CLASS ======================================
class BankAccount : public Person, public BankBase {
private:
    double balance;

public:
    // Constructor
    BankAccount(string n = "Unknown", int acc = 0, double bal = 0.0)
        : Person(n), BankBase(acc), balance(bal) {
        cout << "[Constructor] Account Created: " << accNumber << "\n";
    }

    // Destructor
    ~BankAccount() {
        cout << "[Destructor] Account Destroyed: " << accNumber << "\n";
    }

    // PURE VIRTUAL IMPLEMENTATION (must be const)
    void display() const override {
        cout << "Acc#: " << accNumber << " | Name: " << name 
             << " | Balance: " << balance << "\n";
    }

    //--------------------------------------------
    // OPERATOR OVERLOADING (Requirement 4)
    //--------------------------------------------
    BankAccount operator+(double amt) const {
        if (amt < 0) throw invalid_argument("Negative deposit");
        return BankAccount(name, accNumber, balance + amt);
    }

    BankAccount operator-(double amt) const {
        if (amt < 0) throw invalid_argument("Negative withdrawal");
        if (amt > balance) throw runtime_error("Insufficient funds");
        return BankAccount(name, accNumber, balance - amt);
    }

    friend ostream& operator<<(ostream& os, const BankAccount& acc) {
        os << "Acc#: " << acc.accNumber 
           << " | Name: " << acc.name 
           << " | Balance: " << acc.balance;
        return os;
    }

    //--------------------------------------------
    // Function Overloading + Default Arguments
    //--------------------------------------------
    void update(double amt) {
        balance += amt;
    }

    void update(double amt, bool withdraw) {
        if (withdraw) {
            if (amt > balance) throw runtime_error("Not enough money");
            balance -= amt;
        } else {
            balance += amt;
        }
    }

    void show(bool showBalance = true) const {
        cout << "Account: " << accNumber << " | " << name;
        if (showBalance) cout << " | Bal: " << balance;
        cout << "\n";
    }

    //--------------------------------------------
    // Pass-by-reference & Return-by-reference
    //--------------------------------------------
    double& getBalanceRef() { return balance; }
    const double& getBalanceConst() const { return balance; }

    //--------------------------------------------
    // File handling (serialize)
    //--------------------------------------------
    string serialize() const {
        return name + "|" + to_string(accNumber) + "|" + to_string(balance) + "\n";
    }

    static BankAccount deserialize(string record) {
        size_t p1 = record.find('|');
        size_t p2 = record.find('|', p1 + 1);

        string n = record.substr(0, p1);
        int acc = stoi(record.substr(p1 + 1, p2 - p1 - 1));
        double bal = stod(record.substr(p2 + 1));

        return BankAccount(n, acc, bal);
    }

    int getAccNo() const { return accNumber; }
};

// ====================== ACCOUNT MANAGER (FILE HANDLING) ==========================
class AccountManager {
private:
    const string fileName = "bankdata.txt";

public:
    void saveAccount(const BankAccount& acc) {
        ofstream fout(fileName, ios::app);
        fout << acc.serialize();
    }

    vector<BankAccount> loadAll() {
        vector<BankAccount> list;
        ifstream fin(fileName);
        string line;

        while (getline(fin, line)) {
            if (line.size() > 2)
                list.push_back(BankAccount::deserialize(line));
        }

        return list;
    }
};

// ============================== MAIN PROGRAM ====================================
int main() {
    try {
        AccountManager mgr;

        BankAccount a1("Alice", 1001, 2000);
        BankAccount a2("Bob", 1002, 1500);

        a1.display();
        a2.display();

        cout << "\n--- Operator Overloading ---\n";
        a1 = a1 + 500;
        a2 = a2 - 200;
        cout << a1 << "\n" << a2 << "\n";

        cout << "\n--- Pass-by-ref / Return-by-ref ---\n";
        double& balRef = a1.getBalanceRef();
        balRef += 50;  // modifies object directly
        a1.display();

        cout << "\n--- Default argument ---\n";
        a1.show();
        a1.show(false);

        cout << "\n--- Function Overloading ---\n";
        a2.update(300);
        a2.update(200, true);
        a2.display();

        cout << "\n--- File Handling ---\n";
        mgr.saveAccount(a1);
        mgr.saveAccount(a2);

        cout << "Saved accounts to file.\n";

        cout << "\n--- Loaded Accounts ---\n";
        vector<BankAccount> all = mgr.loadAll();
        for (const auto& acc : all) acc.display();

        cout << "\n--- Template Function ---\n";
        cout << "Max of 20 and 50 is: " << maxValue(20, 50) << "\n";

    } catch (exception& e) {
        cerr << "ERROR: " << e.what() << "\n";
    }

    return 0;
}
                