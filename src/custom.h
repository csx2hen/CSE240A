//
// Created by Medivh on 2021/6/8.
//

#ifndef CUSTOM_H
#define CUSTOM_H

#include <stdint.h>

void init_custom();

uint8_t custom_prediction(uint32_t pc);

void train_custom(uint32_t pc, uint8_t outcome);

#endif //CUSTOM_H
