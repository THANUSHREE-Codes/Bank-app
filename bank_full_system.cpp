// bank_full_system.cpp
// Full Bank Account & Transaction System demonstrating required C++ features.
// Compile: g++ -std=c++17 bank_full_system.cpp -o bank_system
// Run: ./bank_system

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include <stdexcept>
#include <functional>

using namespace std;

// --------------------------- 1) TEMPLATE (Requirement 9) ---------------------------
// Simple template function to calculate matured amount (simple interest demonstration)
template<typename T>
T calculateMatured(T principal, double ratePercent, int years) {
    // T must support multiplication and addition
    return principal * (1 + (ratePercent/100.0) * years);
}

// --------------------------- 2) ABSTRACT BASE (Requirement 6) ---------------------
class AccountBase {
public:
    virtual void display() = 0; // pure virtual -> abstract class
    virtual ~AccountBase() {}
};

// --------------------------- 3) VIRTUAL BASE (Diamond problem - Req 5) ----------
class Person : virtual public AccountBase {
protected:
    string name;                  // protected (accessible to derived classes)
public:
    Person(const string& n = "Unknown") : name(n) {}
    virtual ~Person() {}
};

// Simple base providing account number
class BankBase {
protected:
    int accNumber;                // protected
public:
    BankBase(int acc = 0) : accNumber(acc) {}
    int getAccNumber() const { return accNumber; }
    virtual ~BankBase() {}
};

// --------------------------- 4) TRANSACTION CLASS --------------------------------
class Transaction {
private:
    int fromAcc;
    int toAcc;
    double amount;
    string note;
public:
    Transaction(int f=0, int t=0, double a=0.0, const string& n="") :
        fromAcc(f), toAcc(t), amount(a), note(n) {}

    string toRecord() const {
        // safe simple textual record format: from|to|amount|note
        return to_string(fromAcc) + "|" + to_string(toAcc) + "|" + to_string(amount) + "|" + note + "\n";
    }
};

// --------------------------- 5) BANK ACCOUNT CLASS --------------------------------
// Demonstrates: class design, public/private/protected, constructors/destructors,
// operator overloading, function overloading, default args, pass-by-ref & return-by-ref,
// implements pure virtual display() from AccountBase.
class BankAccount : public Person, public BankBase {
private:
    double balance;               // private member (encapsulation)

public:
    // Constructor (Requirement 2)
    BankAccount(const string& n = "Unknown", int acc = 0, double bal = 0.0) :
        Person(n), BankBase(acc), balance(bal) {
        cout << "[Constructor] Account created: " << accNumberInfo() << "\n";
    }

    // Destructor (Requirement 2)
    ~BankAccount() {
        cout << "[Destructor] Account object for acc# " << accNumberInfo() << " destroyed.\n";
    }

    // Helper to get account summary string
    string accNumberInfo() const {
        return to_string(getAccNumber()) + " (" + name + ")";
    }

    // ----------------- 4a) Operator overloading (Requirement 4) -----------------
    // Deposit with operator+
    BankAccount operator+(double amt) const {
        if (amt < 0) throw invalid_argument("Deposit amount cannot be negative.");
        BankAccount temp = *this;
        temp.balance += amt;
        return temp;
    }

    // Withdraw with operator-
    BankAccount operator-(double amt) const {
        if (amt < 0) throw invalid_argument("Withdrawal amount cannot be negative.");
        if (balance < amt) throw runtime_error("Insufficient balance for withdrawal.");
        BankAccount temp = *this;
        temp.balance -= amt;
        return temp;
    }

    // Stream output operator for convenience (friend)
    friend ostream& operator<<(ostream& os, const BankAccount& acc) {
        os << "Acc#: " << acc.accNumber << " | Name: " << acc.name
           << " | Balance: " << fixed << setprecision(2) << acc.balance;
        return os;
    }

    // ----------------- 7) Function overloading (Requirement 7) -----------------
    // deposit version (single-arg)
    void updateBalance(double amt) {
        if (amt < 0) throw invalid_argument("Amount cannot be negative.");
        balance += amt;
    }
    // overloaded updateBalance: can withdraw if withdraw==true
    void updateBalance(double amt, bool withdraw) {
        if (amt < 0) throw invalid_argument("Amount cannot be negative.");
        if (withdraw) {
            if (balance < amt) throw runtime_error("Insufficient balance for withdrawal.");
            balance -= amt;
        } else {
            balance += amt;
        }
    }

