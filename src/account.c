#include "../include/bank.h"

void bank_init(BankSystem* bank) {
    if (!bank) return;
    bank->account_count = 0;
    bank->next_account_number = ACCOUNT_NUM_START;
    bank->next_tx_id = 50001;
    bank->next_loan_id = 90001;
    memset(bank->accounts, 0, sizeof(bank->accounts));
}

void bank_free(BankSystem* bank) {
    if (!bank) return;
    for (int i = 0; i < bank->account_count; i++) {
        Transaction* curr = bank->accounts[i].transaction_head;
        while (curr) {
            Transaction* next = curr->next;
            free(curr);
            curr = next;
        }
        bank->accounts[i].transaction_head = NULL;
    }
}

Account* account_create(BankSystem* bank, const char* name, AccountType type, double initial_deposit) {
    if (!bank || !name || bank->account_count >= MAX_ACCOUNTS) return NULL;
    if (initial_deposit < 0.0) return NULL;

    Account* acc = &bank->accounts[bank->account_count++];
    acc->account_number = bank->next_account_number++;
    strncpy(acc->holder_name, name, MAX_NAME_LEN - 1);
    acc->holder_name[MAX_NAME_LEN - 1] = '\0';
    acc->type = type;
    acc->balance = initial_deposit;
    acc->is_active = true;
    acc->created_at = time(NULL);
    acc->has_active_loan = false;
    acc->transaction_head = NULL;

    if (initial_deposit > 0.0) {
        transaction_record(acc, TX_DEPOSIT, initial_deposit, acc->balance, "Initial account opening deposit");
    }

    return acc;
}

Account* account_find(BankSystem* bank, long account_number) {
    if (!bank) return NULL;
    for (int i = 0; i < bank->account_count; i++) {
        if (bank->accounts[i].account_number == account_number && bank->accounts[i].is_active) {
            return &bank->accounts[i];
        }
    }
    return NULL;
}

bool account_deposit(BankSystem* bank, long account_number, double amount, const char* note) {
    if (!bank || amount <= 0.0) return false;
    Account* acc = account_find(bank, account_number);
    if (!acc) return false;

    acc->balance += amount;
    transaction_record(acc, TX_DEPOSIT, amount, acc->balance, note ? note : "Cash deposit");
    return true;
}

bool account_withdraw(BankSystem* bank, long account_number, double amount, const char* note) {
    if (!bank || amount <= 0.0) return false;
    Account* acc = account_find(bank, account_number);
    if (!acc) return false;

    if (acc->balance < amount) {
        return false; // Insufficient funds
    }

    acc->balance -= amount;
    transaction_record(acc, TX_WITHDRAWAL, amount, acc->balance, note ? note : "Cash withdrawal");
    return true;
}

bool account_transfer(BankSystem* bank, long from_acc, long to_acc, double amount, const char* note) {
    if (!bank || from_acc == to_acc || amount <= 0.0) return false;
    Account* src = account_find(bank, from_acc);
    Account* dest = account_find(bank, to_acc);
    if (!src || !dest) return false;

    if (src->balance < amount) {
        return false;
    }

    src->balance -= amount;
    dest->balance += amount;

    char src_note[128];
    char dest_note[128];
    snprintf(src_note, sizeof(src_note), "Transfer to A/C %ld: %s", to_acc, note ? note : "Funds transfer");
    snprintf(dest_note, sizeof(dest_note), "Transfer from A/C %ld: %s", from_acc, note ? note : "Funds transfer");

    transaction_record(src, TX_TRANSFER, amount, src->balance, src_note);
    transaction_record(dest, TX_TRANSFER, amount, dest->balance, dest_note);
    return true;
}

const char* account_type_str(AccountType type) {
    switch (type) {
        case ACCOUNT_SAVINGS: return "Savings";
        case ACCOUNT_CURRENT: return "Current";
        case ACCOUNT_SALARY:  return "Salary";
        default: return "Unknown";
    }
}
