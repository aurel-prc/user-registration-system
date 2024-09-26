#include <stdio.h>

typedef struct {
    char* name;
    char* last_name;
    char* email;
    unsigned int age;
} User;

User create_user(char* name, char* last_name, char* email, const unsigned int age) {
    User user;
    user.name = name;
    user.last_name = last_name;
    user.email = email;
    user.age = age;
    return user;
}

int main(void)
{
    printf()
    return 0;
}
