//
// Created by Medivh on 2021/6/8.
//

#ifndef TOURNAMENT_H
#define TOURNAMENT_H

#include <stdint.h>

void init_tournament();

uint8_t tournament_prediction(uint32_t pc);

void train_tournament(uint32_t pc, uint8_t outcome);

#endif //TOURNAMENT_H
