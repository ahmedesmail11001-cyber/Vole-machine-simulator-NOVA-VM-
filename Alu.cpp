#include "Alu.h"

uint8_t Alu::addInt(uint8_t a, uint8_t b) // 07 + 15 8 bits + 8 bits  3201 
{
    return a + b;
}

uint8_t Alu::addFloat(uint8_t a, uint8_t b)
{
<<<<<<< HEAD
    return (float)a + (float)b;
=======
    uint8_t aSign = (a >> 7 & 0x01);
    int aExponent = (a >> 4 & 0x07) - 4;
    uint8_t aMantissa = (a & 0x0F);
    uint8_t bSign = (b >> 7 & 0x01);
    int bExponent = (b >> 4 & 0x07) - 4;
    uint8_t bMantissa = (b & 0x0F);
    uint8_t resultMantissa;
    int resultSign , resultExponent;

    int maxiExponent = max(aExponent,bExponent);
    int miniExponent = min(aExponent,bExponent);
    int commonExponent = maxiExponent ;
    uint8_t diffrence = maxiExponent - miniExponent;

        if(miniExponent == aExponent){
           aMantissa = aMantissa >> diffrence;
        }
        else if(miniExponent == bExponent){
            bMantissa = bMantissa >> diffrence;
        }

        if(aSign == bSign){
            resultSign = aSign;
            resultMantissa = aMantissa + bMantissa;
            resultExponent = commonExponent;
        }
        if(aSign != bSign){
           if(aMantissa>bMantissa){
                resultSign = aSign;
           }
           else if(bMantissa>aMantissa){
               resultSign = bSign;
           }
            resultMantissa = abs(aMantissa - bMantissa);
            resultExponent = commonExponent;
        }
        while((resultMantissa > 15 || resultMantissa < 8) && (resultMantissa != 0) && (resultExponent > -4) && (resultExponent < 3)){

                if(resultMantissa > 15){
                resultMantissa = resultMantissa >> 1;
                resultExponent+=1;
            }
            else if(resultMantissa < 8){
                resultMantissa = resultMantissa << 1;
                resultExponent-=1;
            }

        }
        resultExponent +=4;
        return((resultSign << 7) | (resultExponent << 4) | (resultMantissa));
>>>>>>> a5482792fe394ec43ca726914527cb00e421f593
}

uint8_t Alu::bitOr(uint8_t a, uint8_t b)
{
    return a | b;
}

uint8_t Alu::bitAnd(uint8_t a, uint8_t b)
{
    return a & b;
}

uint8_t Alu::bitXor(uint8_t a, uint8_t b)
{
    return a ^ b;
}

uint8_t Alu::rotateRight(uint8_t a, uint8_t n)
{
    return (a >> n) | (a << (8 - n)); // 00001010 -> 00000001 | 01000000 = 01000001
}
