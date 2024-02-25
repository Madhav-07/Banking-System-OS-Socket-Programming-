#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<string.h>
#include<stdlib.h>
#include"states.h"
#include"client-function.h"

int main()
{
    int client_fd = socket(AF_INET,SOCK_STREAM,0);
    if(client_fd == -1)
    {
        perror("Socket Failed:");
        return 1;
    }
    printf("Starting Client\n");
    struct sockaddr_in str;
    str.sin_family = AF_INET;
    str.sin_port = htons(PORT);
    if(inet_pton(AF_INET,"127.0.0.1",&str.sin_addr) <= 0)
    {
        perror("IP Address Problem:");
        return 1;
    }
    if(connect(client_fd,(struct sockaddr*)&str,sizeof(str)) == -1)
    {
        perror("Connect Failed:");
        return 1;
    }
    start_menu(client_fd);
    return 0;
}

void start_menu(int client_fd)
{
    int option,value;
    while(1)
    {
        printf("\nLogin Menu\n");
        printf("1.Single User\n");
        printf("2.Joint User\n");
        printf("3.Administrator\n");
        printf("4.Exit\n");
        printf("Enter Your Choice:");
        scanf("%d",&option);
        switch(option)
        {
            case 1:
                single_user(client_fd);
                break;
            case 2:
                joint_user(client_fd);
                break;
            case 3:
                admin(client_fd);
                break;
            case 4:
                value = EXIT;
                write(client_fd,&value,sizeof(value));
                return;
            default:
                printf("\nEnter Valid Option\n");
                break;
        }
    }
}

void single_user(int client_fd)
{
    char user[MAX_SIZE],password[MAX_SIZE],temp;
    int ret,value = SINGLE_USER;
    printf("\nEnter Username:");
    scanf("%s",user);
    printf("Enter Password:");
    scanf("%s",password);
    write(client_fd,&value,sizeof(value));
    write(client_fd,user,sizeof(user));
    write(client_fd,password,sizeof(password));
    read(client_fd,&ret,sizeof(ret));
    if(ret == SUCCESS)
    {
        printf("\nLogin Successful\n");
        single_user_menu(client_fd);
    }
    else if(ret == FAIL)
    {
        printf("\nUsername or Password is incorrect\n");
        printf("\nWould you like to try again[Y/N]:");
        scanf("%c",&temp);
        scanf("%c",&temp);
        if(temp == 'y' || temp == 'Y')
            single_user(client_fd);
    }
}

void joint_user(int client_fd)
{
    char user[MAX_SIZE],password[MAX_SIZE],temp;
    int ret,value = JOINT_USER;
    printf("\nEnter Username:");
    scanf("%s",user);
    printf("Enter Password:");
    scanf("%s",password);
    write(client_fd,&value,sizeof(value));
    write(client_fd,user,sizeof(user));
    write(client_fd,password,sizeof(password));
    read(client_fd,&ret,sizeof(ret));
    if(ret == SUCCESS)
    {
        printf("\nLogin Successful\n");
        joint_user_menu(client_fd);
    }
    else if(ret == FAIL)
    {
        printf("\nUsername or Password is incorrect\n");
        printf("\nWould you like to try again[Y/N]:");
        scanf("%c",&temp);
        scanf("%c",&temp);
        if(temp == 'y' || temp == 'Y')
            joint_user(client_fd);
    }
}

void admin(int client_fd)
{
    char user[MAX_SIZE],password[MAX_SIZE],temp;
    int ret,value = ADMIN;
    printf("\nEnter Username:");
    scanf("%s",user);
    printf("Enter Password:");
    scanf("%s",password);
    write(client_fd,&value,sizeof(value));
    write(client_fd,user,sizeof(user));
    write(client_fd,password,sizeof(password));
    read(client_fd,&ret,sizeof(ret));
    if(ret == SUCCESS)
    {
        printf("\nLogin Successful\n");
        admin_menu(client_fd);
    }
    else if(ret == FAIL)
    {
        printf("\nUsername or Password is incorrect\n");
        printf("\nWould you like to try again[Y/N]:");
        scanf("%c",&temp);
        scanf("%c",&temp);
        if(temp == 'y' || temp == 'Y')
            admin(client_fd);
    }
}

