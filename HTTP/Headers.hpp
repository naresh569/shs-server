
class Header {
    public:
        char* property;
        char* value;
        Header* next;

        Header(char*, char*);
        Header(int, char*, int, char*);
        ~Header();
};

Header :: Header(char* p, char* v) {
    property = p;
    value = v;
    next = NULL;
}

Header :: Header(int plen, char* p, int vlen, char* v) {
    property = new char[plen + 1];
    property[0] = 0;
    concat(property, p);

    value = new char[vlen + 1];
    value[0] = 0;
    concat(value, v);

    next = NULL;
}

Header :: ~Header() {
    delete property;
    delete value;
    delete next;
}

class Headers {
    private:
        Header* list;
        int length;
    public:
        Headers();
        ~Headers();

        void put(int, char*, int, char*);
        Header* get(char*);
        void display();
};

Headers :: Headers() {
    length = 0;
    list = NULL;
}

Headers :: ~Headers() {
    delete list;
}

void Headers :: put(int plen, char* p, int vlen, char* v) {
    Header* n = new Header(plen, p, vlen, v);

    if (!list) {
        list = n;
        length++;
        return;
    }

    Header* current = list;
    while (current->next)
    current = current->next;

    current->next = n;
    length++;

    // display();
}

Header* Headers :: get(char* p) {

    if (!list) {
        return NULL;
    }

    Header* result = NULL;
    Header* current = list;
    do {
        if (equals(current->property, p, false)) {
            result = current;
            break;
        }
    } while (current->next && (current = current->next));

    return result;
}

void Headers :: display() {
    Header* current = list;
    while (current) {
        Serial.print(current->property);
        Serial.print(": ");
        Serial.println(current->value);
        current = current->next;
    }
}
