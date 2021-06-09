//
// Created by Medivh on 2021/6/8.
//

#include "predictor.h"
#include "tournament.h"

uint32_t *lhistoryTable;
uint32_t *lpatternTable;
uint32_t *gpatternTable;
uint32_t *choiceTable;
uint32_t lhistory;
uint32_t lmask;
uint32_t ghistory;
uint32_t gmask;
uint32_t pcmask;
uint32_t choice;
uint8_t lprediction;
uint8_t gprediction;

uint32_t
gen_mask(uint32_t len)
{
    uint32_t mask = 1 << len;
    return mask - 1;
}

void
init_tournament()
{
    int size;
    gmask = gen_mask(ghistoryBits);
    ghistory = 0;
    lmask = gen_mask(lhistoryBits);
    pcmask = gen_mask(pcIndexBits);
    // Local history table
    size = 1 << pcIndexBits;
    lhistoryTable = (uint32_t*) malloc(sizeof(uint32_t) * size);
    for (int i = 0; i < size; i++)
    {
        lhistoryTable[i] = 0;
    }
    // Local pattern table
    size = 1 << lhistoryBits;
    lpatternTable = (uint32_t*) malloc(sizeof(uint32_t) * size);
    for (int i = 0; i < size; i++)
    {
        lpatternTable[i] = 1;
    }
    // Global pattern table & Choice table
    size = 1 << ghistoryBits;
    gpatternTable = (uint32_t*) malloc(sizeof(uint32_t) * size);
    choiceTable = (uint32_t*) malloc(sizeof(uint32_t) * size);
    for (int i = 0; i < size; i++)
    {
        gpatternTable[i] = 1;
        choiceTable[i] = 2;
    }
}

uint8_t
tournament_prediction(uint32_t pc)
{
    // Local
    lhistory = lhistoryTable[pc >> 2 & pcmask];
    lprediction = lpatternTable[lhistory] > 1 ? TAKEN : NOTTAKEN;
    // Global
    gprediction = gpatternTable[ghistory] > 1 ? TAKEN : NOTTAKEN;
    // Choose local or global
    choice = choiceTable[ghistory];
    return choice < 2 ? lprediction : gprediction;
}

void
train_tournament(uint32_t pc, uint8_t outcome)
{
    // Update choice table
    if (lprediction == outcome && gprediction != outcome && choiceTable[ghistory] != 0)
        choiceTable[ghistory]--;
    if (lprediction != outcome && gprediction == outcome && choiceTable[ghistory] != 3)
        choiceTable[ghistory]++;
    // Update local & global pattern table
    if (outcome == TAKEN) {
        if (lpatternTable[lhistory] != 3)
            lpatternTable[lhistory]++;
        if (gpatternTable[ghistory] != 3)
            gpatternTable[ghistory]++;
    }
    else
    {
        if (lpatternTable[lhistory] != 0)
            lpatternTable[lhistory]--;
        if (gpatternTable[ghistory] != 0)
            gpatternTable[ghistory]--;
    }
    // Update local & global history
    lhistoryTable[pc >> 2 & pcmask] = (lhistoryTable[pc >> 2 & pcmask] << 1 | outcome) & lmask;
    ghistory = (ghistory << 1 | outcome) & gmask;
}
