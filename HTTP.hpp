
#pragma once
#include "Config.hpp"
#include "HTTP/Headers.hpp"

class HTTP {
    private:
        EthernetClient *client;

        bool lseek;
        bool chunked;
    public:
        HTTP(EthernetClient *c);
        ~HTTP();

        // HTTP Request
        char method[SIZESHORT];
        char uri[SIZENORMAL]; // Uniform Resource Identifier
        char version[SIZESHORT];

        Headers* headers;
        char* reqBody;

        char params[SIZENORMAL * 2];
        // short int noParams;

        int readRequest();
        char getChar();

        // HTTP Response
        int statusCode;
        char reasonPhrase[SIZENORMAL];
        char resBody[SIZELONG];
        void status(int);
        void send();
        void send(int);
        void send(char *);
        void write(int);
        void write(char *);
        void addHeaders();

        int secure();
        void process();
        char* getParamValue(const char *);
        bool compareURI(const char *);
};

HTTP :: HTTP(EthernetClient *c) {
    client = c;
    lseek = false;
    chunked = false;
    statusCode = 404;

    headers = new Headers;

    reqBody = NULL;

    // noParams = 0;
    params[0] = 0;
    params[1] = 0;

    int resCode = readRequest();
    if (resCode == 200) { // on compilation success
        // Serial.print(" >>> ");
        // Serial.print(method);
        // Serial.print(" ");
        // Serial.print(uri);
        // Serial.print(" ");
        // Serial.println(version);
        process();  // PROBLEMATIC STATEMENT
    } else {
        send(resCode); // Bad request
    }
}

HTTP :: ~HTTP() {
    delete headers;
    delete reqBody;
}

