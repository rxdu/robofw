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

#include "receiver/receiver_service.h"
#include "messenger/messenger_service.h"
#include "motion_control/motion_control_service.h"

K_THREAD_STACK_DEFINE(coord_service_stack, 1024);

_Noreturn static void CoordinatorServiceLoop(void *p1, void *p2, void *p3);

bool StartCoordinatorService(CoordinatorServiceDef *def) {
  //   if (def->sdata.robot_state_msgq == NULL) return false;
  //   def->interface.robot_state_msgq_out = def->sdata.robot_state_msgq;

  //  if (def->sdata.control_feedback_msgq == NULL) return false;
  //  def->interface.control_feedback_msgq_out =
  //  def->sdata.control_feedback_msgq;

  // sanity check
  if (def->dependencies.receiver_interface == NULL ||
      def->dependencies.messenger_interface == NULL ||
      def->dependencies.motion_control_interface == NULL) {
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
  robot_state.estop_triggered = false;
  robot_state.control_mode = kControlModeCAN;
  robot_state.sup_mode = kNonSupervised;

  DesiredMotion desired_motion;
  desired_motion.linear = 0;
  desired_motion.angular = 0;

  uint8_t loop_counter = 0;

  uint8_t rc_counter = 0;
  const uint8_t rc_timeout_count = 5;

  uint8_t can_counter = 0;
  const uint8_t can_timeout_count = 5;

  while (1) {
    int64_t t0 = k_loop_start();

    if (loop_counter % 25 == 0) ToggleLed(def->sconf.dd_led_status);

    if (k_msgq_get(def->dependencies.receiver_interface->rc_data_msgq_out,
                   &receiver_data, K_NO_WAIT) == 0) {
      //      printk(
      //          "coordinator: %04f %04f %04f %04f, %04f %04f %04f %04f "
      //          "<-----------\n",
      //          receiver_data.channels[0], receiver_data.channels[1],
      //          receiver_data.channels[2], receiver_data.channels[3],
      //          receiver_data.channels[4], receiver_data.channels[5],
      //          receiver_data.channels[6], receiver_data.channels[7]);

      // rc state
      rc_counter = 0;
      robot_state.rc_connected = true;

      // control mode: rc/can
      if (receiver_data.channels[7] < FLOAT_VALUE_DELTA &&
          receiver_data.channels[7] > -FLOAT_VALUE_DELTA) {
        robot_state.control_mode = kControlModeRC;
        desired_motion.linear = receiver_data.channels[2];
        desired_motion.angular = -receiver_data.channels[0];
      } else {
        robot_state.control_mode = kControlModeCAN;
      }

      // estop: enable/disable
      if (receiver_data.channels[6] > 0) {
        robot_state.estop_triggered = true;
      } else {
        robot_state.estop_triggered = false;
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
    }

    if (robot_state.control_mode == kControlModeCAN) {
      if (k_msgq_get(
              def->dependencies.messenger_interface->desired_motion_msgq_out,
              &desired_motion_from_can, K_NO_WAIT) == 0) {
        can_counter = 0;
      } else {
        if (can_counter > can_timeout_count) {
          desired_motion_from_can.linear = 0;
          desired_motion_from_can.angular = 0;
        } else {
          can_counter++;
        }
      }

      desired_motion.linear = desired_motion_from_can.linear;
      desired_motion.angular = desired_motion_from_can.angular;
    }

    // check supervisor command
    if (k_msgq_get(
            def->dependencies.messenger_interface->supervisor_cmd_msgq_out,
            &sup_cmd, K_NO_WAIT) == 0) {
      robot_state.sup_mode = sup_cmd.supervised_mode;
    }

    //    printk(
    //        "rc: %s, estop: %s, sup: %d, mode: %d, linear: %.4f, angular:
    //        %.4f\n", robot_state.rc_connected ? "on" : "off",
    //        robot_state.estop_triggered ? "on" : "off", robot_state.sup_mode,
    //        robot_state.control_mode, desired_motion.linear,
    //        desired_motion.angular);

    // send out desired motion if not in supervised mode
    if (robot_state.sup_mode == kNonSupervised) {
      if (robot_state.estop_triggered) {
        desired_motion.linear = 0;
        desired_motion.angular = 0;
      }

      // send out desired motion command
      while (k_msgq_put(def->dependencies.motion_control_interface
                            ->desired_motion_msgq_in,
                        &desired_motion, K_NO_WAIT) != 0) {
        k_msgq_purge(
            def->dependencies.motion_control_interface->desired_motion_msgq_in);
      }
    }

    // update robot state
    while (
        k_msgq_put(def->dependencies.messenger_interface->robot_state_msgq_in,
                   &robot_state, K_NO_WAIT) != 0) {
      k_msgq_purge(def->dependencies.messenger_interface->robot_state_msgq_in);
    }

    // task timing
    ++loop_counter;
    k_msleep_until(def->tconf.period_ms, t0);
  }
}