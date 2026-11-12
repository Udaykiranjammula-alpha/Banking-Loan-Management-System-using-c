#include "../include/bank.h"

static void clear_input_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

int main(void) {
    BankSystem bank;
    bank_init(&bank);

    // Seed some demo accounts for demonstration
    Account* a1 = account_create(&bank, "Uday Kiran", ACCOUNT_SAVINGS, 5000.0);
    Account* a2 = account_create(&bank, "Alex Rivera", ACCOUNT_CURRENT, 12000.0);

    if (a1) {
        account_deposit(&bank, a1->account_number, 1500.0, "Consulting fee");
        loan_apply(&bank, a1->account_number, LOAN_EDUCATION, 10000.0, 12);
    }
    if (a2) {
        account_deposit(&bank, a2->account_number, 3000.0, "Client invoice");
    }

    printf("\n======================================================\n");
    printf("   BANKING & LOAN MANAGEMENT SYSTEM (CORE C11 ENGINE) \n");
    printf("======================================================\n");
    printf("Demo Accounts Initialized:\n");
    printf("  - Account %ld: Uday Kiran ($%.2f)\n", a1 ? a1->account_number : 0, a1 ? a1->balance : 0.0);
    printf("  - Account %ld: Alex Rivera ($%.2f)\n", a2 ? a2->account_number : 0, a2 ? a2->balance : 0.0);

    int choice = 0;
    while (choice != 8) {
        printf("\n---------------- MAIN MENU ----------------\n");
        printf("1. Create New Account\n");
        printf("2. Deposit Funds\n");
        printf("3. Withdraw Funds\n");
        printf("4. Transfer Funds\n");
        printf("5. Apply for Loan\n");
        printf("6. Pay Monthly Loan EMI\n");
        printf("7. View Account Statement & Ledger\n");
        printf("8. Exit\n");
        printf("Enter your choice (1-8): ");

        if (scanf("%d", &choice) != 1) {
            clear_input_buffer();
            printf("Invalid input. Please enter a number.\n");
            continue;
        }

        switch (choice) {
            case 1: {
                char name[MAX_NAME_LEN];
                int type_choice;
                double deposit;
                printf("Enter Account Holder Name: ");
                clear_input_buffer();
                if (fgets(name, sizeof(name), stdin)) {
                    name[strcspn(name, "\r\n")] = 0;
                }
                printf("Account Type (0: Savings, 1: Current, 2: Salary): ");
                if (scanf("%d", &type_choice) != 1 || type_choice < 0 || type_choice > 2) {
                    type_choice = 0;
                }
                printf("Initial Deposit ($): ");
                if (scanf("%lf", &deposit) != 1 || deposit < 0.0) {
                    deposit = 0.0;
                }
                Account* new_acc = account_create(&bank, name, (AccountType)type_choice, deposit);
                if (new_acc) {
                    printf("Success! Account created with Account Number: %ld\n", new_acc->account_number);
                } else {
                    printf("Failed to create account. Limit reached or invalid input.\n");
                }
                break;
            }
            case 2: {
                long acc_num;
                double amt;
                printf("Enter Account Number: ");
                scanf("%ld", &acc_num);
                printf("Enter Deposit Amount ($): ");
                scanf("%lf", &amt);
                if (account_deposit(&bank, acc_num, amt, "Cash deposit")) {
                    printf("Deposit successful! New balance: $%.2f\n", account_find(&bank, acc_num)->balance);
                } else {
                    printf("Deposit failed. Check account number and amount.\n");
                }
                break;
            }
            case 3: {
                long acc_num;
                double amt;
                printf("Enter Account Number: ");
                scanf("%ld", &acc_num);
                printf("Enter Withdrawal Amount ($): ");
                scanf("%lf", &amt);
                if (account_withdraw(&bank, acc_num, amt, "Cash withdrawal")) {
                    printf("Withdrawal successful! Remaining balance: $%.2f\n", account_find(&bank, acc_num)->balance);
                } else {
                    printf("Withdrawal failed. Check balance and account number.\n");
                }
                break;
            }
            case 4: {
                long src, dest;
                double amt;
                printf("Enter Sender Account Number: ");
                scanf("%ld", &src);
                printf("Enter Recipient Account Number: ");
                scanf("%ld", &dest);
                printf("Enter Transfer Amount ($): ");
                scanf("%lf", &amt);
                if (account_transfer(&bank, src, dest, amt, "Funds transfer")) {
                    printf("Transfer successful! Sender new balance: $%.2f\n", account_find(&bank, src)->balance);
                } else {
                    printf("Transfer failed. Check account numbers and sender balance.\n");
                }
                break;
            }
            case 5: {
                long acc_num;
                int ltype, tenure;
                double amt;
                printf("Enter Account Number: ");
                scanf("%ld", &acc_num);
                printf("Loan Type (0: Home, 1: Education, 2: Personal, 3: Vehicle): ");
                scanf("%d", &ltype);
                printf("Loan Principal Amount ($): ");
                scanf("%lf", &amt);
                printf("Tenure (Months, e.g. 12, 24, 60): ");
                scanf("%d", &tenure);
                if (loan_apply(&bank, acc_num, (LoanType)ltype, amt, tenure)) {
                    Account* a = account_find(&bank, acc_num);
                    printf("Loan approved and disbursed! Monthly EMI: $%.2f\n", a->active_loan.emi_amount);
                    loan_print_amortization_schedule(amt, a->active_loan.annual_interest_rate, tenure);
                } else {
                    printf("Loan application rejected. Ensure account exists and has no existing active loan.\n");
                }
                break;
            }
            case 6: {
                long acc_num;
                printf("Enter Account Number: ");
                scanf("%ld", &acc_num);
                if (loan_repay_emi(&bank, acc_num)) {
                    Account* a = account_find(&bank, acc_num);
                    printf("EMI payment processed! Remaining loan principal: $%.2f\n", a->active_loan.remaining_principal);
                } else {
                    printf("EMI payment failed. Check account balance and active loan status.\n");
                }
                break;
            }
            case 7: {
                long acc_num;
                printf("Enter Account Number: ");
                scanf("%ld", &acc_num);
                Account* a = account_find(&bank, acc_num);
                if (a) {
                    transaction_print_statement(a);
                } else {
                    printf("Account not found.\n");
                }
                break;
            }
            case 8:
                printf("Exiting Banking System. Thank you.\n");
                break;
            default:
                printf("Invalid option. Please choose between 1 and 8.\n");
        }
    }

    bank_free(&bank);
    return 0;
}