int HTTP :: readRequest() {
    // Returns HTTP response code

    int contentLength = -1; // -1 means Content-Length does not exists
    char c;

    short int stage = 0;
    int i = 0;

    char tempHeaderName[SIZENORMAL];
    int lenTempHeaderName = 0;
    char tempHeaderValue[SIZELONG]; 
    int lenTempHeaderValue = 0;

    while (true) {
        if (stage == 0) {
            // Reading the Method
            // Method always ends with SPACE char
            i = 0;
            do {
                c = getChar();
                if (c == 0) {
                    // No data available to read or broken connection
                    // Serial.println(" > ERROR: 400 Bad Request");
                    return 400;
                }
                if (c == ' ') break;

                // i = total no of bytes already read
                if (i == (SIZESHORT - 1)) { // make this dynamic
                    // Serial.println(" > ERROR: 405 Method Not Allowed");
                    return 405;
                }

                method[i++] = c;
            } while (true);
            
            if (i == 0) {
                // There is nothing in method but it ended with SPACE char
                // Serial.println(" > ERROR: 400 Bad Request");
                return 400;
            }

            method[i] = 0;
            stage = 1;

            // Serial.print(" > Method: ");
            // Serial.println(method);
            Serial.println();
            Serial.print(method);
            Serial.print(" ");
        }

        if (stage == 1) {
            // Reading the URI
            // URI always ends with SPACE char
            
            i = 0;
            do {
                c = getChar();
                if (c == 0) {
                    // No data available to read or broken connection
                    // Serial.println(" > ERROR: 400 Bad Request");
                    return 400;
                }
                if (c == ' ') break;

                if (i == (SIZENORMAL - 1)){
                    // Serial.println(" > ERROR: 414 Request-URI Too Long");
                    return 414;
                }

                uri[i++] = c;
            } while (true);
            
            if (i == 0) {
                // There is nothing in uri but it ended with SPACE char
                // Serial.println(" > ERROR: 400 Bad Request");
                return 400;
            }

            // Notes: URIs should end with '/'
            if (uri[i - 1] != '/') {
                uri[i++] = '/';
            }

            uri[i] = 0;
            stage = 2;

            // Serial.print(" > URI: ");
            // Serial.println(uri);

            Serial.print(uri);
            Serial.print(" ");
        }

        if (stage == 2) {
            // Reading the HTTP Version
            // HTTP Version always ends with '\r' - CARRIAGE RETURN char
            
            i = 0;
            do {
                c = getChar();
                if (c == 0) {
                    // No data available to read or broken connection
                    // Serial.println(" > ERROR: 400 Bad Request");
                    return 400;
                }
                if (c == '\r') break;

                if (i == (SIZESHORT - 1)) {
                    // Serial.println(" > ERROR: 000 Version Too Long");
                    return 400;
                }

                version[i++] = c;
            } while (true);
            
            if (i == 0) {
                // There is nothing in version but it ended with \r char
                // Serial.println(" > ERROR: 400 Bad Request");
                return 400;
            }

            version[i] = 0;
            stage = 3;

            // Serial.print(" > Version: ");
            // Serial.println(version);

            Serial.println(version);
        }

        if (stage == 3) {
            // Reading '\n' - LINE FEED char
            c = getChar();

            if (c == 0) {
                // No data available to read or broken connection
                // Serial.println(" > ERROR: 400 Bad Request");
                return 400;
            }

            if (c == '\n') {
                stage = 4;
                // Serial.println(" > Request Line successfully read..!");
            } else {
                // Serial.println(" > ERROR: 400 Bad Request");
                return 400;
            }
        }

        if (stage == 4) {
            c = getChar();

            if (c == 0) {
                // No data available to read or broken connection
                // Serial.println(" > ERROR: 400 Bad Request");
                return 400;
            }

            if (c == '\r') {
                stage = 9;
            } else {
                // Reading the Header-Name
                // Header-Name always ends with COLON char
                lseek = true; // reads the prev char again
                stage = 5;

            }
        }

        if (stage == 5) {
            i = 0;
            do {
                c = getChar();

                if (c == 0) {
                    // No data available to read or broken connection
                    // Serial.println(" > ERROR: 400 Bad Request");
                    return 400;
                }

                if (c == ':') break;

                if (i == (SIZENORMAL - 1)) {
                    // Serial.println(" > ERROR: 000 Header-Name Too Long");
                    return 400;
                }

                tempHeaderName[i++] = c;
            } while (true);
            
            if (i == 0) {
                // There is nothing in tempHeaderName but it ended with : char
                // Serial.println(" > ERROR: 400 Bad Request");
                return 400;
            }
            
            tempHeaderName[i] = 0;
            lenTempHeaderName = i;
            stage = 6;

            // Serial.print(tempHeaderName);
            // Serial.print(" (");
            // Serial.print(i);
            // Serial.print(") ");
            // Serial.print(": ");


            if (equals(tempHeaderName, "Content-Length", false)) {
                // Serial.println(" > Content-Length found..");
                contentLength = 0; // Content-Length exists and yet to read it's value
            }
        }

        if (stage == 6) {
            // Reading and removing all the LWS
            do {
                c = getChar();

                if (c == 0) {
                    // No data available to read or broken connection
                    // Serial.println(" > ERROR: 400 Bad Request");
                    return 400;
                }

                if (c != ' ') break;
            } while (true);

            lseek = true;
            stage = 7;
        }

        if (stage == 7) {
            // Reading the Header-Value
            i = 0;
            do {
                c = getChar();

                if (c == 0) {
                    // No data available to read or broken connection
                    // Serial.println(" > ERROR: 400 Bad Request");
                    return 400;
                }
                
                if(c == '\r') break;

                if(i == (SIZELONG - 1)) {
                    // Serial.println(" > ERROR: 000 Header-Value Too Long");
                    return 400;
                }

                tempHeaderValue[i++] = c;
            } while (true);
            
            if (i == 0) {
                // There is nothing in tempHeaderValue but it ended with \r char
                // Serial.println(" > ERROR: 400 Bad Request");
                return 400;
            }

            tempHeaderValue[i] = 0;
            lenTempHeaderValue = i;
            stage = 8;

            // Serial.print(tempHeaderValue);
            // Serial.print(" (");
            // Serial.print(i);
            // Serial.print(") ");
            // Serial.println();

            // Serial.print(tempHeaderName);
            // Serial.print(" (");
            // Serial.print(lenTempHeaderName);
            // Serial.print(") ");
            // Serial.print(": ");
            // Serial.print(tempHeaderValue);
            // Serial.print(" (");
            // Serial.print(lenTempHeaderValue);
            // Serial.print(") ");
            // Serial.println();

            headers->put(lenTempHeaderName, tempHeaderName, lenTempHeaderValue, tempHeaderValue);

            if (contentLength == 0) {
                // Serial.print(" > Value of Content-Length: ");
                // Serial.println(headerValue);
                contentLength = toInt(tempHeaderValue);
            }
        }

        if (stage == 8) {
            c = getChar();

            if (c == 0) {
                // No data available to read or broken connection
                // Serial.println(" > ERROR: 400 Bad Request");
                return 400;
            }

            if (c == '\n') {
                stage = 4;
                continue;
            } else {
                // Serial.println(" > ERROR: 400 Bad Request");
                return 400;
            }
        }

        if (stage == 9) {
            c = getChar();

            if (c == 0) {
                // No data available to read or broken connection
                // Serial.println(" > ERROR: 400 Bad Request");
                return 400;
            }

            if (c == '\n') {
                // Serial.println(" > Reached End Of Headers..!");
                stage = 10;
            } else {
                // Serial.println(" > ERROR: 400 Bad Request");
                return 400;
            }
        }

        if (stage == 10) {

            if (contentLength <= 0) {
                // Serial.println(" > Request completed..!");
                return 200; // OK
            }

            reqBody = new char[contentLength + 1]; // Plus one for storing the \0 char in the end 

            // For reading the reqBody, if reqBody is there
            i = 0;
            while (contentLength--) {
                c = getChar();
                if (c == 0) {
                    // No data available to read or broken connection
                    // Serial.println(" > ERROR: 400 Bad Request");
                    return 400;
                }

                if (i == (SIZELONG - 1)) {
                    // Serial.println(" > ERROR: 000 reqBody Too Long");
                    return 400;
                }
                reqBody[i++] = c;
            }

            if (i == 0) {
                // There is nothing in reqBody but contentLegth got mentioned
                // Serial.println(" > ERROR: 400 Bad Request");
                return 400;
            }

            reqBody[i] = 0;

            Serial.print(" > BODY: ");
            Serial.println(reqBody);

            // Serial.println(" > Request completed..!");
            return 200; // OK
        }
    }
}

