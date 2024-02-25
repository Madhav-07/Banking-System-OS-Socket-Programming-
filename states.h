
// Port Number
#define PORT 8081

// Max Size of char array
#define MAX_SIZE 30

// Authentication
#define SINGLE_USER 1
#define JOINT_USER 2
#define ADMIN 3

// Exit Signal
#define EXIT 4

// Single User Functions
#define S_DEPOSIT 5
#define S_WITHDRAW 6
#define S_BALANCE 7
#define S_PASSWORD 8
#define S_VIEW 9

// Joint User Functions
#define J_DEPOSIT 10
#define J_WITHDRAW 11
#define J_BALANCE 12
#define J_PASSWORD 13
#define J_VIEW 14

// Admin Functions
#define A_S_ADD 15
#define A_S_DELETE 16
#define A_S_MODIFY 17
#define A_S_SEARCH 18
#define A_J_ADD 19
#define A_J_DELETE 20
#define A_J_MODIFY 21
#define A_J_SEARCH 22

// Records
#define PRESENT 0
#define DELETED 1

// Boolean
#define SUCCESS 0
#define FAIL 1

// Structures

struct user
{
    char username[MAX_SIZE];
    char password[MAX_SIZE];
    char dob[MAX_SIZE];
};

struct single_user
{
    int account_number;
    struct user holder;
    float balance;
    int flag;   // 0 = Present, 1 = Delete
};

struct joint_user
{
    int count;
    int account_number;
    struct user holders[4];
    float balance;
    int flag;   // 0 = Present, 1 = Delete
};

struct admin
{
    int admin_number;
    char username[MAX_SIZE];
    char password[MAX_SIZE];
};