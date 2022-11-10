/*
 * service.h
 *
 * Created on: Mar 06, 2022 14:16
 * Description:
 *
 * Copyright (c) 2022 Ruixiang Du (rdu)
 */

#ifndef SERVICE_H
#define SERVICE_H

#include <zephyr.h>

#include "interface/type.h"

// e.g.
// {
//      // beginning of the loop
//      int64_t t0 = k_loop_start()
//      ...
//      // do some work
//      ...
//      // sleep for "period - (t - t0)"
//      k_msleep_until(period_ms, t0);
// }
#define k_loop_start() k_uptime_get()
#define k_msleep_until(period, t0) k_msleep(period - k_uptime_delta(&t0))

#endif /* SERVICE_H */
