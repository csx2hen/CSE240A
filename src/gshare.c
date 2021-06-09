//
// Created by Medivh on 2021/6/8.
//

#include "predictor.h"
#include "gshare.h"

uint8_t* gshareTable;
int gshareHistory, gshareLength, gshareMask, gshareIndex;

void
init_gshare()
{
    gshareLength = 1 << ghistoryBits;
    gshareMask = gshareLength - 1;
    gshareTable = malloc(sizeof(uint8_t)*(gshareLength));
    for(int i = 0; i < gshareLength; i++) {
        gshareTable[i] = WN;
    }
}

uint8_t
gshare_prediction(uint32_t pc)
{
    gshareIndex = (pc ^ gshareHistory) & gshareMask;
    if(gshareTable[gshareIndex] < WT) {
        return 0;
    }
    else {
        return 1;
    }
}

void
train_gshare(uint8_t outcome)
{
    // update history
    gshareHistory = ((gshareHistory << 1) | outcome) & gshareMask;
    // update table
    if (outcome == 0 && gshareTable[gshareIndex] > SN) {
        gshareTable[gshareIndex] -= 1;
    }
    if (outcome == 1 && gshareTable[gshareIndex] < ST) {
        gshareTable[gshareIndex] += 1;
    }
}