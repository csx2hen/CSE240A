//
// Created by Medivh on 2021/6/8.
//

#include "predictor.h"
#include "custom.h"

#define ADDR_LENGTH 5
#define HIS_ADDR_LENGTH 3
#define GHIST_LENGTH 30
#define SAT_VALUE 127
int weights[(1<< ADDR_LENGTH)][(1<< HIS_ADDR_LENGTH)][GHIST_LENGTH+1];
int ghistorylength;
uint8_t gHistReg[GHIST_LENGTH];
uint32_t addr[GHIST_LENGTH];
int output;
uint8_t prediction;
int theta;
int mask, mask1, pcMasked;

void
init_custom() {
    mask = ((1 << ADDR_LENGTH) -1);
    mask1 = ((1 << HIS_ADDR_LENGTH) -1);
    ghistorylength = GHIST_LENGTH;
//    theta = 2.14*(GHIST_LENGTH + 1) + 20.58;
    theta = 1.93 * (float)(GHIST_LENGTH + 1) + 14;
    //theta=32;
    int i,j,k;
    for(i=0; i< (1<< ADDR_LENGTH); i++)
    {
        for(j=0; j < (1<< HIS_ADDR_LENGTH); j++)
        {
            for(k=0; k<= GHIST_LENGTH; k++)
            {
                weights[i][j][k]=0;
            }
        }
    }
    for(k=0; k< GHIST_LENGTH; k++)
    {
        gHistReg[k] = 0;
        addr[k] = 0;
    }
}

uint8_t
custom_prediction(uint32_t pc)
{
    pcMasked = (pc & mask);
    output = weights[pcMasked][0][0];
    int i;
    for(i=1; i<= ghistorylength; i++)
    {
        if(gHistReg[i] == TAKEN)
        {
            output = output + weights[pcMasked][(addr[i])][i];
        }
        else
        {
            output = output - weights[pcMasked][(addr[i])][i];
        }
    }
    if(output >= 0)
    {
        prediction = TAKEN;
        return prediction;
    }
    else
    {
        prediction = NOTTAKEN;
        return prediction;
    }
}

void
train_custom(uint32_t pc, uint8_t outcome) {
    pcMasked = pc & mask;
    if (abs(output) < theta || prediction != outcome)
    {
        if(outcome == TAKEN)
        {
            if(weights[pcMasked][0][0] < SAT_VALUE)
                weights[pcMasked][0][0] = weights[pcMasked][0][0]+1;
        }
        else
        {
            if(weights[pcMasked][0][0] > ((-1)*SAT_VALUE))
                weights[pcMasked][0][0] = weights[pcMasked][0][0]-1;

        }
        int i;
        for (i=1; i <= ghistorylength; i++)
        {
            if(gHistReg[i] == outcome)
            {
                if( weights[pcMasked][(addr[i])][i] < SAT_VALUE)
                    weights[pcMasked][(addr[i])][i] = weights[pcMasked][(addr[i])][i] + 1;
            }
            else
            {
                if(weights[pcMasked][(addr[i])][i] > ((-1)*SAT_VALUE ))
                    weights[pcMasked][(addr[i])][i] = weights[pcMasked][(addr[i])][i] - 1;
            }
        }
    }
    int i;
    for(i=ghistorylength-1; i>=2; i--)
    {
        addr[i]=addr[(i-1)];
        gHistReg[i] = gHistReg[(i-1)];

    }
    addr[1]= pc & mask1;
    gHistReg[1]=outcome;
}
