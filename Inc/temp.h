/*
 * temp.h
 *
 *  Created on: 26Nov.,2017
 *      Author: uqracing
 */

#ifndef TEMP_H_
#define TEMP_H_

#include "can.h"

#define TEMP_THRESH			7000

void temp_init();
void temp_check(can_variables rx_pkt);

#endif /* TEMP_H_ */
