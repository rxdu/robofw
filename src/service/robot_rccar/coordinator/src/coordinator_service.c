/*
 * coordinator_service.c
 *
 * Created on: Jan 29, 2022 15:03
 * Description:
 *
 * Copyright (c) 2021 Ruixiang Du (rdu)
 */

#include "coordinator/coordinator_service.h"

#include <math.h>

// #include "actuator/actuator_service.h"
#include "receiver/receiver_service.h"
#include "messenger/messenger_service.h"

#define LINEAR_MOTION_DEADZONE 0.05
#define ANGULAR_MOTION_DEADZONE 0.05

K_THREAD_STACK_DEFINE(coord_service_stack, 1024);

_Noreturn static void CoordinatorServiceLoop(void *p1, void *p2, void *p3);

bool StartCoordinatorService(CoordinatorServiceDef *def) {
  // sanity check
  if (def->dependencies.receiver_interface == NULL ||
      def->dependencies.messenger_interface == NULL) {
    printk("Dependency not set properly\n");
    return false;
  }

  // create and start thread
  def->tconf.tid = k_thread_create(
      &def->tconf.thread, coord_service_stack,
      K_THREAD_STACK_SIZEOF(coord_service_stack), CoordinatorServiceLoop, def,
      NULL, NULL, def->tconf.priority, 0, Z_TIMEOUT_MS(def->tconf.delay_ms));
  k_thread_name_set(def->tconf.tid, "coordinator_service");

  return true;
}

_Noreturn void CoordinatorServiceLoop(void *p1, void *p2, void *p3) {
  CoordinatorServiceDef *def = (CoordinatorServiceDef *)p1;

  ReceiverData receiver_data;

  DesiredMotion desired_motion_from_can;
  desired_motion_from_can.linear = 0;
  desired_motion_from_can.angular = 0;

  SupervisorCommand sup_cmd;
  sup_cmd.supervised_mode = kNonSupervised;

  RobotState robot_state;
  robot_state.rc_connected = false;
  robot_state.estop_triggered = true;
  robot_state.control_mode = kControlModeRC;
  robot_state.sup_mode = kNonSupervised;

  DesiredMotion desired_motion;
  desired_motion.linear = 0;
  desired_motion.angular = 0;

  uint8_t loop_counter = 0;

  uint8_t rc_counter = 0;
  const uint8_t rc_timeout_count = 5;

  //   uint8_t can_counter = 0;
  //   const uint8_t can_timeout_count = 5;

  while (1) {
    int64_t t0 = k_loop_start();

    if (loop_counter % 25 == 0) {
      loop_counter = 0;
      ToggleLed(def->sconf.dd_led_status);
    }

    /*********** read receiver data and update robot state ***********/
    if (k_msgq_get(def->dependencies.receiver_interface->rc_data_msgq_out,
                   &receiver_data, K_NO_WAIT) == 0) {
      // rc state
      rc_counter = 0;
      robot_state.rc_connected = true;

      // control mode: rc/can
      if (receiver_data.channels[6] < -0.5) {
        robot_state.control_mode = kControlModeRC;
        desired_motion.linear = receiver_data.channels[2];
        desired_motion.angular = -receiver_data.channels[1];

        if (desired_motion.linear < LINEAR_MOTION_DEADZONE &&
            desired_motion.linear > -LINEAR_MOTION_DEADZONE) {
          desired_motion.linear = 0;
        }
        if (desired_motion.angular < ANGULAR_MOTION_DEADZONE &&
            desired_motion.angular > -ANGULAR_MOTION_DEADZONE) {
          desired_motion.angular = 0;
        }
      } else {
        robot_state.control_mode = kControlModeCAN;
      }

      // estop: enable/disable
      if (receiver_data.channels[5] > 0) {
        robot_state.estop_triggered = false;
      } else {
        robot_state.estop_triggered = true;
      }
    } else {
      // no RC control
      if (rc_counter > rc_timeout_count) {
        robot_state.rc_connected = false;
        robot_state.estop_triggered = false;
        robot_state.control_mode = kControlModeCAN;
      } else {
        rc_counter++;
      }

      //   printk(
      //       "coordinator: %04f %04f %04f %04f, %04f %04f %04f %04f "
      //       "<-----------\n",
      //       receiver_data.channels[0], receiver_data.channels[1],
      //       receiver_data.channels[2], receiver_data.channels[3],
      //       receiver_data.channels[4], receiver_data.channels[5],
      //       receiver_data.channels[6], receiver_data.channels[7]);
    }

    /*********************** update control **************************/

    if (robot_state.estop_triggered) {
      desired_motion.linear = 0;
      desired_motion.angular = 0;
    }

    // send out desired motion if not in supervised mode
    if (robot_state.sup_mode == kNonSupervised) {
      if (robot_state.control_mode == kControlModeRC) {
        // send out desired motion command from RC
        while (
            k_msgq_put(
                def->dependencies.messenger_interface->desired_motion_msgq_in,
                &desired_motion, K_NO_WAIT) != 0) {
          k_msgq_purge(
              def->dependencies.messenger_interface->desired_motion_msgq_in);
        }
      }
    } else {
      // supervised mode: set PWM directly
      // TODO
    }

    // update robot state
    while (
        k_msgq_put(def->dependencies.messenger_interface->robot_state_msgq_in,
                   &robot_state, K_NO_WAIT) != 0) {
      k_msgq_purge(def->dependencies.messenger_interface->robot_state_msgq_in);
    }

    // printk(
    //     "rc connected: %d, estop: %d, control mode: %d, linear: %4f, angular:
    //     "
    //     "%4f\n",
    //     robot_state.rc_connected, robot_state.estop_triggered,
    //     robot_state.control_mode, desired_motion.linear,
    //     desired_motion.angular);

    // task timing
    ++loop_counter;
    k_msleep_until(def->tconf.period_ms, t0);
  }
}