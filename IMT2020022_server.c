#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<stdlib.h>
#include"states.h"
#include"server-function.h"

int main()
{
    int server_fd = socket(AF_INET,SOCK_STREAM,0);
    if(server_fd == 0)
    {
        perror("Socket Failed:");
        return 1;
    }
    printf("Starting Server\n");
    struct sockaddr_in str;
    str.sin_addr.s_addr = INADDR_ANY;
    str.sin_family = AF_INET;
    str.sin_port = htons(PORT);
    int size = sizeof(str);
    if(bind(server_fd,(struct sockaddr*)&str,size) == -1)
    {
        perror("Bind Failed:");
        return 1;
    }
    if(listen(server_fd,3) == -1)
    {
        perror("Listen Failed:");
        return 1;
    }
    int new_fd;
    while(1)
    {
        new_fd = accept(server_fd,(struct sockaddr*)&str,(socklen_t*)&size);
        if(new_fd == -1)
        {
            perror("Accept Failed:");
            return 1;
        }
        if(!fork())
        {
            close(server_fd);
            handle_requests(new_fd);
            return 0;
        }
        else
        {
            close(new_fd);
        }
    }
    return 0;
}

void handle_requests(int new_fd)
{
    char user[MAX_SIZE],password[MAX_SIZE],buffer[MAX_SIZE];
    float amount;
    int option;
    while(1)
    {
        read(new_fd,&option,sizeof(option));
        
        //Authentication
        if(option == SINGLE_USER)
        {
            read(new_fd,user,sizeof(user));
            read(new_fd,password,sizeof(password));
            int ret = auth_single_user(user,password);
            write(new_fd,&ret,sizeof(ret));
        }
        else if(option == JOINT_USER)
        {
            read(new_fd,user,sizeof(user));
            read(new_fd,password,sizeof(password));
            int ret = auth_joint_user(user,password);
            write(new_fd,&ret,sizeof(ret));
        }
        else if(option == ADMIN)
        {
            read(new_fd,user,sizeof(user));
            read(new_fd,password,sizeof(password));
            int ret = auth_admin_user(user,password);
            write(new_fd,&ret,sizeof(ret));
        }
        // Single User Functions
        else if(option == S_DEPOSIT)
        {
            read(new_fd,&amount,sizeof(amount));
            int ret = s_deposit(user,password,amount);
            write(new_fd,&ret,sizeof(ret));
        }
        else if(option == S_WITHDRAW)
        {
            read(new_fd,&amount,sizeof(amount));
            int ret = s_withdraw(user,password,amount);
            write(new_fd,&ret,sizeof(ret));
        }
        else if(option == S_BALANCE)
        {
            s_balance(user,password,new_fd);
        }
        else if(option == S_PASSWORD)
        {
            read(new_fd,buffer,sizeof(buffer));
            int ret = s_password(user,password,buffer);
            write(new_fd,&ret,sizeof(ret));              
        }
        else if(option == S_VIEW)
        {
            s_view(user,password,new_fd);
        }
        // Joint User Functions
        else if(option == J_DEPOSIT)
        {
            read(new_fd,&amount,sizeof(amount));
            int ret = j_deposit(user,password,amount);
            write(new_fd,&ret,sizeof(ret));
        }
        else if(option == J_WITHDRAW)
        {
            read(new_fd,&amount,sizeof(amount));
            int ret = j_withdraw(user,password,amount);
            write(new_fd,&ret,sizeof(ret));
        }
        else if(option == J_BALANCE)
        {
            j_balance(user,password,new_fd);
        }
        else if(option == J_PASSWORD)
        {
            read(new_fd,buffer,sizeof(buffer));
            int ret = j_password(user,password,buffer);
            write(new_fd,&ret,sizeof(ret));             
        }
        else if(option == J_VIEW)
        {
            j_view(user,password,new_fd);
        }
        // Admin Functions
        else if(option == A_S_ADD)
        {
            a_s_add(new_fd);
        }
        else if(option == A_S_DELETE)
        {
            int ret = a_s_delete(new_fd);
            write(new_fd,&ret,sizeof(ret));
        }
        else if(option == A_S_MODIFY)
        {
            int ret = a_s_modify(new_fd);
            write(new_fd,&ret,sizeof(ret));
        }
        else if(option == A_S_SEARCH)
        {
            a_s_search(new_fd);
        }
        else if(option == A_J_ADD)
        {
            a_j_add(new_fd);
        }
        else if(option == A_J_DELETE)
        {
            int ret = a_j_delete(new_fd);
            write(new_fd,&ret,sizeof(ret));
        }
        else if(option == A_J_MODIFY)
        {
            int ret = a_j_modify(new_fd);
            write(new_fd,&ret,sizeof(ret));
        }
        else if(option == A_J_SEARCH)
        {
            a_j_search(new_fd);
        }
        // Exit
        else if(option == EXIT)
            break;
    }
}

