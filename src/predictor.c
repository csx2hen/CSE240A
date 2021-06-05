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

//
//TODO: Add your own Branch Predictor data structures here
//
// Gshare data structures
uint8_t* gshareTable;
int gshareHistory, gshareLength, gshareMask, gshareIndex;
void init_Gshare();
uint8_t gshare_prediction(uint32_t pc);
void train_gshare(uint8_t outcome);

//------------------------------------//
//        Predictor Functions         //
//------------------------------------//

// Initialize the predictor
//
void
init_predictor()
{
  //
  //TODO: Initialize Branch Predictor Data Structures
  //

  switch (bpType) {
    case STATIC:
      break;
    case GSHARE:
      init_Gshare();
    case TOURNAMENT:
    case CUSTOM:
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
  //
  //TODO: Implement prediction scheme
  //

  // Make a prediction based on the bpType
  switch (bpType) {
    case STATIC:
      return TAKEN;
    case GSHARE:
      return gshare_prediction(pc);
    case TOURNAMENT:
    case CUSTOM:
    default:
      break;
  }

  // If there is not a compatable bpType then return NOTTAKEN
  return NOTTAKEN;
}

// Train the predictor the last executed branch at PC 'pc' and with
// outcome 'outcome' (true indicates that the branch was taken, false
// indicates that the branch was not taken)
//
void
train_predictor(uint32_t pc, uint8_t outcome)
{
  //
  //TODO: Implement Predictor training
  //

  switch (bpType) {
    case STATIC:
      break;
    case GSHARE:
      train_gshare(outcome);
    case TOURNAMENT:
    case CUSTOM:
    default:
      break;
  }
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