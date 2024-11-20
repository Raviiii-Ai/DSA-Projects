#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Complaint structure
typedef struct ComplaintData{
    int id;
    int studentID;
    char type[50];
    char description[100];
    int priority; // 1 = High, 0 = Low
    int status;   // 0 = Unsolved, 1 = Solved, 2 = Rejected
    char comment[200]; // To store comments for solved complaints
} Complaint;

#define MAX_COMPLAINTS 100
Complaint complaints[MAX_COMPLAINTS];
int complaintCount = 0;

// Passwords
char problemSolverPassword[20] = "root";
char vcPassword[20] = "vc";

// Linked List Node for Rejected Complaints
typedef struct Node {
    Complaint complaint;
    struct Node *next;
} Node;

// Stack for Solved Complaints
typedef struct StackNode {
    Complaint complaint;
    struct StackNode *next;
} StackNode;

StackNode *solvedStack = NULL; // Top of the stack
Node *rejectedList = NULL; // Head of the rejected linked list

// Queue for Unsolved Complaints
typedef struct QueueNode {
    Complaint complaint;
    struct QueueNode *next;
} QueueNode;

typedef struct {
    QueueNode *front;
    QueueNode *rear;
} Queue;

Queue unsolvedQueue = {NULL, NULL}; // Initialize empty queue

// Change password function
void change_password(char *currentPassword, char *portalName) {
    char oldPassword[20], newPassword[20];
    printf("Enter current password for %s Portal: ", portalName);
    scanf("%s", oldPassword);

    if (strcmp(currentPassword, oldPassword) != 0) {
        printf("Incorrect current password.\n");
        return;
    }

    printf("Enter new password: ");
    scanf("%s", newPassword);
    strcpy(currentPassword, newPassword);
    printf("Password for %s Portal changed successfully.\n", portalName);
}


// File name
const char *fileName = "complaints.csv";

// Function Declaration
void student_portal();
void problem_solver_portal();
void vice_chancellor_portal();
void file_complaint();
void track_complaint();
void view_complaint_history();
void view_unsolved_complaints();
void view_rejected_complaints();
void view_solved_complaints();
void solve_complaint();
void reject_complaint();
void display_complaints_table(Complaint complaints[], int count, int filterStatus);
void add_comment_to_complaint();
int read_complaints_from_file();
void append_complaint_to_file(const Complaint *complaint);
int get_highest_complaint_id();
void sort_complaints_by_priority();  

