#include "HTTP.hpp"

void HTTP :: process() {

if (equals(method, "OPTIONS", false)) {
    send(200);
    return;
}

if (equals(method, "POST", false) && compareURI("/register/")) {
    StaticJsonBuffer<256> json; // 4 strings - 4*64 bytes
    JsonObject& data = json.parseObject(reqBody);

    const char* name = data["name"];
    const char* username = data["username"];
    const char* pin = data["pin"];
    const char* deviceUUId = data["deviceUUId"];

    if (!name || !username || !pin || !deviceUUId) {
        // send(400);
        send("{\"success\": false, \"message\": \"Registration Failed!\"}");
        return;
    }

    // short int newIndex = User :: total; // some problem..
    User* usr = User :: addUser(name, username, pin, deviceUUId);
    if (usr) {
        send("{\"success\": true, \"message\": \"Registration Successful!\"}");
    } else {
        send("{\"success\": false, \"message\": \"Registration Failed!\"}");
    }

    // I put the below code for debugging purpose
    // Serial.println(" > USERS: ");
    // for (int i = 0; i < User :: total; i++) {
    //     Serial.println(users[i]->name);
    // }
    return;
}

if (equals(method, "POST", false) && compareURI("/authenticate/")) {
    StaticJsonBuffer<256> json; // 4 strings - 4*64 bytes
    JsonObject& data = json.parseObject(reqBody);

    const char* pin = data["userPin"];
    const char* deviceUUId = data["deviceUUId"];

    if (pin == NULL || deviceUUId == NULL) {
        send(400);
        return;
    }

    // Serial.println(pin);
    // Serial.println(deviceUUId);

    short int index = -1;
    for (int i = 0; i < User :: total; i++) {
        if (equals(users[i]->deviceUUId, deviceUUId, true)) {
            // Serial.println(" > User found..");
            // Serial.print(" > user: ");
            // Serial.println(users[i]->name);
            index = i;
            break; // first occurence
        }
    }

    if (index < 0) {
        // User not found
        send("{\"success\": false, \"message\": \"User Does Not Exist.\"}");
        // send("{\"success\": false, \"message\": \"Invalid User Credentials.\"}");
        return;
    }

    if (!equals(users[index]->pin, pin, true)) {
        // Invalid pin
        send("{\"success\": false, \"message\": \"Invalid User Credentials.\"}");
        return;
    }

    if (users[index]->deactivated) {
        // User is deactivated
        send("{\"success\": false, \"message\": \"Inactive User ID. Please Contact ADMIN.\"}");
        return;
    }

    Serial.println(" > Login successful..");
    Session* s = Session :: createSession(users[index]->_id);
    char* token = s->getToken();
    Session :: display();
    write("{\"success\":true, \"message\": \"Login Successful.\", ");
    write("\"token\": \"");
    write(token);
    write("\", \"userId\": ");
    write(users[index]->_id);
    write("}");
    write("");
    return;
}

if (equals(method, "GET", false) && compareURI("/overview/")) {
    write("{\"switches\":{\"sActive\":");
    int total = Switch :: total;
    int active = 0;
    for (int i = 0; i < total; i++) {
        if (switches[i]->stat > '0')
        active++;
    }
    write(active);
    write(",\"sTotal\":");
    write(total);
    write("}}");
    write("");
    return;
}

if (equals(method, "GET", false) && compareURI("/blocks/")) {
    write("[");
    short int total = Block :: total;
    for (short int i = 0; i < total; i++) {
        write("{\"bid\": ");
        write(blocks[i]->bid);
        write(", \"label\": \"");
        write(blocks[i]->label);
        write("\", \"desc\": \"");
        write(blocks[i]->desc);
        write("\", \"switchIds\": [");
        int sTotal = blocks[i]->sTotal;
        int sActive = 0;
        for (int j = 0; j < sTotal; j++) {
            int sid = blocks[i]->switchIds[j];
            write(sid);
            if (switches[sid - 1]->stat > '0')
            sActive++;
            if(j != sTotal-1)
            write(",");
        }
        write("], \"sTotal\": ");
        write(sTotal);
        write(", \"sActive\": ");
        write(sActive);
        write("}");
        if(i != total-1)
        write(",");
    }
    write("]");

    write(""); // Note: Chunked transfer of DATA always end with EMPTY line
    return;
}

if (equals(method, "GET", false) && compareURI("/blocks/:bid/")) {
    char *id = getParamValue("bid");
    int bid;
    if (*id) {
        bid = toInt(id);
    }
    Serial.print(" > Switches for block - ");
    Serial.println(bid);

    short int index = bid - 1;
    write("{\"bid\": ");
    write(blocks[index]->bid);
    write(", \"label\": \"");
    write(blocks[index]->label);
    write("\", \"desc\": \"");
    write(blocks[index]->desc);
    write("\", \"switchIds\": [");
    int sTotal = blocks[index]->sTotal;
    int sActive = 0;
    for (int j = 0; j < sTotal; j++) {
        int sid = blocks[index]->switchIds[j];
        write(sid);
        if(j != sTotal - 1)
        write(",");
        if (switches[sid - 1]->stat > '0')
        sActive++;
    }
    write("], \"sTotal\": ");
    write(sTotal);
    write(", \"sActive\": ");
    write(sActive);
    write("}");

    write(""); // Note: Chunked transfer of DATA always end with EMPTY line
    return;
}

if (equals(method, "GET", false) && compareURI("/blocks/:bid/switches/")) {
    char *id = getParamValue("bid");
    int bid;
    if (*id) {
        bid = toInt(id);
    }
    write("[");
    short int index = bid - 1;
    int sTotal = blocks[index]->sTotal;
    for(int j = 0; j < sTotal; j++){
        int sid = blocks[index]->switchIds[j];

        // index of a switch with perticular sid
        // will be (sid - 1)
        int index = sid - 1;
        write(switches[index]->getDetails());
        if(j != sTotal-1)
        write(",");
    }
    write("]");
    write(""); // Note: Chunked transfer of DATA always end with EMPTY line
    return;
}

if (equals(method, "GET", false) && compareURI("/switches/")) {

    write("[");
    int total = Switch :: total;
    for(int i = 0; i < total; i++){
        write(switches[i]->getDetails());
        if(i != total-1)
        write(",");
    }
    write("]");

    write(""); // Note: Chunked transfer of DATA always end with EMPTY line
    return;
}

if (equals(method, "GET", false) && compareURI("/switches/:id/")) {
    char *id = getParamValue("id");
    if (id == NULL) {
        send(400);
        return;
    }
    int sid = toInt(id);
    Serial.print(" > GET /switches/");
    Serial.println(sid);

    // index of a switch with perticular sid
    // will be (sid - 1)
    int index = sid - 1;
    int total = Switch :: total;
    if (index < total && index >= 0) {
        send(switches[index]->getDetails());
    } else {
        send(404); // Resourse Not Found
    }
    return;
}

if (equals(method, "PUT", false) && compareURI("/switches/:id/")) {
    char *id = getParamValue("id");
    int sid;
    if (*id) {
        sid = toInt(id);
    }
    // Serial.print(" > PUT /switches/");
    // Serial.println(sid);

    // index of a switch with perticular sid
    // will be (sid - 1)
    int index = sid - 1;
    int total = Switch :: total;
    if (index < total && index >= 0) {
        Serial.println(reqBody);
        if (equals(reqBody, "{\"status\":\"5\"}", true)) {
            // Change the switch status to 5 (ON)

            // Notes: Response from switches[INT].changeStatus(CHAR)
            // 0 = No Error
            // 1 = Invalid Status
            // 2 = Switch Locked
            short int r = switches[index]->changeStatus('5');
            if (r == 0) {
                send("{\"success\":true}");
            } else if (r == 1) {
                send("{\"success\":false, \"message\": \"Invalid Status\"}");
            } else if (r == 2) {
                send("{\"success\":false, \"message\": \"Switch Locked\"}");
            }

        } else if (equals(reqBody, "{\"status\":\"0\"}", true)) {
            short int r = switches[index]->changeStatus('0');
            if (r == 0) {
                send("{\"success\":true}");
            } else if (r == 1) {
                send("{\"success\":false, \"message\": \"Invalid Status\"}");
            } else if (r == 2) {
                send("{\"success\":false, \"message\": \"Switch Locked\"}");
            }
        } else {
            send(404);
        }
    } else {
        send(404); // Resourse Not Found
    }
    return;
}

if (equals(method, "GET", false) && compareURI("/users/")) {

    write("[");
    short int total = User :: total;
    for (int i = 0; i < total; i++) {
        write("{");
        write("\"_id\": ");
        write(users[i]->_id);
        write(", \"name\": \"");
        write(users[i]->name);
        write("\", \"username\": \"");
        write(users[i]->username);
        write("\", \"accessLevel\": ");
        write(users[i]->accessLevel);
        write(", \"deactivated\": ");
        if (users[i]->deactivated) {
            write("true");
        } else {
            write("false");
        }
        write(", \"pin\": \"");
        write(users[i]->pin);
        write("\"");
        write("}");

        if(i != total-1)
        write(", ");
    }
    write("]");

    write(""); // Note: Chunked transfer of DATA always end with EMPTY line
    return;
}

if (equals(method, "PUT", false) && compareURI("/users/")) {
    StaticJsonBuffer<256> json; // 4 strings - 4*64 bytes
    JsonObject& data = json.parseObject(reqBody);

    const char* name = data["name"];
    const char* username = data["username"];
    const char* pin = data["pin"];
    const char* deviceUUId = data["deviceUUId"];
    Serial.println(name);
    Serial.println(username);
    Serial.println(pin);
    Serial.println(deviceUUId);

    if (!name || !username || !pin || !deviceUUId) {
        // send(400);
        send("{\"success\": false, \"message\": \"Registration Failed!\"}");
        return;
    }

    User* usr = User :: addUser(name, username, pin, deviceUUId);
    if (!usr) { // no error
        write("{\"success\": true, \"message\": \"User Added Successfully!\", \"id\":"); // 
        short int pos = User :: total; // position always will the index + 1
        write(pos);
        write("}");
        write(""); // Note: Chunked transfer of DATA always end with EMPTY line
    } else {
        send("{\"success\": false, \"message\": \"Registration Failed!\"}");
    }
    // } else if (user == 1) {
    //     send("{\"success\": false, \"message\": \"User Already Exists!\"}");
    // } else if (user == 2) {
    //     send("{\"success\": false, \"message\": \"Max Users Reached!\"}");
    // }
    return;
}

if (equals(method, "GET", false) && compareURI("/users/:id/")) {
    char *id = getParamValue("id");
    int uid;
    if (*id) {
        uid = toInt(id);
    }

    short int index = -1;
    short int total = User :: total;
    for (int i = 0; i < total; i++) {
        if (users[i]->_id == uid) {
            index = i;
        }
    }

    if (index < 0) {
        send(404);
        return;
    }

    write("{");
    write("\"_id\": ");
    write(users[index]->_id);
    write(", \"name\": \"");
    write(users[index]->name);
    write("\", \"username\": \"");
    write(users[index]->username);
    write("\", \"accessLevel\": ");
    write(users[index]->accessLevel);
    write(", \"deactivated\": ");
    if (users[index]->deactivated) {
        write("true");
    } else {
        write("false");
    }
    write("}");
    write(""); // Note: Chunked transfer of DATA always end with EMPTY line
    return;
}

if (equals(method, "PUT", false) && compareURI("/users/:id/")) {
    char *id = getParamValue("id");
    int uid;
    if (*id) {
        uid = toInt(id);
    } else {
        // If user id not there..
        send("{\"success\": false, \"message\": \"User ID Not Found!\"}");
        return;
    }

    // For this method reqBody should be there
    if (reqBody == NULL) {
        send("{\"success\": false, \"message\": \"Operation Failed!\"}");
        return;
    }
    
    short int total = User :: total;
    short int index = uid - 1; // index will be always id - 1
    if (index < 0 || index >= total) {
        // If user id validation fails or user does not exist
        send("{\"success\": false, \"message\": \"User Not Found!\"}");
        return;
    }
    
    bool success = true;

    StaticJsonBuffer<256> json; // 4 strings - 4*64 bytes
    JsonObject& data = json.parseObject(reqBody);

    // UPDATE - pin
    if (success && data.containsKey("newPin")) { // if curPin does not exists, then it will be empty
        const char* curPin = data["curPin"];
        const char* newPin = data["newPin"];
        if (equals(curPin, users[index]->pin, true)) {
            users[index]->pin[0] = 0;
            concat(users[index]->pin, newPin);
            success = true;
        } else {
            success = false;
        }
    }

    // UPDATE - accessLevel
    if (success && data.containsKey("accessLevel")) {
        // accessLevel = 0 means it is not set
        // accessLevel range: [1-9] INCLUDING
        int accessLevel = data["accessLevel"];
        if (accessLevel > 0 && accessLevel < 10) {
            // valid inputs: [1-9] INCLUDING
            users[index]->accessLevel = accessLevel;
            success = true;
        } else {
            success = false;
        }
    }

    // UPDATE - name
    if (success && data.containsKey("name")) {
        const char* name = data["name"];
        users[index]->name[0] = 0;
        concat(users[index]->name, name);
        success = true;
    }

    // UPDATE - deactivated
    if (success && data.containsKey("deactivated")) {
        bool deactivated = data["deactivated"];
        users[index]->deactivated = deactivated;
        success = true;
    }


    if (success) {
        send("{\"success\": true, \"message\": \"Operation Success!\"}");
    } else {
        send("{\"success\": false, \"message\": \"Operation Failed!\"}");
    }
    return;
}

if (equals(method, "DELETE", false) && compareURI("/users/:id/")) {
    char *id = getParamValue("id");
    int uid;
    if (*id) {
        uid = toInt(id);
    } else {
        // If user id not there..
        send("{\"success\": false, \"message\": \"User ID Not Found!\"}");
        return;
    }

    short int total = User :: total;
    short int index = uid - 1; // index will be always id - 1
    if (index < 0 || index >= total) {
        // If user id validation fails or user does not exist
        send("{\"success\": false, \"message\": \"User Not Found!\"}");
        return;
    }

    // *** Logic is pending.. ***
    users[index] = NULL;
    send("{\"success\": true, \"message\": \"Operation Success!\"}");
    return;
}

send(404);
}
