#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    GET_LINE_SUCCESS,
    GET_LINE_LINE_PTR_NULL,
    // Not really an error. More like a warning that the line was truncated.
    GET_LINE_REALLOCATION_FAILED,
    GET_LINE_SIZE_LESS_THAN_10,
} GetLineError;

/* Similar to https://en.cppreference.com/w/c/experimental/dynamic/getline.
 * The `size` parameter will be the initial size of the buffer (if it is not already allocated),
 * and each time the buffer has to reallocate,
 * the `size` parameter will be added to the old size and that will be the new size.
*/
GetLineError get_line(char **line_ptr, const size_t initial_capacity) {
    if (line_ptr == nullptr) {
        return GET_LINE_LINE_PTR_NULL;
    }

    if (initial_capacity < 10) {
        return GET_LINE_SIZE_LESS_THAN_10;
    }

    if (*line_ptr == nullptr) {
        *line_ptr = malloc(initial_capacity * sizeof(char));
    }

    size_t capacity = initial_capacity;

    for (int i = 0;; i++) {
        char c = (char) getchar();

        if (i > 0 && (i + 1) % initial_capacity == 0) {
            capacity *= 2;
            char *temp_ptr = realloc(*line_ptr, capacity);

            if (temp_ptr == nullptr) {
                return GET_LINE_REALLOCATION_FAILED;
            }

            *line_ptr = temp_ptr;
        }

        if (c == '\n') {
            (*line_ptr)[i] = '\0';
            break;
        } else {
            (*line_ptr)[i] = c;
        }
    }

    return GET_LINE_SUCCESS;
}

// Safely reads one size_t. Returns -1 if there was an error parsing.
ssize_t get_size_from_line() {
    char *line = nullptr;
    // See get_char same line.
    get_line(&line, 10);

    char *end_ptr;
    errno = 0;
    size_t n;
    n = strtol(line, &end_ptr, 10);

    if (errno != 0) {
        return -1;
    }

    return n;
}

typedef struct {
    char *name;
    char *last_name;
    char *email;
} User;

User user_create(char *name, char *last_name, char *email) {
    User user;
    user.name = name;
    user.last_name = last_name;
    user.email = email;
    return user;
}

typedef struct {
    User *users;
    size_t length;
    size_t capacity;
} Userlist;

Userlist userlist_create(size_t capacity) {
    Userlist userlist;
    userlist.users = malloc(capacity * sizeof(User));
    userlist.length = 0;
    userlist.capacity = capacity;
    return userlist;
}

// Returns -1 if the list could not be reallocated.
int userlist_push(Userlist *userlist, User user) {
    size_t length_now = userlist->length + 1;
    size_t i = length_now - 1;
    size_t capacity = userlist->capacity;

    if (length_now > capacity) {
        size_t new_capacity = capacity * 2;
        User *temp_ptr = realloc(userlist->users, new_capacity * sizeof(User));

        if (temp_ptr == nullptr) {
            return -1;
        }

        userlist->capacity = new_capacity;
        userlist->users = temp_ptr;
    }

    userlist->users[i] = user;
    userlist->length++;

    return 0;
}

// Returns -1 if the index is out of bounds.
int userlist_remove(Userlist *userlist, size_t i) {
    if (i >= userlist->length) {
        return -1;
    }

    if (i < userlist->length - 1) {
        for (size_t j = i; j + 1 < userlist->length; j++) {
            userlist->users[j] = userlist->users[j + 1];
        }
    }

    userlist->length--;
    return 0;
}

// Returns -1 if the index is out of bounds.
int userlist_print_at(Userlist *userlist, size_t i) {
    if (i >= userlist->length) {
        return -1;
    }

    User user = userlist->users[i];
    printf("User[%zu]\n\tName: %s\n\tLast name: %s\n\tEmail: %s\n", i, user.name, user.last_name, user.email);
    return 0;
}

void userlist_print_all(Userlist *userlist) {
    for (size_t i = 0; i < userlist->length; i++) {
        userlist_print_at(userlist, i);
    }
}

int handle_get_line(char **line_ptr, size_t size) {
    switch (get_line(line_ptr, size)) {
        case GET_LINE_SUCCESS:
            return 0;
        case GET_LINE_LINE_PTR_NULL:
            fprintf(stderr, "get_line error: LINE_PTR_NULL");
            return -1;
        case GET_LINE_REALLOCATION_FAILED:
            fprintf(stderr, "get_line warning: REALLOCATION_FAILED. Retrieved:%s", *line_ptr);
            return 0;
        case GET_LINE_SIZE_LESS_THAN_10:
            fprintf(stderr, "get_line error: SIZE_LESS_THAN_10");
            return -1;
    }
}

int main() {
    // 30 is the *initial* capacity.
    Userlist userlist = userlist_create(30);

    while (true) {
        printf("Choose action by typing the number:\n"
               "1: EXIT\n"
               "2: Add user\n"
               "3: Remove user\n"
               "4: Print user\n"
               "5: Print all users\n");

        switch (get_size_from_line()) {
            case 1:
                printf("Users:\n");
                userlist_print_all(&userlist);
                return 0;
            case 2:
                printf("Enter the following information for the new user:\nName:");

                char *name = nullptr;

                if (handle_get_line(&name, 20) < 0) {
                    return -1;
                }

                printf("Last name:");

                char *last_name = nullptr;

                if (handle_get_line(&last_name, 20) < 0) {
                    return -1;
                }

                printf("Email:");

                char *email = nullptr;

                if (handle_get_line(&email, 20) < 0) {
                    return -1;
                }

                if (userlist_push(&userlist, user_create(name, last_name, email)) == 0) {
                    printf("User was added.\n");
                } else {
                    printf("Error: Reallocation failed.\n");
                }

                break;
            case 3:
                printf("Which user should be deleted? (index from 0 to %zu)\n", userlist.length - 1);

                if (userlist_remove(&userlist, get_size_from_line()) == 0) {
                    printf("User was removed.\n");
                } else {
                    printf("Error: Index out of bounds.\n");
                }

                break;
            case 4:
                printf("Which user should be printed? (index from 0 to %zu)\n", userlist.length - 1);

                if (userlist_print_at(&userlist, get_size_from_line()) == -1) {
                    printf("Error: Index out of bounds.\n");
                }

                break;
            case 5:
                userlist_print_all(&userlist);
                break;
            default:
                printf("Unknown action.\n");
                break;
        }
    }
}