int auth_single_user(char* username,char* password)
{
    int fd = open("single_account",O_CREAT|O_RDWR,0766);
    struct single_user temp;
    while(read(fd,&temp,sizeof(struct single_user)))
    {
        if(temp.flag == PRESENT && !strcmp(temp.holder.username,username) && !strcmp(temp.holder.password,password))
        {
            close(fd);
            return 0;
        }
    }
    close(fd);
    return 1;
}

int lock(int fd,int start,int length,int type)
{
    struct flock temp;
    temp.l_len = length;
    temp.l_pid = getpid();
    temp.l_start = start;
    temp.l_type = type;
    temp.l_whence = SEEK_SET;
    int ret = fcntl(fd,F_SETLKW,&temp);
    return ret;
}

int s_deposit(char* username,char* password,float amount)
{
    int fd = open("single_account",O_RDWR);
    struct single_user temp;
    while(read(fd,&temp,sizeof(temp)))
    {
        if(temp.flag == PRESENT && !strcmp(temp.holder.username,username) && !strcmp(temp.holder.password,password))
        {
            int start = lseek(fd,-sizeof(temp),SEEK_CUR);
            lock(fd,start,sizeof(temp),F_WRLCK);
            temp.balance += amount;
            write(fd,&temp,sizeof(temp));
            lock(fd,start,sizeof(temp),F_UNLCK);
            close(fd);
            return 0;
        }
    }
    close(fd);
    return 1;
}

int s_withdraw(char* username,char* password,float amount)
{
    int fd = open("single_account",O_RDWR);
    struct single_user temp;
    while(read(fd,&temp,sizeof(temp)))
    {
        if(temp.flag == PRESENT && !strcmp(temp.holder.username,username) && !strcmp(temp.holder.password,password))
        {
            int start = lseek(fd,-sizeof(temp),SEEK_CUR);
            lock(fd,start,sizeof(temp),F_WRLCK);
            temp.balance -= amount;
            write(fd,&temp,sizeof(temp));
            lock(fd,start,sizeof(temp),F_UNLCK);
            close(fd);
            return 0;
        }
    }
    close(fd);
    return 1;
}

void s_balance(char* username,char* password,int new_fd)
{
    int fd = open("single_account",O_RDWR);
    struct single_user temp;
    while(read(fd,&temp,sizeof(struct single_user)))
    {
        if(temp.flag == PRESENT && !strcmp(temp.holder.username,username) && !strcmp(temp.holder.password,password))
        {
            write(new_fd,&temp.balance,sizeof(temp.balance));
            break;
        }
    }
    close(fd);
    return;
}

int s_password(char* username,char* password,char* new)
{
    int fd = open("single_account",O_RDWR);
    struct single_user temp;
    while(read(fd,&temp,sizeof(temp)))
    {
        if(temp.flag == PRESENT && !strcmp(temp.holder.username,username) && !strcmp(temp.holder.password,password))
        {
            int start = lseek(fd,-sizeof(temp),SEEK_CUR);
            lock(fd,start,sizeof(temp),F_WRLCK);
            sprintf(temp.holder.password,"%s",new);
            write(fd,&temp,sizeof(temp));
            lock(fd,start,sizeof(temp),F_UNLCK);
            close(fd);
            return 0;
        }
    }
    close(fd);
    return 1;
}

