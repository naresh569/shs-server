
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
        static User* addUser(int, const char *, const char *, const char *, const char *, int, bool);
        static int total;

        User(int, const char *, const char *, const char *, const char *);
        User (int, const char *, const char *, const char *, const char *, int, bool);
        ~User();
} *users[MAXUSERS];

int User :: total = 0;

User :: User (int puserid, const char *pname, const char *puname, const char *ppin, const char *uuid) {

    _id = puserid;

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

User :: User (int pUserId, const char *pname, const char *puname, const char *ppin, const char *uuid, int pAccessLevel, bool pDeactivated) {

    _id = pUserId;

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

    accessLevel = pAccessLevel;

    deactivated = pDeactivated;

    total++;
}

User :: ~User () {
    // Still pending..
    // Care need to be taken when deleting one object
    // rearranging all the objects in the array - users
    total--;
}

User* User :: addUser(const char *pname, const char *puname, const char *ppin, const char *uuid) {
    
    int index = -1;
    bool found = false;
    for (int i = 0; i < MAXUSERS; i++) {
        if (users[i] == NULL) {
            index = index < 0 ? i : index;
            continue;
        }
        if (equals(users[i]->deviceUUId, uuid, true)) {
            found = true;
            break; // first occurence
        }
        if (equals(users[i]->username, puname, true)) {
            found = true;
            break; // first occurence
        }
    }

    // Check wether the user already exists..
    if (found) {
        Serial.println(" > ERROR: User already exists..");
        return NULL;
    }

    // Check wether the space is available for new user
    if (index == -1 || User :: total == MAXUSERS) {
        Serial.println(" > ERROR: Limit for max users reached..");
        return NULL;
    }

    users[index] = new User (index + 1, pname, puname, ppin, uuid);
    Serial.println(" > SUCCESS: User created successfully..");
    return users[index];

}

User* User :: addUser(int pUserId, const char *pname, const char *puname, const char *ppin, const char *uuid, int pAccessLevel, bool pDeactivated) {

    bool found = false;
    for (int i = 0; i < MAXUSERS; i++) {
        if (users[i] == NULL)
        continue;
        if (equals(users[i]->deviceUUId, uuid, true)) {
            found = true;
            break; // first occurence
        }
        if (equals(users[i]->username, puname, true)) {
            found = true;
            break; // first occurence
        }
    }

    // Check wether the user already exists..
    if (found) {
        Serial.println(" > ERROR: User already exists..");
        return NULL;
    }

    // Check wether the space is available for new user
    if (User :: total == MAXUSERS) {
        Serial.println(" > ERROR: Limit for max users reached..");
        return NULL;
    }

    // If pUserId is available, then the user should be placed in the
    // position (pUserId - 1).
    // Check wether the position available or not
    if (users[pUserId - 1] != NULL) {
        Serial.println(" > ERROR: Requested postion not available..");
        return NULL;
    }
    
    users[pUserId - 1] = new User (pUserId, pname, puname, ppin, uuid, pAccessLevel, pDeactivated);
    Serial.println(" > SUCCESS: User created successfully..");
    return users[pUserId - 1];

}