void single_user_menu(int client_fd)
{
    int option,value,ret;
    char message[MAX_SIZE];
    float amount;
    while(1)
    {
        printf("\nSingle User Menu\n");
        printf("1.Deposit\n");
        printf("2.Withdraw\n");
        printf("3.Balance Enquiry\n");
        printf("4.Password Change\n");
        printf("5.View Details\n");
        printf("6.Exit\n");
        printf("Enter Your Choice:");
        scanf("%d",&option);
        switch(option)
        {
            case 1:
                value = S_DEPOSIT;
                write(client_fd,&value,sizeof(value));
                printf("\nEnter Amount to be Deposited:");
                scanf("%f",&amount);
                write(client_fd,&amount,sizeof(amount));
                read(client_fd,&ret,sizeof(ret));
                if(ret == SUCCESS)
                    printf("\nDeposit Success\n");
                else if(ret == FAIL)
                    printf("\nDeposit Failed\n");
                break;
            case 2:
                value = S_WITHDRAW;
                write(client_fd,&value,sizeof(value));
                printf("\nEnter Amount to be Withdrawed:");
                scanf("%f",&amount);
                write(client_fd,&amount,sizeof(amount));
                read(client_fd,&ret,sizeof(ret));
                if(ret == SUCCESS)
                    printf("\nWithdraw Successful\n");
                else if(ret == FAIL)
                    printf("\nWithdraw Failed\n");
                break;
            case 3:
                value = S_BALANCE;
                write(client_fd,&value,sizeof(value));
                read(client_fd,&amount,sizeof(amount));
                printf("\nYour Balance:%0.2f\n",amount);
                break;
            case 4:
                value = S_PASSWORD;
                write(client_fd,&value,sizeof(value));
                printf("\nEnter New Password:");
                scanf("%s",message);
                write(client_fd,message,sizeof(message));
                read(client_fd,&ret,sizeof(ret));
                if(ret == SUCCESS)
                    printf("\nPassword Change Succesful\n");
                else if(ret == FAIL)
                    printf("\nPassword Change Failed\n");
                break;
            case 5:
                value = S_VIEW;
                write(client_fd,&value,sizeof(value));
                printf("\nYour Details:\n");
                read(client_fd,&value,sizeof(value));
                printf("Account Number:%d\n",value);
                read(client_fd,message,sizeof(message));
                printf("Holder:%s\n",message);
                read(client_fd,message,sizeof(message));
                printf("Date Of Birth:%s\n",message);
                break;
            case 6:
                return;
            default:
                printf("\nEnter Valid Option\n");
                break;
        }
    }
}