void s_view(char* username,char* password,int new_fd)
{
    int fd = open("single_account",O_RDWR);
    struct single_user temp;
    while(read(fd,&temp,sizeof(struct single_user)))
    {
        if(temp.flag == PRESENT && !strcmp(temp.holder.username,username) && !strcmp(temp.holder.password,password))
        {
            write(new_fd,&temp.account_number,sizeof(temp.balance));
            write(new_fd,temp.holder.username,sizeof(temp.holder.username));
            write(new_fd,temp.holder.dob,sizeof(temp.holder.dob));
            break;
        }
    }
    close(fd);
    return;
}

int auth_joint_user(char* username,char* password)
{
    int fd = open("joint_account",O_CREAT|O_RDWR,0766);
    struct joint_user temp;
    while(read(fd,&temp,sizeof(temp)))
    {
        if(temp.flag == DELETED)
            continue;
        for(int i = 0;i < temp.count;i++)
        {
            if(!strcmp(temp.holders[i].username,username) && !strcmp(temp.holders[i].password,password))
            {
                close(fd);
                return 0;
            }
        }
    }
    close(fd);
    return 1;
}

int j_deposit(char* username,char* password,float amount)
{
    int fd = open("joint_account",O_RDWR);
    struct joint_user temp;
    while(read(fd,&temp,sizeof(temp)))
    {
        if(temp.flag == DELETED)
            continue;
        for(int i = 0;i < temp.count;i++)
        {
            if(!strcmp(temp.holders[i].username,username) && !strcmp(temp.holders[i].password,password))
            {
                int start = lseek(fd,-sizeof(temp),SEEK_CUR);
                lock(fd,start,sizeof(temp),F_WRLCK);
                temp.balance += amount;
                write(fd,&temp,sizeof(temp));
                lock(fd,start,sizeof(temp),F_UNLCK);
                close(fd);
                return 0;
            }
        }
    }
    close(fd);
    return 1;
}

int j_withdraw(char* username,char* password,float amount)
{
    int fd = open("joint_account",O_RDWR);
    struct joint_user temp;
    while(read(fd,&temp,sizeof(temp)))
    {
        if(temp.flag == DELETED)
            continue;
        for(int i = 0;i < temp.count;i++)
        {
            if(!strcmp(temp.holders[i].username,username) && !strcmp(temp.holders[i].password,password))
            {
                int start = lseek(fd,-sizeof(temp),SEEK_CUR);
                lock(fd,start,sizeof(temp),F_WRLCK);
                temp.balance -= amount;
                write(fd,&temp,sizeof(temp));
                lock(fd,start,sizeof(temp),F_UNLCK);
                close(fd);
                return 0;
            }
        }
    }
    close(fd);
    return 1;
}

void j_balance(char* username,char* password,int new_fd)
{
    int fd = open("joint_account",O_RDWR);
    struct joint_user temp;
    while(read(fd,&temp,sizeof(temp)))
    {
        if(temp.flag == DELETED)
            continue;
        for(int i = 0;i < temp.count;i++)
        {
            if(!strcmp(temp.holders[i].username,username) && !strcmp(temp.holders[i].password,password))
            {
                write(new_fd,&temp.balance,sizeof(temp.balance));
                close(fd);
                return;
            }
        }
    }
    close(fd);
    return;
}

int j_password(char* username,char* password,char* new)
{
    int fd = open("joint_account",O_RDWR);
    struct joint_user temp;
    while(read(fd,&temp,sizeof(temp)))
    {
        if(temp.flag == DELETED)
            continue;
        for(int i = 0;i < temp.count;i++)
        {
            if(!strcmp(temp.holders[i].username,username) && !strcmp(temp.holders[i].password,password))
            {
                int start = lseek(fd,-sizeof(temp),SEEK_CUR);
                lock(fd,start,sizeof(temp),F_WRLCK);
                sprintf(temp.holders[i].password,"%s",new);
                write(fd,&temp,sizeof(temp));
                lock(fd,start,sizeof(temp),F_UNLCK);
                close(fd);
                return 0;
            }
        }
    }
    close(fd);
    return 1;
}

