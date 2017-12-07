
#pragma once
#include "Config.hpp"

class User {
    public:
        int _id;
        char* name;
        char* username;
        char* pin;
        char* deviceUUId;
        short int accessLevel;
        bool deactivated;

        static User* addUser(const char *, const char *, const char *, const char *);
        static int total;

        User(const char *pname, const char *puname, const char *ppin, const char *uuid);
        ~User();
} *users[MAXUSERS];

int User :: total = 0;

User :: User (const char *pname, const char *puname, const char *ppin, const char *uuid) {

    _id = total + 1;

    name = new char[SIZENORMAL];
    name[0] = 0;
    concat(name, pname);

    username = new char[SIZENORMAL];
    username[0] = 0;
    concat(username, puname);

    pin = new char[SIZENORMAL];
    pin[0] = 0;
    concat(pin, ppin);

    deviceUUId = new char[SIZENORMAL];
    deviceUUId[0] = 0;
    concat(deviceUUId, uuid);

    // For the first user, admin level access
    if (total == 0) {
        accessLevel = 2;
    } else {
        accessLevel = 1;
    }

    // By default all the users except first user
    // will be deactivated
    if (total == 0) {
        deactivated = false;
    } else {
        deactivated = true;
    }
    
    total++;
}

User :: ~User () {
    // Still pending..
    // Care need to be taken when deleting one object
    // rearranging all the objects in the array - users
    total--;
}

User* User :: addUser(const char *pname, const char *puname, const char *ppin, const char *uuid) {

    // finding if the username or deviceUUId already exists
    short int index = -1, total = User :: total;
    for (int i = 0; i < total; i++) {
        if (equals(users[i]->deviceUUId, uuid, true)) {
            index = i;
            break; // first occurence
        }
        if (equals(users[i]->username, puname, true)) {
            index = i;
            break; // first occurence
        }
    }

    if (index > -1) {
        // User already exists
        // return 1;
        return NULL;
    }

    if (total == MAXUSERS) {
        // No place for new user
        // return 2;
        return NULL;
    }

    users[total] = new User (pname, puname, ppin, uuid);
    return users[total];

}
