#include "../include/bank.h"
#include <assert.h>
#include <math.h>

void test_account_creation_and_deposit(void) {
    BankSystem bank;
    bank_init(&bank);

    Account* acc = account_create(&bank, "Test User", ACCOUNT_SAVINGS, 1000.0);
    assert(acc != NULL);
    assert(acc->balance == 1000.0);
    assert(acc->account_number == ACCOUNT_NUM_START);

    bool dep_res = account_deposit(&bank, acc->account_number, 500.0, "Test deposit");
    assert(dep_res == true);
    assert(acc->balance == 1500.0);

    bool with_res = account_withdraw(&bank, acc->account_number, 300.0, "Test withdrawal");
    assert(with_res == true);
    assert(acc->balance == 1200.0);

    // Overdraft attempt
    bool over_res = account_withdraw(&bank, acc->account_number, 2000.0, "Overdraft");
    assert(over_res == false);
    assert(acc->balance == 1200.0);

    bank_free(&bank);
    printf("PASS: test_account_creation_and_deposit\n");
}

void test_transfer(void) {
    BankSystem bank;
    bank_init(&bank);

    Account* a1 = account_create(&bank, "Alice", ACCOUNT_SAVINGS, 2000.0);
    Account* a2 = account_create(&bank, "Bob", ACCOUNT_CURRENT, 500.0);

    bool transfer_res = account_transfer(&bank, a1->account_number, a2->account_number, 800.0, "Rent");
    assert(transfer_res == true);
    assert(a1->balance == 1200.0);
    assert(a2->balance == 1300.0);

    // Transfer more than balance
    bool fail_res = account_transfer(&bank, a1->account_number, a2->account_number, 5000.0, "Too big");
    assert(fail_res == false);
    assert(a1->balance == 1200.0);

    bank_free(&bank);
    printf("PASS: test_transfer\n");
}

void test_loan_and_emi(void) {
    BankSystem bank;
    bank_init(&bank);

    Account* acc = account_create(&bank, "Charlie", ACCOUNT_SALARY, 1000.0);
    assert(acc != NULL);

    // Formula test: P = 10000, R = 12% (1% monthly), N = 12 months
    // EMI = 10000 * 0.01 * (1.01^12) / ((1.01^12) - 1) ~ 888.49
    double emi = loan_calculate_emi(10000.0, 12.0, 12);
    assert(fabs(emi - 888.49) < 0.1);

    bool applied = loan_apply(&bank, acc->account_number, LOAN_PERSONAL, 5000.0, 6);
    assert(applied == true);
    assert(acc->has_active_loan == true);
    assert(acc->balance == 6000.0); // 1000 + 5000 disbursed

    // Repay EMI
    bool repaid = loan_repay_emi(&bank, acc->account_number);
    assert(repaid == true);
    assert(acc->active_loan.months_paid == 1);
    assert(acc->active_loan.remaining_principal < 5000.0);

    bank_free(&bank);
    printf("PASS: test_loan_and_emi\n");
}

int main(void) {
    printf("Running Banking System Unit Tests...\n");
    test_account_creation_and_deposit();
    test_transfer();
    test_loan_and_emi();
    printf("ALL TESTS PASSED SUCCESSFULLY!\n");
    return 0;
}
