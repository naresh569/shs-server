
#pragma once
#include "Config.hpp"

int length(char *inp) {
    int count = 0;
    for (int i=0; inp[i] != '\0'; i++) {
        count++;
    }
    return count;
}

void concat(char *str1, char const *str2) {
    int i = 0, j = 0;
    while (str1[i] != '\0')
    i++;

    while (str2[j] != '\0')
    str1[i++] = str2[j++];

    str1[i] = '\0';
}

void concat(char *str1, bool str2) {
    int i = 0;
    while (str1[i] != '\0')
    i++;

    // str2 is boolean value
    if (str2 == true)
    str1[i++] = '1';
    else
    str1[i++] = '0';

    str1[i] = '\0';
}

void concat(char *str1, char str2) {
    int i = 0;
    while (str1[i] != '\0')
    i++;
    
    str1[i++] = str2;

    str1[i] = '\0';
}

void concat(char *str1, int str2) {
    int i = 0;
    while (str1[i] != '\0')
    i++;

    if (str2 == 0) {
        str1[i++] = '0';
        str1[i] = '\0';
        return;
    }

    if (str2 < 0) {
        str1[i++] = '-';
        str2 = -str2;
    }

    char temp[SIZESHORT] = "";
    int j = 0, count = 0;
    while (str2 != 0) {
        temp[j++] = (char) 48 + (str2 % 10);
        count++;
        str2 = str2 / 10;
    }

    while (count--) {
        str1[i++] = temp[count];
    }

    str1[i] = '\0';
}

bool equals(char const *first, char const *second, bool caseSensitive) {
    if (first == NULL || *first == '\0') {
        // Serial.println(" > ERROR: NULL or empty string argument..!");
        return false;
    }
    if (second == NULL || *second == '\0') {
        // Serial.println(" > ERROR: NULL or empty string argument..!");
        return false;
    }

    char **params = NULL;
    
    char a = 0, b = 0;
    do {
        a = *first;
        b = *second;
        if (a == '\0' || b == '\0') break;
    
        if (!caseSensitive) {
            if (a >= 97 && a <= 122) a -= 32;
            if (b >= 97 && b <= 122) b -= 32;
        }
    } while (first++ && second++ && a == b);
    
    if (a == b) {
        return true;
    } else {
        return false;
    }
}

int toInt(char const *s) {
    if ( s == NULL || *s == '\0' ) {
        // Serial.println(" > ERROR: NULL or empty string argument..!");
        return 0;
    }

    bool negate = (s[0] == '-');
    if (*s == '+' || *s == '-') 
    ++s;

    if ( *s == '\0'){
        // Serial.println("ERROR: Sign character only..!");
        return 0;
    }

    int result = 0;
    while (*s) {
        if ( *s >= '0' && *s <= '9' ) {
            result = result * 10  - (*s - '0');  //assume negative number
        } else {
            //   Serial.println("ERROR: Invalid input string..!");
            return 0;
        }
        ++s;
    }
    return negate ? result : -result; //-result is positive!
} 

void toHex(int dec, char *hex) {
    if (dec == 0) {
        hex[0] = '0';
        hex[1] = 0;
        return;
    }
    
    int rem, i = 0;
    char result[10] = "";
    
    while (dec > 0) {
        rem = dec % 16; // Get the remainder.
        if (rem > 9) {
            // Map the character given that the remainder is greater than 9.
            switch (rem) {
                case 10: result[i++] = 'A'; break;
                case 11: result[i++] = 'B'; break;
                case 12: result[i++] = 'C'; break;
                case 13: result[i++] = 'D'; break;
                case 14: result[i++] = 'E'; break;
                case 15: result[i++] = 'F'; break;
            }
        } else {
            result[i++] = rem + 48;
        }
        dec = dec/16;
    }
    
    int j = 0;
    while (i--) {
        hex[j++] = result[i];
    }
    hex[j] = 0;
}

int find(const char *string, char delimiter) {
    if (string == NULL || *string == 0) {
        // Serial.println(" > ERROR: NULL or empty string argument..!");
        return 0;
    }
    
    int count = 0;
    do {
        if(*string == delimiter)
        count++;
    } while (*(++string) != 0);
    
    return count;    
}