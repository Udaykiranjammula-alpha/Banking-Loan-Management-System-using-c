#ifndef BANK_H
#define BANK_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#define MAX_NAME_LEN 64
#define MAX_ACCOUNTS 1000
#define MAX_TRANSACTIONS 5000
#define ACCOUNT_NUM_START 100100

typedef enum {
    ACCOUNT_SAVINGS,
    ACCOUNT_CURRENT,
    ACCOUNT_SALARY
} AccountType;

typedef enum {
    TX_DEPOSIT,
    TX_WITHDRAWAL,
    TX_TRANSFER,
    TX_LOAN_DISBURSEMENT,
    TX_LOAN_REPAYMENT
} TransactionType;

typedef enum {
    LOAN_HOME,
    LOAN_EDUCATION,
    LOAN_PERSONAL,
    LOAN_VEHICLE
} LoanType;

typedef enum {
    LOAN_PENDING,
    LOAN_APPROVED,
    LOAN_ACTIVE,
    LOAN_CLOSED,
    LOAN_REJECTED
} LoanStatus;

typedef struct Transaction {
    long transaction_id;
    long account_number;
    TransactionType type;
    double amount;
    double balance_after;
    time_t timestamp;
    char note[128];
    struct Transaction* next;
} Transaction;

typedef struct Loan {
    long loan_id;
    long account_number;
    LoanType type;
    LoanStatus status;
    double principal_amount;
    double annual_interest_rate; // Percentage e.g. 8.5
    int tenure_months;
    double emi_amount;
    double remaining_principal;
    int months_paid;
} Loan;

typedef struct Account {
    long account_number;
    char holder_name[MAX_NAME_LEN];
    AccountType type;
    double balance;
    bool is_active;
    time_t created_at;
    Loan active_loan;
    bool has_active_loan;
    Transaction* transaction_head;
} Account;

typedef struct BankSystem {
    Account accounts[MAX_ACCOUNTS];
    int account_count;
    long next_account_number;
    long next_tx_id;
    long next_loan_id;
} BankSystem;

// System Management
void bank_init(BankSystem* bank);
void bank_free(BankSystem* bank);

// Account Operations
Account* account_create(BankSystem* bank, const char* name, AccountType type, double initial_deposit);
Account* account_find(BankSystem* bank, long account_number);
bool account_deposit(BankSystem* bank, long account_number, double amount, const char* note);
bool account_withdraw(BankSystem* bank, long account_number, double amount, const char* note);
bool account_transfer(BankSystem* bank, long from_acc, long to_acc, double amount, const char* note);

// Loan & Amortization Operations
double loan_calculate_emi(double principal, double annual_rate, int tenure_months);
bool loan_apply(BankSystem* bank, long account_number, LoanType type, double amount, int tenure_months);
bool loan_repay_emi(BankSystem* bank, long account_number);
void loan_print_amortization_schedule(double principal, double annual_rate, int tenure_months);

// Ledger & Transaction Operations
void transaction_record(Account* acc, TransactionType type, double amount, double balance_after, const char* note);
void transaction_print_statement(const Account* acc);

// Utilities
const char* account_type_str(AccountType type);
const char* loan_type_str(LoanType type);
const char* loan_status_str(LoanStatus status);
const char* tx_type_str(TransactionType type);

#endif // BANK_H
