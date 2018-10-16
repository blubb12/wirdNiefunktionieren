//
// Created by simon on 03.06.18.
//

#include "GF256.h"

GF256::GF256(u_int8_t value) {
    this->value = value;
    init(3);
}

GF256::~GF256() = default;

void GF256::init(u_int8_t g){
    u_int8_t temp=1;
    for (int i=0;i<256 ;i++){
        if (i==0 || i==255)
            expTable[i]=1;
        else
            expTable[i] = russianMultiplication(temp,g);
        temp = expTable[i];
    }

    for(int i=1;i<256;i++) {
        temp = expTable[i];
        logTable[temp] = i;
    }
    logTable[0]=0;
    logTable[1]=0;
}

GF256 GF256::operator+(GF256 other) {
    return GF256(this->value ^ other.value);
}

GF256 GF256::operator-(GF256 other) {
    return GF256(this->value ^ other.value);
}

GF256 GF256::operator*(GF256 other) {
    u_int8_t temp = 0;
    u_int8_t multiplicandOne = this->value;
    u_int8_t multiplicandTwo = other.value;

    for (int i = 0; i < 8; i++) {
        if ((multiplicandTwo & 0x01) == 1) {
            temp = temp ^ multiplicandOne;
        }

        if ((multiplicandOne & 0x80) == 0x80) {
            multiplicandOne <<= 1;
            multiplicandOne ^= 0x1b;
        } else {
            multiplicandOne <<= 1;
        }
        multiplicandTwo >>= 1;
    }
    return GF256(temp);
}

u_int8_t GF256::getValue() const {
    return value;
}

u_int8_t GF256::russianMultiplication(u_int8_t multiplicandOne, u_int8_t multiplicandTwo) {
    u_int8_t temp=0;

    for (int i =0; i<8;i++){
        if((multiplicandTwo & 0x01)==1){
            temp= temp ^ multiplicandOne;
        }

        if((multiplicandOne & 0x80) == 0x80){
            multiplicandOne<<=1;
            multiplicandOne^= 0x1b;
        } else{
            multiplicandOne<<=1;
        }
        multiplicandTwo>>=1;
    }
    return temp;
}

GF256 GF256::inverse() {
    return GF256(expTable[(255-logTable[this->value])]);
}

const u_int8_t *GF256::getExpTable() const {
    return expTable;
}

const u_int8_t *GF256::getLogTable() const {
    return logTable;
}






