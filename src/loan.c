#include "../include/bank.h"
#include <math.h>

double loan_calculate_emi(double principal, double annual_rate, int tenure_months) {
    if (principal <= 0.0 || tenure_months <= 0) return 0.0;
    if (annual_rate <= 0.0) return principal / tenure_months;

    double monthly_rate = (annual_rate / 12.0) / 100.0;
    double factor = pow(1.0 + monthly_rate, tenure_months);
    double emi = principal * monthly_rate * factor / (factor - 1.0);
    return emi;
}

bool loan_apply(BankSystem* bank, long account_number, LoanType type, double amount, int tenure_months) {
    if (!bank || amount <= 0.0 || tenure_months <= 0) return false;
    Account* acc = account_find(bank, account_number);
    if (!acc || acc->has_active_loan) return false;

    double interest_rate = 8.5; // Base rate
    switch (type) {
        case LOAN_HOME:      interest_rate = 7.5; break;
        case LOAN_EDUCATION: interest_rate = 6.8; break;
        case LOAN_VEHICLE:   interest_rate = 8.9; break;
        case LOAN_PERSONAL:  interest_rate = 11.5; break;
    }

    double emi = loan_calculate_emi(amount, interest_rate, tenure_months);

    Loan loan;
    loan.loan_id = bank->next_loan_id++;
    loan.account_number = account_number;
    loan.type = type;
    loan.status = LOAN_ACTIVE;
    loan.principal_amount = amount;
    loan.annual_interest_rate = interest_rate;
    loan.tenure_months = tenure_months;
    loan.emi_amount = emi;
    loan.remaining_principal = amount;
    loan.months_paid = 0;

    acc->active_loan = loan;
    acc->has_active_loan = true;

    // Disburse funds directly into account
    acc->balance += amount;
    char note[128];
    snprintf(note, sizeof(note), "%s Loan #%ld Disbursed", loan_type_str(type), loan.loan_id);
    transaction_record(acc, TX_LOAN_DISBURSEMENT, amount, acc->balance, note);

    return true;
}

bool loan_repay_emi(BankSystem* bank, long account_number) {
    if (!bank) return false;
    Account* acc = account_find(bank, account_number);
    if (!acc || !acc->has_active_loan || acc->active_loan.status != LOAN_ACTIVE) return false;

    Loan* loan = &acc->active_loan;
    double emi = loan->emi_amount;

    if (acc->balance < emi) {
        return false; // Insufficient funds for EMI deduction
    }

    double monthly_rate = (loan->annual_interest_rate / 12.0) / 100.0;
    double interest_part = loan->remaining_principal * monthly_rate;
    double principal_part = emi - interest_part;
    if (principal_part > loan->remaining_principal) {
        principal_part = loan->remaining_principal;
    }

    acc->balance -= emi;
    loan->remaining_principal -= principal_part;
    loan->months_paid++;

    char note[128];
    snprintf(note, sizeof(note), "EMI Payment (%d/%d) - Loan #%ld", loan->months_paid, loan->tenure_months, loan->loan_id);
    transaction_record(acc, TX_LOAN_REPAYMENT, emi, acc->balance, note);

    if (loan->remaining_principal <= 0.01 || loan->months_paid >= loan->tenure_months) {
        loan->status = LOAN_CLOSED;
        loan->remaining_principal = 0.0;
        acc->has_active_loan = false;
    }

    return true;
}

void loan_print_amortization_schedule(double principal, double annual_rate, int tenure_months) {
    if (principal <= 0.0 || tenure_months <= 0) return;

    double emi = loan_calculate_emi(principal, annual_rate, tenure_months);
    double monthly_rate = (annual_rate / 12.0) / 100.0;
    double balance = principal;

    printf("\n======================= LOAN AMORTIZATION SCHEDULE =======================\n");
    printf("Principal: $%.2f | Annual Rate: %.2f%% | Tenure: %d Months | Monthly EMI: $%.2f\n",
           principal, annual_rate, tenure_months, emi);
    printf("--------------------------------------------------------------------------\n");
    printf("%-5s | %-12s | %-12s | %-12s | %-12s\n", "Month", "EMI", "Interest", "Principal", "Remaining");
    printf("--------------------------------------------------------------------------\n");

    for (int month = 1; month <= tenure_months; month++) {
        double interest = balance * monthly_rate;
        double principal_part = emi - interest;
        balance -= principal_part;
        if (balance < 0.0) balance = 0.0;

        printf("%-5d | $%-11.2f | $%-11.2f | $%-11.2f | $%-11.2f\n",
               month, emi, interest, principal_part, balance);
    }
    printf("==========================================================================\n\n");
}

const char* loan_type_str(LoanType type) {
    switch (type) {
        case LOAN_HOME:      return "Home Loan";
        case LOAN_EDUCATION: return "Education Loan";
        case LOAN_PERSONAL:  return "Personal Loan";
        case LOAN_VEHICLE:   return "Vehicle Loan";
        default: return "General Loan";
    }
}

const char* loan_status_str(LoanStatus status) {
    switch (status) {
        case LOAN_PENDING:  return "Pending";
        case LOAN_APPROVED: return "Approved";
        case LOAN_ACTIVE:   return "Active";
        case LOAN_CLOSED:   return "Closed";
        case LOAN_REJECTED: return "Rejected";
        default: return "Unknown";
    }
}
