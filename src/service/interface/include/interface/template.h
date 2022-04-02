/*
 * template.h
 *
 * Created on: Mar 06, 2022 15:41
 * Description:
 *
 * Copyright (c) 2022 Ruixiang Du (rdu)
 */

#ifndef TEMPLATE_H
#define TEMPLATE_H

#include "interface/service.h"

// service related config
typedef struct {
  int8_t* template_conf;
} TemplateSrvConf;

typedef struct {
  struct k_msgq* msgq;
} TemplateSrvData;

typedef struct {
  struct k_msgq* template_cmd_queue;
} TemplateInterface;

//---------------------------------------------------------//

typedef struct {
  // thread config
  ThreadConfig tconf;

  // service config
  TemplateSrvConf sconf;

  // dependent interfaces
  struct {
    // OtherServiceInterface* ibus;
  } dependencies;

  // interface
  TemplateInterface interface;
} TemplateServiceDef;

bool StartTemplateService(TemplateServiceDef* def);

#endif /* TEMPLATE_H */