void joint_user_menu(int client_fd)
{
    int option,count,value,ret;
    char message[MAX_SIZE];
    float amount;
    while(1)
    {
        printf("\nJoint User Menu\n");
        printf("1.Deposit\n");
        printf("2.Withdraw\n");
        printf("3.Balance Enquiry\n");
        printf("4.Password Change\n");
        printf("5.View Details\n");
        printf("6.Exit\n");
        printf("Enter Your Choice:");
        scanf("%d",&option);
        switch(option)
        {
            case 1:
                value = J_DEPOSIT;
                write(client_fd,&value,sizeof(value));
                printf("\nEnter Amount to be Deposited:");
                scanf("%f",&amount);
                write(client_fd,&amount,sizeof(amount));
                read(client_fd,&ret,sizeof(ret));
                if(ret == SUCCESS)
                    printf("\nDeposit Success\n");
                else if(ret == FAIL)
                    printf("\nDeposit Failed\n");
                break;
            case 2:
                value = J_WITHDRAW;
                write(client_fd,&value,sizeof(value));
                printf("\nEnter Amount to be Withdrawed:");
                scanf("%f",&amount);
                write(client_fd,&amount,sizeof(amount));
                read(client_fd,&ret,sizeof(ret));
                if(ret == SUCCESS)
                    printf("\nWithdraw Successful\n");
                else if(ret == FAIL)
                    printf("\nWithdraw Failed\n");
                break;
            case 3:
                value = J_BALANCE;
                write(client_fd,&value,sizeof(value));
                read(client_fd,&amount,sizeof(amount));
                printf("\nYour Balance:%0.2f\n",amount);
                break;
            case 4:
                value = J_PASSWORD;
                write(client_fd,&value,sizeof(value));
                printf("\nEnter New Password:");
                scanf("%s",message);
                write(client_fd,message,sizeof(message));
                read(client_fd,&ret,sizeof(ret));
                if(ret == SUCCESS)
                    printf("\nPassword Change Succesful\n");
                else if(ret == FAIL)
                    printf("\nPassword Change Failed\n");
                break;
            case 5:
                value = J_VIEW;
                write(client_fd,&value,sizeof(value));
                printf("\nYour Details:\n");
                read(client_fd,&value,sizeof(value));
                printf("Account Number:%d\n",value);
                read(client_fd,&count,sizeof(count));
                for(int i = 0;i < count;i++)
                {
                    read(client_fd,message,sizeof(message));
                    printf("Holder %d:%s\n",i+1,message);
                    read(client_fd,message,sizeof(message));
                    printf("Date Of Birth:%s\n",message);
                }
                break;
            case 6:
                return;
            default:
                printf("\nEnter Valid Option\n");
                break;
        }
    }
}