char HTTP :: getChar() {
    static char myChar = 0;

    if (lseek) {
        // returns the last read character
        lseek = false;
        return myChar;
    }

    if (client->connected() != true) {
        // Serial.println(" > ERROR: Connection Broken");
        return 0;
    }

    if (!client->available()) {
        // Serial.println(" > INFO: No data available to read from client..!");
        return 0;
    }

    myChar = client->read();
    return myChar;
}

char* HTTP :: getParamValue(const char *param) {

    bool eop = false;
    int i;
    for (i = 0; i < (SIZENORMAL * 2); i++){
        // Serial.print(" > i = ");
        // Serial.println(i);
        if (eop && params[i] == 0) {
            // Serial.println(" > EOParams");
            break;
        } else {
            eop = false;
        }
        // Serial.print(" >>> param: ");
        // Serial.println(&params[i]);
        const char *first = param;
        char *second = &params[i];

        int index = 0;

        char a = 0, b = 0;
        do {
            a = *first;
            b = *second;
            if (a == '\0' || b == '\0') break;
            index++;
        } while (first++ && second++ && a == b);

        if (a == 0 && b == ':') {
            return &params[i + index + 1];
        }

        while (params[i] != 0) {
            i++;
        }
        // Serial.println();
        // Serial.print(" > i = ");
        // Serial.println(i);
        eop = true;
    }

    return NULL;
}

bool HTTP :: compareURI(const char *pathOne) {
    char *pathTwo = uri;
    
    if (pathOne == NULL || *pathOne == 0) {
        // Serial.println(" > ERROR: Given string is NULL or Empty");
        return false;
    }
    
    if (pathTwo == NULL || *pathTwo == 0) {
        // Serial.println(" > ERROR: Given string is NULL or Empty");
        return false;
    }

    // noParams = find(pathOne, ':');
    // Serial.print(" > No of variables: ");
    // Serial.println(noParams);
    // params = new char* [noParams];
    // short int indParams = 0;

    params[0] = 0;
    params[1] = 0;
    int indParams = 0;

    bool isVariable = false;
    short int state = 0;
    int i, j;
    for (i = 0, j = 0; pathOne[i] != 0 && pathTwo[j] != 0; i++, j++) {

        if (state == 0) {
            if (pathOne[i] != '/' || pathTwo[j] != '/') {
                // cout << " > Invalid URI" << endl;
                return false;
            }
            state = 1;
            continue;
        }
        
        if (state == 1) {
            if (pathOne[i] == ':') {
                isVariable = true;
                i++;
            }
            if (isVariable) {
                // char temp[SIZELONG];
                short int ind = 0;

                while (pathOne[i] != 0) {
                    if (pathOne[i] == '/') {
                        break;
                    }
                    params[indParams++] = pathOne[i];
                    i++;
                }
                
                params[indParams++] = ':';
                i--;
                
                while (pathTwo[j] != 0) {
                    if (pathTwo[j] == '/') {
                        break;
                    }
                    params[indParams++] = pathTwo[j];
                    j++;
                }
                params[indParams++] = 0;
                // Serial.print(" > Added Param: ");
                // Serial.println(params);
                j--;

                // params[indParams] = new char[ind + 1]; // ind = last index in which \0 exists
                // int k = 0;
                // for (k = 0; k < ind; k++) {
                //     params[indParams][k] = temp[k];
                // }
                // params[indParams][k] = 0;
                // Serial.println(" > Below Item Added to Params..");
                // Serial.println(params[indParams]);
                // indParams++;


                state = 0;
                isVariable = false;
            } else {
                if (pathOne[i] == pathTwo[j]) {
                    if (pathOne[i] == '/') {
                        i--;
                        j--;
                        state = 0;
                    } else {
                    }
                } else {
                    return false;
                }
            }
            
        }
        
    }
    params[indParams] = 0;

    // bool eop = false;
    // for (i = 0; i < (SIZENORMAL * 2); i++){
    //     Serial.print(" > i = ");
    //     Serial.println(i);
    //     if (eop && params[i] == 0) {
    //         Serial.println(" > EOParams");
    //         break;
    //     } else {
    //         eop = false;
    //     }
    //     Serial.print(" >>> param: ");
    //     Serial.println(&params[i]);

    //     while (params[i] != 0) {
    //         // Serial.print(params[i]);
    //         i++;
    //     }
    //     Serial.println();
    //     Serial.print(" > i = ");
    //     Serial.println(i);
    //     eop = true;
    // }
    
    if (pathOne[i] == 0 && pathTwo[j] == 0)
    return true;
    return false;
}