void j_view(char* username,char* password,int new_fd)
{
    int fd = open("joint_account",O_RDWR);
    struct joint_user temp;
    while(read(fd,&temp,sizeof(temp)))
    {
        if(temp.flag == DELETED)
            continue;
        for(int i = 0;i < temp.count;i++)
        {
            if(!strcmp(temp.holders[i].username,username) && !strcmp(temp.holders[i].password,password))
            {
                write(new_fd,&temp.account_number,sizeof(temp.account_number));
                write(new_fd,&temp.count,sizeof(temp.count));
                for(int j = 0;j < temp.count;j++)
                {
                    write(new_fd,temp.holders[j].username,sizeof(char) * MAX_SIZE);
                    write(new_fd,temp.holders[j].dob,sizeof(char) * MAX_SIZE);
                }
                return;
            }
        }
    }
    close(fd);
    return;
}

int auth_admin_user(char* username,char* password)
{
    int fd = open("admin",O_CREAT|O_RDWR,0766);
    struct admin temp;
    while(read(fd,&temp,sizeof(temp)))
    {
        if(!strcmp(temp.username,username) && !strcmp(temp.password,password))
        {
            close(fd);
            return 0;
        }
    }
    close(fd);
    return 1;
}

void a_s_add(int new_fd)
{
    int fd = open("single_account",O_RDWR|O_APPEND|O_CREAT,0744);
    struct single_user temp;
    read(new_fd,&temp.account_number,sizeof(temp.account_number));
    read(new_fd,temp.holder.username,sizeof(temp.holder.username));
    read(new_fd,temp.holder.password,sizeof(temp.holder.password));
    read(new_fd,temp.holder.dob,sizeof(temp.holder.dob));
    read(new_fd,&temp.balance,sizeof(temp.balance));
    temp.flag = 0;
    int ret = write(fd,&temp,sizeof(temp));
    if(ret > 0)
        ret = 0;
    else
        ret = 1;
    write(new_fd,&ret,sizeof(ret));
}

int a_s_delete(int new_fd)
{
    char username[MAX_SIZE];
    int fd = open("single_account",O_CREAT|O_RDWR,0766);
    struct single_user temp;
    read(new_fd,username,sizeof(username));
    while(read(fd,&temp,sizeof(temp)))
    {
        if(temp.flag == PRESENT && !strcmp(temp.holder.username,username))
        {
            int start = lseek(fd,-sizeof(temp),SEEK_CUR);
            lock(fd,start,sizeof(temp),F_WRLCK);
            temp.flag = 1;
            write(fd,&temp,sizeof(temp));
            lock(fd,start,sizeof(temp),F_UNLCK);
            close(fd);
            return 0;
        }
    }
    close(fd);
    return 1;
}

int a_s_modify(int new_fd)
{
    char username[MAX_SIZE],new[MAX_SIZE];
    int fd = open("single_account",O_RDWR|O_CREAT,0766);
    struct single_user temp;
    read(new_fd,username,sizeof(username));
    while(read(fd,&temp,sizeof(temp)))
    {
        if(temp.flag == PRESENT && !strcmp(temp.holder.username,username))
        {
            int start = lseek(fd,-sizeof(temp),SEEK_CUR);
            lock(fd,start,sizeof(temp),F_WRLCK);
            read(new_fd,temp.holder.username,sizeof(char) * MAX_SIZE);
            write(fd,&temp,sizeof(temp));
            lock(fd,start,sizeof(temp),F_UNLCK);
            close(fd);
            return 0;
        }
    }
    close(fd);
    return 1;
}

