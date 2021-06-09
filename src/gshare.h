//
// Created by Medivh on 2021/6/8.
//

#ifndef GSHARE_H
#define GSHARE_H

#include <stdint.h>

void init_gshare();

uint8_t gshare_prediction(uint32_t pc);

void train_gshare(uint8_t outcome);

#endif //GSHARE_H
