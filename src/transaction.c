#include "../include/bank.h"

void transaction_record(Account* acc, TransactionType type, double amount, double balance_after, const char* note) {
    if (!acc) return;

    Transaction* tx = (Transaction*)malloc(sizeof(Transaction));
    if (!tx) return;

    static long global_tx_seq = 10001;
    tx->transaction_id = global_tx_seq++;
    tx->account_number = acc->account_number;
    tx->type = type;
    tx->amount = amount;
    tx->balance_after = balance_after;
    tx->timestamp = time(NULL);
    strncpy(tx->note, note ? note : "", sizeof(tx->note) - 1);
    tx->note[sizeof(tx->note) - 1] = '\0';
    tx->next = NULL;

    // Append to end of linked list (O(1) amortized or append)
    if (!acc->transaction_head) {
        acc->transaction_head = tx;
    } else {
        Transaction* curr = acc->transaction_head;
        while (curr->next) {
            curr = curr->next;
        }
        curr->next = tx;
    }
}

void transaction_print_statement(const Account* acc) {
    if (!acc) return;

    printf("\n============================ ACCOUNT STATEMENT ============================\n");
    printf("Account No: %ld | Holder: %s | Type: %s | Current Balance: $%.2f\n",
           acc->account_number, acc->holder_name, account_type_str(acc->type), acc->balance);
    if (acc->has_active_loan) {
        printf("Active Loan: %s | Loan ID: #%ld | Remaining Balance: $%.2f | Monthly EMI: $%.2f\n",
               loan_type_str(acc->active_loan.type), acc->active_loan.loan_id,
               acc->active_loan.remaining_principal, acc->active_loan.emi_amount);
    }
    printf("---------------------------------------------------------------------------\n");
    printf("%-8s | %-12s | %-12s | %-12s | %-20s\n", "TxID", "Type", "Amount", "Balance After", "Description");
    printf("---------------------------------------------------------------------------\n");

    const Transaction* curr = acc->transaction_head;
    if (!curr) {
        printf("No transactions recorded yet.\n");
    } else {
        while (curr) {
            printf("#%-7ld | %-12s | $%-11.2f | $%-11.2f | %-20s\n",
                   curr->transaction_id, tx_type_str(curr->type),
                   curr->amount, curr->balance_after, curr->note);
            curr = curr->next;
        }
    }
    printf("===========================================================================\n\n");
}

const char* tx_type_str(TransactionType type) {
    switch (type) {
        case TX_DEPOSIT:           return "Deposit";
        case TX_WITHDRAWAL:        return "Withdrawal";
        case TX_TRANSFER:          return "Transfer";
        case TX_LOAN_DISBURSEMENT: return "Loan Disb";
        case TX_LOAN_REPAYMENT:    return "EMI Payment";
        default: return "Other";
    }
}
