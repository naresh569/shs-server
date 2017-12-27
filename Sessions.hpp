
#pragma once
#include "Config.hpp"

class Session {
    public:
        int _id;
        int userId;
        char* token;
        char* timeOfStart;
        bool isValid;

        Session(int);
        ~Session();

        void generateToken();
        char* getToken();

        static int total;
        static Session* createSession(int);
        static void display();
        static int getUserId(char*);
} *sessions[MAXUSERS];

int Session :: total = 0;

Session :: Session(int puserId) {
    _id = total + 1;

    userId = puserId;

    token = "";
    generateToken();

    timeOfStart = "";
    isValid = true;

    total++;
}

Session :: ~Session() {
    total--;
}

void Session :: generateToken() {

    token = new char[SIZENORMAL];
    token[0] = 0;

    long randInt;
    for (int i = 0; i < LENGTHTOKEN; i++) {
        long randInt = random(97, 123);
        concat(token, (char) randInt);
    }

    // Serial.print(" > Generated token: ");
    // Serial.println(token);
}

char* Session :: getToken() {
    return token;
}

Session* Session :: createSession(int puserId) {

    int index = -1;

    for (int i = 0; i < MAXUSERS; i++) {

        // By default, place the session in the first available position
        if (sessions[i] == NULL) {
            // Serial.println(" > Session NULL");
            if (index == -1)
            index = i;
            continue;
        }

        // If session for the user already exists, release that session
        // create new session in that place
        if (sessions[i]->userId == puserId) {
            index = i;
            delete sessions[index];
        }

    }

    if (index == -1) {
        Serial.println(" > ERROR: Limit for max sessions reached..");
        return NULL;
    }

    // Serial.print(" > Placing position for the session: ");
    // Serial.println(index);
    sessions[index] = new Session(puserId);
    return sessions[index];
}

void Session :: display() {
    int t = total;
    Serial.println();
    Serial.print(" > Total sessions: ");
    Serial.println(t);

    if (!t)
    return;

    Serial.println(" # SESSIONS\nSNo\tUserID\tToken");
    short int count = 0; 
    for(int i = 0; i < MAXUSERS; i++) {
        if (sessions[i] == NULL)
        continue;

        count++;
        Serial.print(count);
        Serial.print("\t");
        Serial.print(sessions[i]->userId);
        Serial.print("\t");
        Serial.println(sessions[i]->token);
    }
    Serial.println();
}

int Session :: getUserId(char* tk) {
    if (!tk) {
        Serial.println(" > ERROR: Given token is invalid!");
        return 0;
    }

    int t = total;
    if (!t) {
        Serial.println(" > ERROR: No sessions available!");
        return 0;
    }

    int userId = 0;
    for(int i = 0; i < MAXUSERS; i++) {
        if (sessions[i] == NULL)
        continue;

        if (equals(sessions[i]->token, tk, true)) {
            userId = sessions[i]->userId;
            break;
        }
    }

    return userId;
}