void a_s_search(int new_fd)
{
    char username[MAX_SIZE];
    struct single_user temp;
    int fd = open("single_account",O_RDONLY|O_CREAT,0766);
    read(new_fd,username,sizeof(username));
    while(read(fd,&temp,sizeof(temp)))
    {
        if(temp.flag == PRESENT && !strcmp(temp.holder.username,username))
        {
            write(new_fd,&temp.account_number,sizeof(temp.account_number));
            write(new_fd,temp.holder.username,sizeof(username));
            write(new_fd,&temp.balance,sizeof(temp.balance));
            close(fd);
            return;
        }
    }
    close(fd);
    fd = -1;
    write(new_fd,&fd,sizeof(fd));
    return;
}

void a_j_add(int new_fd)
{
    int fd = open("joint_account",O_WRONLY|O_APPEND|O_CREAT,0744);
    struct joint_user temp;
    read(new_fd,&temp.account_number,sizeof(temp.account_number));
    read(new_fd,&temp.count,sizeof(temp.count));
    for(int i = 0;i < temp.count;i++)
    {
        read(new_fd,temp.holders[i].username,sizeof(char) * MAX_SIZE);
        read(new_fd,temp.holders[i].password,sizeof(char) * MAX_SIZE);
        read(new_fd,temp.holders[i].dob,sizeof(char) * MAX_SIZE);
    }
    read(new_fd,&temp.balance,sizeof(temp.balance));
    int ret = write(fd,&temp,sizeof(temp));
    if(ret > 0)
        ret = 0;
    else
        ret = 1;
    write(new_fd,&ret,sizeof(ret));
}

int a_j_delete(int new_fd)
{
    char username[MAX_SIZE];
    struct joint_user temp;
    int fd = open("joint_account",O_RDWR|O_CREAT,0766);
    read(new_fd,username,sizeof(username));
    while(read(fd,&temp,sizeof(temp)))
    {
        if(temp.flag == DELETED)
            continue;
        for(int i = 0;i < temp.count;i++)
        {
            if(!strcmp(temp.holders[i].username,username))
            {
                int start = lseek(fd,-sizeof(temp),SEEK_CUR);
                lock(fd,start,sizeof(temp),F_WRLCK);
                temp.flag = 1;
                write(fd,&temp,sizeof(temp));
                lock(fd,start,sizeof(temp),F_UNLCK);
                close(fd);
                return 0;
            }
        }
    }
    close(fd);
    return 1;
}
int a_j_modify(int new_fd)
{
    char username[MAX_SIZE];
    int fd = open("joint_account",O_RDWR|O_CREAT,0766);
    struct joint_user temp;
    read(new_fd,username,sizeof(username));
    while(read(fd,&temp,sizeof(temp)))
    {
        if(temp.flag == 1)
            continue;
        for(int i = 0;i < temp.count;i++)
        {
            if(!strcmp(temp.holders[i].username,username))
            {
                int start = lseek(fd,-sizeof(temp),SEEK_CUR);
                lock(fd,start,sizeof(temp),F_WRLCK);
                read(new_fd,temp.holders[i].username,sizeof(char) * MAX_SIZE);
                write(fd,&temp,sizeof(temp));
                lock(fd,start,sizeof(temp),F_UNLCK);
                close(fd);
                return 0;
            }
        }
    }
    close(fd);
    return 1;
}
void a_j_search(int new_fd)
{
    char username[MAX_SIZE];
    int fd = open("joint_account",O_RDONLY|O_CREAT,0766);
    struct joint_user temp;
    read(new_fd,username,sizeof(username));
    while(read(fd,&temp,sizeof(temp)))
    {
        if(temp.flag == 1)
            continue;
        for(int i = 0;i < temp.count;i++)
        {
            if(!strcmp(temp.holders[i].username,username))
            {
                write(new_fd,&temp.account_number,sizeof(temp.account_number));
                write(new_fd,&temp.count,sizeof(temp.count));
                for(int j = 0;j < temp.count;j++)
                    write(new_fd,temp.holders[j].username,sizeof(char) * MAX_SIZE);
                write(new_fd,&temp.balance,sizeof(temp.balance));
                return;
            }
        }
    }
    close(fd);
    fd = -1;
    write(new_fd,&fd,sizeof(fd));
    return;
}