    // ----------------- Default argument (Requirement 7) -------------------------
    void showDetails(bool full = true) const {
        cout << "Account Number: " << accNumber << " | Name: " << name;
        if (full) cout << " | Balance: " << fixed << setprecision(2) << balance;
        cout << "\n";
    }

    // ----------------- Pass-by-reference & Return-by-reference ------------------
    double& getBalanceRef() { return balance; }              // return by reference
    const double& getBalanceConstRef() const { return balance; }

    // ----------------- Required ctor/dtor, and implement pure virtual -----------
    void display() const override {
        showDetails(true);
    }

    // For file handling (serialize/deserialize)
    string toRecord() const {
        // Name can include spaces; use '|' separator
        return name + "|" + to_string(accNumber) + "|" + to_string(balance) + "\n";
    }

    static BankAccount fromRecord(const string& rec) {
        // Format: name|acc|balance
        size_t p1 = rec.find('|');
        size_t p2 = rec.find('|', p1 + 1);
        string n = rec.substr(0, p1);
        int acc = stoi(rec.substr(p1 + 1, p2 - (p1 + 1)));
        double bal = stod(rec.substr(p2 + 1));
        return BankAccount(n, acc, bal);
    }
};

// --------------------------- 6) ACCOUNT MANAGER (File handling) ------------------
// Demonstrates file handling to store & retrieve data (Requirement 8)
class AccountManager {
private:
    const string accountsFile = "accounts.txt";
    const string transactionsFile = "transactions.txt";

public:
    // Create or append account to file
    void createAccount(const BankAccount& acc) {
        ofstream ofs(accountsFile, ios::app);
        if (!ofs) throw runtime_error("Unable to open accounts file for writing.");
        ofs << acc.toRecord();
        ofs.close();
    }

    // Load all accounts from file
    vector<BankAccount> loadAllAccounts() {
        vector<BankAccount> list;
        ifstream ifs(accountsFile);
        if (!ifs) return list; // file might not exist yet
        string line;
        while (getline(ifs, line)) {
            if (line.empty()) continue;
            try {
                BankAccount a = BankAccount::fromRecord(line);
                list.push_back(a);
            } catch (...) {
                // ignore malformed lines
            }
        }
        ifs.close();
        return list;
    }

    // Save all accounts (overwrite)
    void saveAllAccounts(const vector<BankAccount>& accounts) {
        ofstream ofs(accountsFile);
        if (!ofs) throw runtime_error("Unable to open accounts file for writing.");
        for (const auto& a : accounts) ofs << a.toRecord();
        ofs.close();
    }

    // Find account index by account number
    int findAccountIndex(vector<BankAccount>& list, int accNo) {
        for (size_t i = 0; i < list.size(); ++i) {
            if (list[i].getAccNumber() == accNo) return (int)i;
        }
        return -1;
    }

    // Transfer funds (shows objects passed & returned and exception handling)
    bool transferFunds(int fromAcc, int toAcc, double amount) {
        if (amount <= 0) throw invalid_argument("Transfer amount must be positive.");
        vector<BankAccount> list = loadAllAccounts();
        int idxFrom = findAccountIndex(list, fromAcc);
        int idxTo = findAccountIndex(list, toAcc);
        if (idxFrom == -1 || idxTo == -1) throw runtime_error("Source or destination account not found.");

        // Check balance
        if (list[idxFrom].getBalanceConstRef() < amount) throw runtime_error("Insufficient funds in source account.");

        // Perform transfer using overloaded functions
        list[idxFrom].updateBalance(amount, true); // withdraw
        list[idxTo].updateBalance(amount);         // deposit

        // Save transaction record
        Transaction tx(fromAcc, toAcc, amount, "transfer");
        ofstream ofs(transactionsFile, ios::app);
        if (!ofs) throw runtime_error("Unable to write transaction file.");
        ofs << tx.toRecord();
        ofs.close();

        // rewrite accounts file
        saveAllAccounts(list);
        return true;
    }