// Main menu
int main() {
    int choice;
    printf("\n\t\t\t\t\033[1mWELCOME TO COMPLAINT MANAGEMENT SYSTEM\033[0m\n");
    read_complaints_from_file();  
    do {
        printf("\nMain Menu:\n");
        printf("1. Student Portal\n");
        printf("2. Problem Solver Portal\n");
        printf("3. Vice Chancellor Portal\n");
        printf("4. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: student_portal(); break;
            case 2: problem_solver_portal(); break;
            case 3: vice_chancellor_portal(); break;
            case 4: printf("Exiting program.\n"); break;
            default: printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 4);

    return 0;
}

// Student Portal
void student_portal() {
    int choice;
    do {
        printf("\nStudent Portal:\n");
        printf("1. File Complaint\n");
        printf("2. Track Complaint by ID\n");
        printf("3. View Complaint History\n");
        printf("4. Back\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: file_complaint(); break;
            case 2: track_complaint(); break;
            case 3: view_complaint_history(); break;
            case 4: break;
            default: printf("Invalid choice. Try again.\n");
        }
    } while (choice != 4);
}

// Problem Solver Portal
void problem_solver_portal() {
    char password[20];
    printf("\nEnter password for Problem Solver Portal: ");
    scanf("%s", password);

    if (strcmp(password, problemSolverPassword) != 0) {
        printf("Incorrect password.\n");
        return;
    }

    int choice;
    do {
        printf("\nProblem Solver Portal:\n");
        printf("1. View Unsolved Complaints\n");
        printf("2. Solve Complaint\n");
        printf("3. Reject Complaint\n");
        printf("4. View Rejected Complaints\n");
        printf("5. View Solved Complaints\n");
        printf("6. Change Password\n");
        printf("7. Back\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: view_unsolved_complaints(); break;
            case 2: solve_complaint(); break;
            case 3: reject_complaint(); break;
            case 4: view_rejected_complaints(); break;
            case 5: view_solved_complaints(); break;
            case 6: change_password(problemSolverPassword, "Problem Solver"); break;
            case 7: break;
            default: printf("Invalid choice. Try again.\n");
        }
    } while (choice != 7);
}

// Vice Chancellor Portal
void vice_chancellor_portal() {
    char password[20];
    printf("\nEnter password for Vice Chancellor Portal: ");
    scanf("%s", password);

    if (strcmp(password, vcPassword) != 0) {
        printf("Incorrect password.\n");
        return;
    }

    int choice;
    do {
        printf("\nVice Chancellor Portal:\n");
        printf("1. View Unsolved Complaints\n");
        printf("2. View Solved Complaints\n");
        printf("3. View Rejected Complaints\n");
        printf("4. Add Comment to Complaint\n");
        printf("5. Change Password\n");
        printf("6. Back\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: view_unsolved_complaints(); break;
            case 2: view_solved_complaints(); break;
            case 3: view_rejected_complaints(); break;
            case 4: add_comment_to_complaint(); break;
            case 5: change_password(vcPassword, "Vice Chancellor"); break;
            case 6: break;
            default: printf("Invalid choice. Try again.\n");
        }
    } while (choice != 6);
}

void enqueue_unsolved(Complaint *complaint) {
    QueueNode *newNode = (QueueNode *)malloc(sizeof(QueueNode));
    newNode->complaint = *complaint;
    newNode->next = NULL;

    if (unsolvedQueue.rear == NULL) {
        unsolvedQueue.front = unsolvedQueue.rear = newNode;
    } else {
        unsolvedQueue.rear->next = newNode;
        unsolvedQueue.rear = newNode;
    }
}

Complaint dequeue_unsolved() {
    if (unsolvedQueue.front == NULL) {
        printf("No unsolved complaints.\n");
        exit(1);
    }

    QueueNode *temp = unsolvedQueue.front;
    Complaint complaint = temp->complaint;
    unsolvedQueue.front = unsolvedQueue.front->next;

    if (unsolvedQueue.front == NULL) {
        unsolvedQueue.rear = NULL;
    }

    free(temp);
    return complaint;
}

void push_solved(Complaint *complaint) {
    StackNode *newNode = (StackNode *)malloc(sizeof(StackNode));
    newNode->complaint = *complaint;
    newNode->next = solvedStack;
    solvedStack = newNode;
}

Complaint pop_solved() {
    if (solvedStack == NULL) {
        printf("No solved complaints.\n");
        exit(1);
    }

    StackNode *temp = solvedStack;
    Complaint complaint = temp->complaint;
    solvedStack = solvedStack->next;
    free(temp);
    return complaint;
}

void add_rejected(Complaint *complaint) {
    Node *newNode = (Node *)malloc(sizeof(Node));
    newNode->complaint = *complaint;
    newNode->next = rejectedList;
    rejectedList = newNode;
}

void display_rejected() {
    Node *current = rejectedList;
    if (!current) {
        printf("No rejected complaints.\n");
        return;
    }

    while (current) {
        printf("Complaint ID: %d\n", current->complaint.id);
        printf("Type: %s\n", current->complaint.type);
        printf("Description: %s\n", current->complaint.description);
        printf("\n");
        current = current->next;
    }
}

void solve_complaint() {
    int found = 0;

    // First, check for high-priority complaints
    for (int i = 0; i < complaintCount; i++) {
        if (complaints[i].priority == 1 && complaints[i].status == 0) {  // High priority and unsolved
            complaints[i].status = 1;  // Mark the complaint as solved
            printf("High priority complaint with ID %d has been solved.\n", complaints[i].id);
            found = 1;
        }
    }

    // If no high priority complaints were found, solve the first low priority complaint
    if (!found) {
        for (int i = 0; i < complaintCount; i++) {
            if (complaints[i].priority == 0 && complaints[i].status == 0) {  // Low priority and unsolved
                complaints[i].status = 1;  // Mark the complaint as solved
                printf("Low priority complaint with ID %d has been solved.\n", complaints[i].id);
                found = 1;
                return;  // Exit after solving the first low-priority complaint
            }
        }
    }

    if (!found) {
        printf("No unsolved complaints found.\n");
    }
}


void reject_complaint() {
    if (unsolvedQueue.front == NULL) {
        printf("No unsolved complaints to reject.\n");
        return;
    }

    // Dequeue the complaint and mark it as rejected
    Complaint complaint = dequeue_unsolved();
    complaint.status = 2; // Rejected
    add_rejected(&complaint);

    printf("Complaint ID %d marked as rejected.\n", complaint.id);
}

// Sort complaints by priority (High priority first)
void sort_complaints_by_priority() {
    for (int i = 0; i < complaintCount - 1; i++) {
        for (int j = i + 1; j < complaintCount; j++) {
            if (complaints[i].priority < complaints[j].priority) {
                // Swap the complaints if the current one has lower priority
                Complaint temp = complaints[i];
                complaints[i] = complaints[j];
                complaints[j] = temp;
            }
        }
    }
}



// Display complaints in a tabular format
void display_complaints_table(Complaint complaints[], int count, int filterStatus) {
    // Sort complaints by priority: High priority first
    sort_complaints_by_priority();

    printf("-----------------------------------------------------------------------------------------------------\n");
    printf("| ID | Student ID | Type              | Description                          | Priority | Status     |\n");
    printf("-----------------------------------------------------------------------------------------------------\n");

    int found = 0;
    for (int i = 0; i < count; i++) {
        if (complaints[i].status == filterStatus || filterStatus == -1) {
            found = 1;
            printf("| %-2d | %-10d | %-17s | %-35s | %-8s | %-10s |\n",
                   complaints[i].id,
                   complaints[i].studentID,
                   complaints[i].type,
                   complaints[i].description,
                   complaints[i].priority == 1 ? "High" : "Low",
                   complaints[i].status == 0 ? "Unsolved" : 
                   complaints[i].status == 1 ? "Solved" : "Rejected");
        }
    }

    if (!found) {
        printf("| No complaints found matching the criteria.                                                          |\n");
    }

    printf("-----------------------------------------------------------------------------------------------------\n");
}

void file_complaint() {
    Complaint newComplaint;

    newComplaint.id = get_highest_complaint_id() + 1;  // Get the next available complaint ID
    printf("Enter your Student ID: ");
    scanf("%d", &newComplaint.studentID);

    printf("Enter Complaint Type (e.g., Ragging, Water Issue, Electricity Issue, Others): ");
    getchar(); // Clear newline character from buffer
    fgets(newComplaint.type, sizeof(newComplaint.type), stdin);
    newComplaint.type[strcspn(newComplaint.type, "\n")] = '\0'; // Remove newline

    printf("Enter Complaint Description: ");
    fgets(newComplaint.description, sizeof(newComplaint.description), stdin);
    newComplaint.description[strcspn(newComplaint.description, "\n")] = '\0'; // Remove newline

    // Set priority based on complaint type
    if (strcmp(newComplaint.type, "Sexual Harassment") == 0 || 
        strcmp(newComplaint.type, "Water Issue") == 0 || 
        strcmp(newComplaint.type, "Electricity Issue") == 0 || 
        strcmp(newComplaint.type, "Ragging") == 0) {
        newComplaint.priority = 1;  // High priority
    } else {
        newComplaint.priority = 0;  // Low priority
    }

    newComplaint.status = 0;  // Unsolved status
    strcpy(newComplaint.comment, ""); // Initialize with empty comment

    // Add complaint to the unsolved queue
    enqueue_unsolved(&newComplaint);

    // Update in-memory array for viewing history
    complaints[complaintCount] = newComplaint;
    complaintCount++;

    // Append the complaint to the file
    append_complaint_to_file(&newComplaint);

    printf("Complaint filed successfully. Your Complaint ID is: %d\n", newComplaint.id);
}


// Track a complaint
void track_complaint() {
    int id;
    printf("Enter Complaint ID: ");
    scanf("%d", &id);

    for (int i = 0; i < complaintCount; i++) {
        if (complaints[i].id == id) {
            printf("Complaint ID: %d\n", complaints[i].id);
            printf("Type: %s\n", complaints[i].type);
            printf("Description: %s\n", complaints[i].description);
            printf("Priority: %s\n", complaints[i].priority == 1 ? "High" : "Low");
            printf("Status: %s\n", complaints[i].status == 0 ? "Unsolved" : 
                                     complaints[i].status == 1 ? "Solved" : "Rejected");
            return;
        }
    }

    printf("Complaint ID not found.\n");
}

// View complaint history
void view_complaint_history() {
    int studentID;
    printf("Enter your Student ID to view complaint history: ");
    scanf("%d", &studentID);

    printf("-----------------------------------------------------------------------------------------------------\n");
    printf("| ID | Student ID | Type              | Description                          | Priority | Status     |\n");
    printf("-----------------------------------------------------------------------------------------------------\n");

    int found = 0;
    for (int i = 0; i < complaintCount; i++) {
        if (complaints[i].studentID == studentID) {
            found = 1;
            printf("| %-2d | %-10d | %-17s | %-35s | %-8s | %-10s |\n",
                   complaints[i].id,
                   complaints[i].studentID,
                   complaints[i].type,
                   complaints[i].description,
                   complaints[i].priority == 1 ? "High" : "Low",
                   complaints[i].status == 0 ? "Unsolved" : 
                   complaints[i].status == 1 ? "Solved" : "Rejected");
        }
    }

    if (!found) {
        printf("| No complaints found for Student ID %d.                                                             |\n", studentID);
    }

    printf("-----------------------------------------------------------------------------------------------------\n");
}


// View unsolved complaints
void view_unsolved_complaints() {
    display_complaints_table(complaints, complaintCount, 0);
}

// View rejected complaints
void view_rejected_complaints() {
    display_complaints_table(complaints, complaintCount, 2);
}

// View solved complaints
void view_solved_complaints() {
    display_complaints_table(complaints, complaintCount, 1);
}


// Add comment to a complaint
void add_comment_to_complaint() {
    int id;
    printf("Enter Complaint ID to add comment: ");
    scanf("%d", &id);

    for (int i = 0; i < complaintCount; i++) {
        if (complaints[i].id == id) {
            printf("Enter comment: ");
            getchar(); // Clear newline character from buffer
            fgets(complaints[i].comment, sizeof(complaints[i].comment), stdin);
            complaints[i].comment[strcspn(complaints[i].comment, "\n")] = '\0';  // Remove newline
            printf("Comment added.\n");
            return;
        }
    }

    printf("Complaint ID not found.\n");
}

int read_complaints_from_file() {
    FILE *file = fopen("complaints.csv", "r");
    if (!file) {
        printf("No complaints file found.\n");
        return 0;
    }

    char line[300];
    while (fgets(line, sizeof(line), file)) {
        Complaint c;
        sscanf(line, "%d,%d,%49[^,],%99[^,],%d,%d,%199[^\n]",
               &c.id,
               &c.studentID,
               c.type,
               c.description,
               &c.priority,
               &c.status,
               c.comment);
        complaints[complaintCount++] = c;
    }
    fclose(file);
    return 1;
}


void append_complaint_to_file(const Complaint *complaint) {
    FILE *file = fopen("complaints.csv", "a");
    if (file) {
        fprintf(file, "%d,%d,%s,%s,%d,%d,%s\n",
                complaint->id,
                complaint->studentID,
                complaint->type,
                complaint->description,
                complaint->priority,
                complaint->status,
                complaint->comment);
        fclose(file);
    } else {
        perror("Error opening complaints file");
    }
}


// Get the highest complaint ID from the existing complaints
int get_highest_complaint_id() {
    int highestID = 0; // Default to 0 if no complaints exist
    for (int i = 0; i < complaintCount; i++) {
        if (complaints[i].id > highestID) {
            highestID = complaints[i].id;
        }
    }
    return highestID;
}
