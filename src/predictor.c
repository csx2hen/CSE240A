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
uint8_t* gshareTable;
int gshareHistory, gshareLength, gshareMask, gshareIndex;
void init_Gshare();
uint8_t gshare_prediction(uint32_t pc);
void train_gshare(uint8_t outcome);

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

void
init_Gshare()
{
  gshareLength = 1 << ghistoryBits;
  gshareMask = gshareLength - 1;
  gshareTable = malloc(sizeof(uint8_t)*(gshareLength));
  for(int i = 0; i < gshareLength; i++) {
    gshareTable[i] = WN;
  }
  return;
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
  return;
}

void
init_Tournament()
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

// Initialize the predictor
//
void
init_predictor()
{
  switch (bpType) {
    case GSHARE:
      init_Gshare();
      break;
    case TOURNAMENT:
      init_Tournament();
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
      return gshare_prediction(pc);
    case TOURNAMENT:
      return tournament_prediction(pc);
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
      train_gshare(outcome);
      break;
    case TOURNAMENT:
      train_tournament(pc, outcome);
      break;
    case CUSTOM:
      break;
    default:
      break;
  }
}
