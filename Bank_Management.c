#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Transaction {
    char type[10];  
    float amount;
    float balance_after;
    struct Transaction* next;
} Transaction;

typedef struct Account {
    int acc_number;
    char name[50];
    float balance;
    Transaction* transactions;
    struct Account* next;
} Account;


Account* find_account(int acc_num);

Account* head = NULL;
const char* FILENAME = "bank_data.txt";
const char* TRANSACTION_FILE = "transactions.txt";


void clear_input() {
    while (getchar() != '\n');
}

void press_to_continue() {
    printf("\nPress Enter to continue...");
    clear_input();
}


void save_to_file() {
    FILE* file = fopen(FILENAME, "w");
    if (!file) {
        printf("Error saving data!\n");
        return;
    }

    Account* current = head;
    while (current != NULL) {
        fprintf(file, "%d %s %.2f\n", 
                current->acc_number, 
                current->name, 
                current->balance);
        current = current->next;
    }
    fclose(file);

  
    file = fopen(TRANSACTION_FILE, "w");
    if (!file) {
        printf("Error saving transactions!\n");
        return;
    }

    current = head;
    while (current != NULL) {
        Transaction* trans = current->transactions;
        while (trans != NULL) {
            fprintf(file, "%d %s %.2f %.2f\n",
                    current->acc_number,
                    trans->type,
                    trans->amount,
                    trans->balance_after);
            trans = trans->next;
        }
        current = current->next;
    }
    fclose(file);
}

void load_from_file() {
    FILE* file = fopen(FILENAME, "r");
    if (!file) return;

    int acc_num;
    char name[50];
    float balance;

    while (fscanf(file, "%d %49s %f", &acc_num, name, &balance) == 3) {
        Account* new_acc = (Account*)malloc(sizeof(Account));
        new_acc->acc_number = acc_num;
        strcpy(new_acc->name, name);
        new_acc->balance = balance;
        new_acc->transactions = NULL;  
        new_acc->next = head;
        head = new_acc;
    }
    fclose(file);

    
    file = fopen(TRANSACTION_FILE, "r");
    if (!file) return;

    int account_no;
    char type[10];
    float amount, balance_after;

    while (fscanf(file, "%d %9s %f %f", &account_no, type, &amount, &balance_after) == 4) {
        Account* acc = find_account(account_no);
        if (acc != NULL) {
            Transaction* new_trans = (Transaction*)malloc(sizeof(Transaction));
            strcpy(new_trans->type, type);
            new_trans->amount = amount;
            new_trans->balance_after = balance_after;
            new_trans->next = acc->transactions;
            acc->transactions = new_trans;
        }
    }
    fclose(file);
}


void create_account() {
    Account* new_acc = (Account*)malloc(sizeof(Account));
    
    printf("\nEnter account number: ");
    scanf("%d", &new_acc->acc_number);
    clear_input();
    
    printf("Enter account holder name: ");
    fgets(new_acc->name, 50, stdin);
    new_acc->name[strcspn(new_acc->name, "\n")] = '\0';
    
    printf("Enter initial balance: ");
    scanf("%f", &new_acc->balance);
    clear_input();
    
    new_acc->transactions = NULL;  
    
    new_acc->next = head;
    head = new_acc;
    
    save_to_file();
    printf("\nAccount created successfully!\n");
}

void display_all() {
    printf("\n%-15s %-20s %s\n", "Account No.", "Name", "Balance");
    printf("----------------------------------------\n");
    
    Account* current = head;
    while (current != NULL) {
        printf("%-15d %-20s $%.2f\n", 
               current->acc_number, 
               current->name, 
               current->balance);
        current = current->next;
    }
}

Account* find_account(int acc_num) {
    Account* current = head;
    while (current != NULL) {
        if (current->acc_number == acc_num) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

void add_transaction(Account* acc, const char* type, float amount) {
    Transaction* new_trans = (Transaction*)malloc(sizeof(Transaction));
    strcpy(new_trans->type, type);
    new_trans->amount = amount;
    new_trans->balance_after = acc->balance;
    new_trans->next = acc->transactions;
    acc->transactions = new_trans;
}

void display_transactions(int acc_num) {
    Account* acc = find_account(acc_num);
    if (acc == NULL) {
        printf("Account not found!\n");
        return;
    }

    printf("\nTransaction History for Account %d (%s)\n", acc->acc_number, acc->name);
    printf("----------------------------------------\n");
    printf("%-10s %-15s %s\n", "Type", "Amount", "Balance After");
    printf("----------------------------------------\n");

    Transaction* trans = acc->transactions;
    while (trans != NULL) {
        printf("%-10s $%-14.2f $%.2f\n",
               trans->type,
               trans->amount,
               trans->balance_after);
        trans = trans->next;
    }
}

void deposit() {
    int acc_num;
    float amount;
    
    printf("\nEnter account number: ");
    scanf("%d", &acc_num);
    
    Account* acc = find_account(acc_num);
    if (acc == NULL) {
        printf("Account not found!\n");
        return;
    }
    
    printf("Enter amount to deposit: ");
    scanf("%f", &amount);
    clear_input();
    
    if (amount <= 0) {
        printf("Invalid amount!\n");
        return;
    }
    
    acc->balance += amount;
    add_transaction(acc, "DEPOSIT", amount);
    save_to_file();
    printf("\nDeposit successful. New balance: $%.2f\n", acc->balance);
}

void withdraw() {
    int acc_num;
    float amount;

    printf("\nEnter account number: ");
    scanf("%d", &acc_num);

    Account* acc = find_account(acc_num);
    if (acc == NULL) {
        printf("Account not found!\n");
        return;
    }

    printf("Enter amount to withdraw: ");
    scanf("%f", &amount);
    clear_input();

    if (amount <= 0 || amount > acc->balance) {
        printf("Invalid or insufficient funds!\n");
        return;
    }

    acc->balance -= amount;
    add_transaction(acc, "WITHDRAW", amount);
    save_to_file();
    printf("\nWithdrawal successful. New balance: $%.2f\n", acc->balance);
}

// 4. Menu System
void show_menu() {
    printf("\nSimple Banking System\n");
    printf("1. Create New Account\n");
    printf("2. Display All Accounts\n");
    printf("3. Deposit Money\n");
    printf("4. Withdraw Money\n");
    printf("5. View Transaction History\n");
    printf("6. Exit\n");
    printf("Enter your choice: ");
}

int main() {
    load_from_file();
    int choice;
    
    do {
        show_menu();
        scanf("%d", &choice);
        clear_input();
        
        switch(choice) {
            case 1: create_account(); break;
            case 2: display_all(); break;
            case 3: deposit(); break;
            case 4: withdraw(); break;
            case 5: {
                int acc_num;
                printf("\nEnter account number to view transactions: ");
                scanf("%d", &acc_num);
                clear_input();
                display_transactions(acc_num);
                break;
            }
            case 6: printf("Exiting...\n"); break;
            default: printf("Invalid choice!\n");
        }
        if (choice != 6) press_to_continue();
    } while (choice != 6);
    
    
    Account* current = head;
    while (current != NULL) {
        
        Transaction* trans = current->transactions;
        while (trans != NULL) {
            Transaction* temp_trans = trans;
            trans = trans->next;
            free(temp_trans);
        }
        
        Account* temp = current;
        current = current->next;
        free(temp);
    }
    
    return 0;
}
