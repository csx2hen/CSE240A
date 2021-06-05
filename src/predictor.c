//========================================================//
//  predictor.c                                           //
//  Source file for the Branch Predictor                  //
//                                                        //
//  Implement the various branch predictors below as      //
//  described in the README                               //
//========================================================//
#include <stdio.h>
#include "predictor.h"

//
// TODO:Student Information
//
const char *studentName = "NAME";
const char *studentID   = "PID";
const char *email       = "EMAIL";

//------------------------------------//
//      Predictor Configuration       //
//------------------------------------//

// Handy Global for use in output routines
const char *bpName[4] = { "Static", "Gshare",
                          "Tournament", "Custom" };

int ghistoryBits; // Number of bits used for Global History
int lhistoryBits; // Number of bits used for Local History
int pcIndexBits;  // Number of bits used for PC index
int bpType;       // Branch Prediction Type
int verbose;

//------------------------------------//
//      Predictor Data Structures     //
//------------------------------------//

uint32_t ghistory;
uint32_t gmask;

// gshare

// tournament
uint32_t *lhistoryTable;
uint32_t *lpatternTable;
uint32_t *gpatternTable;
uint32_t *choiceTable;
uint32_t lhistory;
uint32_t lmask;
uint32_t pcmask;
uint32_t choice;
uint8_t lprediction;
uint8_t gprediction;

//------------------------------------//
//        Predictor Functions         //
//------------------------------------//

uint32_t
gen_mask(uint32_t len)
{
  uint32_t mask = 0;
  for (int i = 0; i < len; i++)
  {
    mask = mask | 1 << i;
  }
  return mask;
}

// Initialize the predictor
//
void
init_predictor()
{
  int size;
  gmask = gen_mask(ghistoryBits);
  ghistory = 0;

  switch (bpType) {
    case GSHARE:
      break;
    case TOURNAMENT:
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
      break;
    case CUSTOM:
      break;
    default:
      break;
  }
}

// Make a prediction for conditional branch instruction at PC 'pc'
// Returning TAKEN indicates a prediction of taken; returning NOTTAKEN
// indicates a prediction of not taken
//
uint8_t
make_prediction(uint32_t pc)
{
  // Make a prediction based on the bpType
  switch (bpType) {
    case STATIC:
      return TAKEN;
    case GSHARE:
    case TOURNAMENT:
      // Local
      lhistory = lhistoryTable[pc & pcmask];
      lprediction = lpatternTable[lhistory] > 1 ? TAKEN : NOTTAKEN;
      // Global
      gprediction = gpatternTable[ghistory] > 1 ? TAKEN : NOTTAKEN;
      // Choose local or global
      choice = choiceTable[ghistory];
      return choice < 2 ? lprediction : gprediction;
    case CUSTOM:
    default:
      break;
  }

  // If there is not a compatible bpType then return NOTTAKEN
  return NOTTAKEN;
}

// Train the predictor the last executed branch at PC 'pc' and with
// outcome 'outcome' (true indicates that the branch was taken, false
// indicates that the branch was not taken)
//
void
train_predictor(uint32_t pc, uint8_t outcome)
{
  switch (bpType) {
    case GSHARE:
      break;
    case TOURNAMENT:
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
      lhistoryTable[pc & pcmask] = (lhistoryTable[pc & pcmask] << 1 | outcome) & lmask;
      ghistory = (ghistory << 1 | outcome) & gmask;
      break;
    case CUSTOM:
      break;
    default:
      break;
  }
}
