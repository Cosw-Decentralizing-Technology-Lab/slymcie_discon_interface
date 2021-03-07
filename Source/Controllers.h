/*
 * Controllers.h
 *
 *  Created on: 2021. 2. 18.
 *      Author: Inwoo Chung (gutomitai@cosw.space)
 */

#ifndef CONTROLLERS_H_
#define CONTROLLERS_H_

extern int InitBaramInterface();
extern int UninitBaramInterface();
extern int CalculateYawMisalignment(double rdRaw
			, double fwsRaw
			, double rsRaw
			, double powRaw
			, long tRaw
			, double *ym);

#endif /* CONTROLLERS_H_ */
