
#pragma once
#include "Config.hpp"

class Switch {
    public:
        int sid;
        char type;
        int pin;
        char stat;
        char *title;
        char *desc;
        bool lock;
        Switch(char, int,  char *, char *);
        ~Switch();

        // Common static variables
        static int total;

        // Normal switch operations
        char* getDetails();
        int changeStatus(char);
        char* changeLockStatus(bool);

        // Links to prev and next switches
        // Double linked list implementation is used
        bool isFirst = false;
        Switch *prev = NULL;
        Switch *next = NULL;

        static void unlink(Switch *);
        static void link(Switch *, char);
        static char* display(char);

        static Switch* addSwitch(char ptype, int ppin, char *ptitle, char *pdesc);
} *status[6], *switches[MAXSWITCHES];

int Switch :: total = 0;

Switch :: Switch(char ptype, int ppin, char *ptitle, char *pdesc){ // params

    // Settting the given parameters as Details of the switch
    sid = total + 1;
    type = ptype;

    // Configuring the pin for OUTPUT
    pin = ppin;
    pinMode(pin, OUTPUT);

    // Implementation is as per the Rule - 1
    // If you change the below three lines, please update the Rule - 1 as well
    lock = false;
    changeStatus('0');
    // lock = plock;

    title = ptitle;
    desc = pdesc;

    total++;
}

Switch :: ~Switch() {
    total--;
}

char* Switch :: getDetails(){

    static char result[200] = "";
    result[0] = 0;
    
    concat(result, "{\"sid\": ");
    concat(result, sid); 
    concat(result, ", \"type\": \"");
    concat(result, type);
    concat(result, "\", \"pin\": ");
    concat(result, pin);
    concat(result, ", \"status\": \"");
    concat(result, stat);
    concat(result, "\", \"lock\": ");
    if (lock) {
        concat(result, "true"); 
    } else {
        concat(result, "false"); 
    }
    concat(result, ", \"title\": \"");
    concat(result, title);
    concat(result, "\", \"desc\": \"");
    concat(result, desc);
    concat(result, "\"");
    concat(result, "}");
    return result;

}

int Switch :: changeStatus(char s){

    // Notes: Regarding Response
    // 0 = No Error
    // 1 = Invalid Status
    // 2 = Switch Locked

    if (lock) {
        // If lock is true
        // means the switch is locked
        return 2; // Error - Switch is locked
    }

    if (s < '0' || s > '5') {
        // s is the input which has value
        // between 0 and 5 only (0 and 5 also include)
        return 1; // Error - Invalid status
    }

    // if (stat == s) {
    //     concat(result, "{success: false; message: 'switch already is in same status!'}");
    //     return result;
    // }
    
    unlink(this);
    link(this, s);
    stat = s;

    // REMOVE the below code if it supports
    // BRIGHTNESS functionality
    if (s > '0') {
        digitalWrite(pin, HIGH);
    } else {
        digitalWrite(pin, LOW);
    }
    // END
    
    return 0; // Operation completed successfully
}

char *Switch :: changeLockStatus(bool s){
    
    static char result[64] = "";
    result[0] = 0;
    
    lock = s;
    concat(result, "{\"success\": true; \"message\": \"change of lock status is successful.\"}");
    return result;
}


void Switch :: unlink(Switch *s) {

    if(s->isFirst){
        int index = s->stat - 48;
        status[index] = s->next;
        status[index]->isFirst = true;
    }

    if(s->prev != NULL){
        s->prev->next = s->next;
    }

    if(s->next != NULL){
        s->next->prev = s->prev;
    }

    s->isFirst = false;
    s->prev = NULL;
    s->next = NULL;

}


void Switch :: link(Switch *s, char pstat) {

    int index = pstat - 48; // ASCII value of '0' is 48

    if(status[index] == NULL){
        status[index] = s;
        status[index]->isFirst = true;
        return;
    }

    Switch *temp = status[index];
    while(temp->next != NULL){
        temp = temp->next;
    }
    temp->next = s;
    s->prev = temp;

}

char *Switch :: display(char pstat) {

    static char result[200] = "";
    result[0] = 0;

    int index = pstat - 48; // ASCII value of '0' is 48
    // Serial.print("index ");
    // Serial.print(index);
    Switch *temp = status[index];
    while(temp != NULL){
        // Serial.println(temp->sid);
        concat(result, temp->sid);
        concat(result, '>');
        temp = temp->next;
    }

    return result;
}

Switch* Switch :: addSwitch(char ptype, int ppin, char *ptitle, char *pdesc) {
    short int total = Switch :: total;

    if (total >= MAXSWITCHES) {
        return NULL;
    }

    switches[total] = new Switch (ptype, ppin, ptitle, pdesc);
    return switches[total];
}