
#pragma once
#include "Config.hpp"
#include "Switch.hpp"

class Block {
    public:
        int bid;
        char* label;
        char* desc;
        int sTotal;
        int switchIds[MAXSWITCHES];
        Block();
        ~Block();
        Block(const char*, const char*);
        Switch* addSwitch(char , int , char*, char*);

        static short int total;
        static Block* addBlock(const char*, const char*);

} *blocks[MAXBLOCKS];

short int Block :: total = 0;

Block :: Block() {
    bid = total + 1;
    label = NULL;
    desc = NULL;
    sTotal = 0;

    total++;
}

Block :: ~Block() {
    total--;
}

Block :: Block(const char* lbl, const char* dsc) {
    bid = total + 1;

    label = new char[SIZESHORT];
    label[0] = 0;
    concat(label, lbl);

    desc = new char[SIZENORMAL];
    desc[0] = 0;
    concat(desc, dsc);

    sTotal = 0;

    total++;
}

Switch* Block :: addSwitch(char ptype, int ppin, char *ptitle, char *pdesc) {
    short int total = Switch :: total;

    if (total >= MAXSWITCHES) {
        return NULL;
    }

    switches[total] = new Switch (ptype, ppin, ptitle, pdesc);
    switchIds[sTotal++] = switches[total]->sid;

    return switches[total];
}

Block* Block :: addBlock(const char* lbl, const char* dsc) {
    short int total = Block :: total;

    if (total >= MAXBLOCKS) {
        return NULL;
    }

    blocks[total] = new Block(lbl, dsc);
    return blocks[total];
}