void admin_menu(int client_fd)
{
    int option,temp,value,ret;
    char message[MAX_SIZE];
    float amount;
    while(1)
    {
        printf("\nAdmin Menu\n");
        printf("1.Add Single User Account\n");
        printf("2.Delete Single User Account\n");
        printf("3.Modify Single User Account\n");
        printf("4.Search Single User Account\n");
        printf("5.Add Joint User Account\n");
        printf("6.Delete Joint User Account\n");
        printf("7.Modify Joint User Account\n");
        printf("8.Search Joint User Account\n");
        printf("9.Exit\n");
        printf("Enter Your Choice:");
        scanf("%d",&option);
        switch(option)
        {
            case 1:
                value = A_S_ADD;
                write(client_fd,&value,sizeof(value));
                printf("\nEnter Account Number:");
                scanf("%d",&value);
                write(client_fd,&value,sizeof(value));
                printf("Enter Username:");
                scanf("%s",message);
                write(client_fd,message,sizeof(message));
                printf("Enter Password:");
                scanf("%s",message);
                write(client_fd,message,sizeof(message));
                printf("Enter Date Of Birth:");
                scanf("%s",message);
                write(client_fd,message,sizeof(message));
                printf("Enter Balance:");
                scanf("%f",&amount);
                write(client_fd,&amount,sizeof(amount));
                read(client_fd,&ret,sizeof(ret));
                if(ret == SUCCESS)
                    printf("\nSingle User Account Add Successful\n");
                else if(ret == FAIL)
                    printf("\nSingle User Account Add Failed\n");
                break;
            case 2:
                value = A_S_DELETE;
                write(client_fd,&value,sizeof(value));
                printf("\nEnter Username:");
                scanf("%s",message);
                write(client_fd,message,sizeof(message));
                read(client_fd,&ret,sizeof(ret));
                if(ret == SUCCESS)
                    printf("\nSingle User Account Deletion Successful\n");
                else if(ret == FAIL)
                    printf("\nSingle User Account Deletion Failed\n");
                break;
            case 3:
                value = A_S_MODIFY;
                write(client_fd,&value,sizeof(value));
                printf("\nEnter Old Username:");
                scanf("%s",message);
                write(client_fd,message,sizeof(message));
                printf("Enter New Username:");
                scanf("%s",message);
                write(client_fd,message,sizeof(message));
                read(client_fd,&ret,sizeof(ret));
                if(ret == SUCCESS)
                    printf("\nSingle User Account Modification Successful\n");
                else if(ret == FAIL)
                    printf("\nSingle User Account Modification Failed\n");
                break;
            case 4:
                value = A_S_SEARCH;
                write(client_fd,&value,sizeof(value));
                printf("\nEnter Username:");
                scanf("%s",message);
                write(client_fd,message,sizeof(message));
                read(client_fd,&temp,sizeof(temp));
                if(temp == -1)
                {
                    printf("\nNo Single Account Found\n");
                    break;
                }
                printf("\nAccount Number:%d\n",temp);
                read(client_fd,message,sizeof(message));
                printf("Holder:%s\n",message);
                read(client_fd,&amount,sizeof(message));
                printf("Balance:%0.2f\n",amount);
                break;
            case 5:
                printf("\nEnter Number of Joint Users[2-4]:");
                scanf("%d",&temp);
                if(temp > 4)
                {
                    printf("\nMax Number of Joint Users is 4\n");
                    return;
                }
                else if(temp < 2)
                {
                    printf("\nMin Number of Joint Users is 2\n");
                }
                value = A_J_ADD;
                write(client_fd,&value,sizeof(value));
                printf("\nEnter Account Number:");
                scanf("%d",&value);
                write(client_fd,&value,sizeof(value));
                write(client_fd,&temp,sizeof(temp));
                for(int i = 0;i < temp;i++)
                {
                    printf("User %d:\n",i+1);
                    printf("Enter Username:");
                    scanf("%s",message);
                    write(client_fd,message,sizeof(message));
                    printf("Enter Password:");
                    scanf("%s",message);
                    write(client_fd,message,sizeof(message));
                    printf("Enter Date Of Birth:");
                    scanf("%s",message);
                    write(client_fd,message,sizeof(message));
                }
                printf("Enter Balance:");
                scanf("%f",&amount);
                write(client_fd,&amount,sizeof(amount));
                read(client_fd,&ret,sizeof(ret));
                if(ret == SUCCESS)
                    printf("\nJoint User Account Add Successful\n");
                else if(ret == FAIL)
                    printf("\nSingle User Account Add Failed\n");
                break;
            case 6:
                value = A_J_DELETE;
                write(client_fd,&value,sizeof(value));
                printf("\nEnter Joint Username:");
                scanf("%s",message);
                write(client_fd,message,sizeof(message));
                read(client_fd,&ret,sizeof(ret));
                if(ret == SUCCESS)
                    printf("\nJoint User Account Deletion Successful\n");
                else if(ret == FAIL)
                    printf("\nJoint User Account Deletion Failed\n");
                break;
            case 7:
                value = A_J_MODIFY;
                write(client_fd,&value,sizeof(value));
                printf("\nEnter Username:");
                scanf("%s",message);
                write(client_fd,message,sizeof(message));
                printf("Enter New Username:");
                scanf("%s",message);
                write(client_fd,message,sizeof(message));
                read(client_fd,&ret,sizeof(ret));
                if(ret == SUCCESS)
                    printf("\nSingle User Account Modification Successful\n");
                else if(ret == FAIL)
                    printf("\nSingle User Account Modification Failed\n");
                break;
            case 8:
                value = A_J_SEARCH;
                write(client_fd,&value,sizeof(value));
                printf("\nEnter Username:");
                scanf("%s",message);
                write(client_fd,message,sizeof(message));
                read(client_fd,&value,sizeof(value));
                if(value == -1)
                {
                    printf("\nNo Joint Account Found\n");
                    break;
                }
                printf("\nAccount Number:%d\n",value);
                read(client_fd,&value,sizeof(value));
                for(int i = 0;i < value;i++)
                {
                    read(client_fd,message,sizeof(message));
                    printf("Holder %d:%s\n",i+1,message);
                }
                read(client_fd,&amount,sizeof(amount));
                printf("Balance:%0.2f\n",amount);
                break;
            case 9:
                return;
            default:
                printf("\nEnter Valid Option\n");
                break;
        }
    }
}