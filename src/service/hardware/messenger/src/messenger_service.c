/* 
 * messenger_service.c
 *
 * Created on 3/16/22 10:22 PM
 * Description:
 *
 * Copyright (c) 2022 Ruixiang Du (rdu)
 */

#include "messenger/messenger_service.h"

static void MessengerServiceLoop(void *p1, void *p2, void *p3);

bool StartMessengerService(MessengerServiceDef *def) {
  // create and start thread
  def->tconf.tid = k_thread_create(&def->tconf.thread, def->tconf.stack,
                                   K_THREAD_STACK_SIZEOF(def->tconf.stack),
                                   MessengerServiceLoop, def, NULL, NULL,
                                   def->tconf.priority, 0, def->tconf.delay);
  return true;
}

void MessengerServiceLoop(void *p1, void *p2, void *p3) {

}