    // Utility: deposit using object pass & return
    BankAccount depositToAccount(BankAccount acc, double amount) {
        // acc passed by value (object passed) and returned by value (object returned)
        if (amount <= 0) throw invalid_argument("Deposit amount must be positive.");
        acc = acc + amount; // uses operator+
        return acc;
    }

    // Utility: withdraw by reference (pass-by-reference)
    void withdrawFromAccount(vector<BankAccount>& list, int accNo, double amount) {
        int idx = findAccountIndex(list, accNo);
        if (idx == -1) throw runtime_error("Account not found.");
        // use operator- via assignment
        list[idx] = list[idx] - amount;
    }
};

// --------------------------- 7) UTILITY FUNCTION (pass/return objects) ----------
BankAccount giveSignupBonus(BankAccount acc) {
    // Example of object passed & returned
    acc.updateBalance(100.0); // bonus
    return acc;
}

// --------------------------- 8) MAIN: demonstration & simple menu ---------------
int main() {
    AccountManager mgr;
    try {
        cout << "=== Bank Account & Transaction System (Demo) ===\n\n";

        // 1) Create sample accounts and persist them
        cout << "[Demo] Creating sample accounts (Alice, Bob)...\n";
        BankAccount a1("Alice", 1001, 1500.0);
        BankAccount a2("Bob",   1002, 800.0);

        // Save to file
        mgr.createAccount(a1);
        mgr.createAccount(a2);

        // 2) Display (abstract override + operator<<)
        cout << "\n[Demo] Displaying created accounts:\n";
        a1.display();
        a2.display();

        // 3) Operator overloading deposit and withdraw
        cout << "\n[Demo] Depositing 200 to Alice using operator+ ...\n";
        a1 = a1 + 200.0; // operator+
        cout << a1 << "\n";

        cout << "[Demo] Withdrawing 100 from Alice using updateBalance(withdraw)...\n";
        a1.updateBalance(100.0, true);
        cout << a1 << "\n";

        // 4) Return-by-reference demo
        double &refBal = a1.getBalanceRef(); // return by reference
        refBal += 25.0;                       // directly update using reference
        cout << "[Demo] After modifying via returned reference: " << a1 << "\n";

        // 5) Template usage
        double matured = calculateMatured<double>(1000.0, 5.0, 2); // simple interest
        cout << "[Demo] Template function calculateMatured: 1000 at 5% for 2 years -> " << matured << "\n";

        // 6) Process account via object pass & return
        cout << "[Demo] Applying signup bonus to Bob (object pass/return)...\n";
        a2 = giveSignupBonus(a2);
        cout << a2 << "\n";

        // 7) Write updated single-account objects (rewrite all)
        vector<BankAccount> all = mgr.loadAllAccounts();
        // update entries in file to reflect modified balances for demo
        for (auto &acc : all) {
            if (acc.getAccNumber() == a1.getAccNumber()) acc = a1;
            if (acc.getAccNumber() == a2.getAccNumber()) acc = a2;
        }
        mgr.saveAllAccounts(all);

        // 8) Perform a transfer using AccountManager (file-backed)
        cout << "\n[Demo] Transfer 200 from Alice (1001) to Bob (1002)...\n";
        bool t = mgr.transferFunds(1001, 1002, 200.0);
        if (t) cout << "[Demo] Transfer successful and files updated.\n";

        // 9) Exception handling demo: attempted invalid withdrawal
        try {
            cout << "\n[Demo] Attempting invalid withdrawal (-50) to demonstrate exception handling...\n";
            a1.updateBalance(-50.0);
        } catch (const exception &e) {
            cerr << "[Caught Exception] " << e.what() << "\n";
        }

        // 10) Show file-backed accounts now
        cout << "\n[Demo] Final accounts loaded from file:\n";
        vector<BankAccount> finalList = mgr.loadAllAccounts();
        for (const auto &x : finalList) x.display();

        cout << "\n=== Demo finished successfully ===\n";
    } catch (const exception &e) {
        cerr << "[Unhandled Exception] " << e.what() << "\n";
    }
    return 0;
}