void HTTP :: status(int status){
    switch(status){
        case 200:
            statusCode = 200;
            reasonPhrase[0] = '\0';
            concat(reasonPhrase, "OK");
            break;
        case 201:
            statusCode = 201;
            reasonPhrase[0] = '\0';
            concat(reasonPhrase, "Created");
            break;
        case 202:
            statusCode = 202;
            reasonPhrase[0] = '\0';
            concat(reasonPhrase, "Accepted");
            break;

        case 400:
            statusCode = 400;
            reasonPhrase[0] = '\0';
            concat(reasonPhrase, "Bad Request");
            break;
        case 401:
            statusCode = 401;
            reasonPhrase[0] = '\0';
            concat(reasonPhrase, "Unauthorized");
            break;
        case 403:
            statusCode = 403;
            reasonPhrase[0] = '\0';
            concat(reasonPhrase, "Forbidden");
            break;
        case 404:
            statusCode = 404;
            reasonPhrase[0] = '\0';
            concat(reasonPhrase, "Not Found");
            break;
        case 405:
            statusCode = 405;
            reasonPhrase[0] = '\0';
            concat(reasonPhrase, "Method Not Allowed");
            break;
        case 406:
            statusCode = 406;
            reasonPhrase[0] = '\0';
            concat(reasonPhrase, "Not Acceptable");
            break;
    }
    resBody[0] = 0;
}

void HTTP :: send(){

    client->print("HTTP/1.1 ");
    client->print(statusCode);
    client->print(" ");
    client->println(reasonPhrase);

    addHeaders();

    int contentLength = length(resBody);
    if(contentLength > 0){
        client->print("Content-Length: ");
        client->println(contentLength);
        client->println("Content-Type: application/json");
    }
    client->println(); // End of Headers
    if(contentLength > 0){
        client->println(resBody);
    }
}

void HTTP :: send(int st) {
    status(st);
    send();
}

void HTTP :: send(char *res){
    status(200);
    resBody[0] = 0;
    concat(resBody, res);
    send();
}

void HTTP :: write(int arg) {
    char result[SIZESHORT];
    if (arg == 0) {
        result[0] = '0';
        result[1] = 0;
        write(result);
        return;
    }
    
    short int i = 0, rem, len;
    while (arg != 0) {
        result[i++] = (arg % 10) + 48;
        arg = arg/10;
    }
    result[i] = 0;

    // reverse the string
    len = i;
    i--;
    short int temp, j = 0;
    while (j < i) {
        temp = result[j];
        result[j] = result[i];
        result[i] = temp;
        i--;
        j++;
    }
    write(result);
}

void HTTP :: write(char *arg){
    // For chunked transfer of data
    if(!chunked){
        status(200);
        chunked = true;

        client->print("HTTP/1.1 ");
        client->print(statusCode);
        client->println(reasonPhrase);

        addHeaders();

        client->println("Transfer-Encoding: chunked");
        client->println("Content-Type: application/json");
        client->println();
    }

    // Serial.print(" > Length: ");
    // Serial.println(length(arg));
    char numInHex[10] = "";
    toHex(length(arg), numInHex);

    client->println(numInHex);
    // Serial.println(numInHex);
    client->println(arg);
    // Serial.println(arg);
}

void HTTP :: addHeaders() {
    client->println("Connection: close");
    client->println("Access-Control-Allow-Origin: *");
    client->println("Access-Control-Allow-Methods: PUT");
    client->println("Access-Control-Allow-Headers: Content-Type");
}
