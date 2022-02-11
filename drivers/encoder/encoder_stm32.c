/*
 * Copyright (c) 2022 Ruixiang Du (ruixiang.du@gmail.com)
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define DT_DRV_COMPAT st_stm32_encoder

#include <errno.h>

#include <soc.h>
#include <stm32_ll_rcc.h>
#include <stm32_ll_tim.h>
#include <drivers/encoder.h>
#include <device.h>
#include <kernel.h>
#include <init.h>

#include <drivers/clock_control/stm32_clock_control.h>
#include <pinmux/pinmux_stm32.h>

#include <logging/log.h>
LOG_MODULE_REGISTER(encoder_stm32, CONFIG_ENCODER_LOG_LEVEL);

/* L0 series MCUs only have 16-bit timers and don't have below macro defined */
#ifndef IS_TIM_32B_COUNTER_INSTANCE
#define IS_TIM_32B_COUNTER_INSTANCE(INSTANCE) (0)
#endif

/** ENCODER data. */
struct encoder_stm32_data {
  /** Timer clock (Hz). */
  uint32_t tim_clk;
};

/** ENCODER configuration. */
struct encoder_stm32_config {
  /** Timer instance. */
  TIM_TypeDef *timer;
  /** Encoder mode */
  uint32_t encoder_mode;
  /** Prescaler. */
  uint32_t prescaler;
  /** Clock configuration. */
  struct stm32_pclken pclken;
  /** pinctrl configurations. */
  const struct soc_gpio_pinctrl *pinctrl;
  /** Number of pinctrl configurations. */
  size_t pinctrl_len;
};

/**
 * Obtain timer clock speed.
 *
 * @param pclken  Timer clock control subsystem.
 * @param tim_clk Where computed timer clock will be stored.
 *
 * @return 0 on success, error code otherwise.
 */
static int get_tim_clk(const struct stm32_pclken *pclken, uint32_t *tim_clk) {
  int r;
  const struct device *clk;
  uint32_t bus_clk, apb_psc;

  clk = DEVICE_DT_GET(STM32_CLOCK_CONTROL_NODE);

  r = clock_control_get_rate(clk, (clock_control_subsys_t *)pclken, &bus_clk);
  if (r < 0) {
    return r;
  }

#if defined(CONFIG_SOC_SERIES_STM32H7X)
  if (pclken->bus == STM32_CLOCK_BUS_APB1) {
    apb_psc = STM32_D2PPRE1;
  } else {
    apb_psc = STM32_D2PPRE2;
  }
#else
  if (pclken->bus == STM32_CLOCK_BUS_APB1) {
    apb_psc = STM32_APB1_PRESCALER;
  }
#if !defined(CONFIG_SOC_SERIES_STM32F0X) && !defined(CONFIG_SOC_SERIES_STM32G0X)
  else {
    apb_psc = STM32_APB2_PRESCALER;
  }
#endif
#endif

#if defined(RCC_DCKCFGR_TIMPRE) || defined(RCC_DCKCFGR1_TIMPRE) || \
    defined(RCC_CFGR_TIMPRE)
  /*
   * There are certain series (some F4, F7 and H7) that have the TIMPRE
   * bit to control the clock frequency of all the timers connected to
   * APB1 and APB2 domains.
   *
   * Up to a certain threshold value of APB{1,2} prescaler, timer clock
   * equals to HCLK. This threshold value depends on TIMPRE setting
   * (2 if TIMPRE=0, 4 if TIMPRE=1). Above threshold, timer clock is set
   * to a multiple of the APB domain clock PCLK{1,2} (2 if TIMPRE=0, 4 if
   * TIMPRE=1).
   */

  if (LL_RCC_GetTIMPrescaler() == LL_RCC_TIM_PRESCALER_TWICE) {
    /* TIMPRE = 0 */
    if (apb_psc <= 2u) {
      LL_RCC_ClocksTypeDef clocks;

      LL_RCC_GetSystemClocksFreq(&clocks);
      *tim_clk = clocks.HCLK_Frequency;
    } else {
      *tim_clk = bus_clk * 2u;
    }
  } else {
    /* TIMPRE = 1 */
    if (apb_psc <= 4u) {
      LL_RCC_ClocksTypeDef clocks;

      LL_RCC_GetSystemClocksFreq(&clocks);
      *tim_clk = clocks.HCLK_Frequency;
    } else {
      *tim_clk = bus_clk * 4u;
    }
  }
#else
  /*
   * If the APB prescaler equals 1, the timer clock frequencies
   * are set to the same frequency as that of the APB domain.
   * Otherwise, they are set to twice (×2) the frequency of the
   * APB domain.
   */
  if (apb_psc == 1u) {
    *tim_clk = bus_clk;
  } else {
    *tim_clk = bus_clk * 2u;
  }
#endif

  return 0;
}

int encoder_stm32_get_counts(const struct device *dev, uint32_t *counts) {
  const struct encoder_stm32_config *cfg = dev->config;
  *counts = LL_TIM_GetCounter(cfg->timer);
  return 0;
}

int encoder_stm32_is_counting_up(const struct device *dev, bool *counting_up) {
  const struct encoder_stm32_config *cfg = dev->config;
  *counting_up = (LL_TIM_GetCounterMode(cfg->timer) == LL_TIM_COUNTERMODE_UP);
  return 0;
}

static const struct encoder_driver_api encoder_stm32_driver_api = {
    .get_counts = encoder_stm32_get_counts,
    .is_counting_up = encoder_stm32_is_counting_up};

static int encoder_stm32_init(const struct device *dev) {
  struct encoder_stm32_data *data = dev->data;
  const struct encoder_stm32_config *cfg = dev->config;

  int r;
  const struct device *clk;
  LL_TIM_InitTypeDef init;

  /* enable clock and store its speed */
  clk = DEVICE_DT_GET(STM32_CLOCK_CONTROL_NODE);

  r = clock_control_on(clk, (clock_control_subsys_t *)&cfg->pclken);
  if (r < 0) {
    LOG_ERR("Could not initialize clock (%d)", r);
    return r;
  }

  r = get_tim_clk(&cfg->pclken, &data->tim_clk);
  if (r < 0) {
    LOG_ERR("Could not obtain timer clock (%d)", r);
    return r;
  }

  /* configure pinmux */
  r = stm32_dt_pinctrl_configure(cfg->pinctrl, cfg->pinctrl_len,
                                 (uint32_t)cfg->timer);
  if (r < 0) {
    LOG_ERR("ENCODER pinctrl setup failed (%d)", r);
    return r;
  }

  /* configure encoder mode */
  if (cfg->encoder_mode == ENCODER_MODE_X2_TI1) {
    LL_TIM_SetEncoderMode(cfg->timer, LL_TIM_ENCODERMODE_X2_TI1);
  } else if (cfg->encoder_mode == ENCODER_MODE_X2_TI2) {
    LL_TIM_SetEncoderMode(cfg->timer, LL_TIM_ENCODERMODE_X2_TI2);
  } else if (cfg->encoder_mode == ENCODER_MODE_X4_TI12) {
    LL_TIM_SetEncoderMode(cfg->timer, LL_TIM_ENCODERMODE_X4_TI12);
  } else {
    LOG_ERR("Invalid encoder mode");
    return -EIO;
  }
  LL_TIM_IC_SetActiveInput(cfg->timer, LL_TIM_CHANNEL_CH1,
                           LL_TIM_ACTIVEINPUT_DIRECTTI);
  LL_TIM_IC_SetPrescaler(cfg->timer, LL_TIM_CHANNEL_CH1, LL_TIM_ICPSC_DIV1);
  LL_TIM_IC_SetFilter(cfg->timer, LL_TIM_CHANNEL_CH1, LL_TIM_IC_FILTER_FDIV1);
  LL_TIM_IC_SetPolarity(cfg->timer, LL_TIM_CHANNEL_CH1,
                        LL_TIM_IC_POLARITY_RISING);
  LL_TIM_IC_SetActiveInput(cfg->timer, LL_TIM_CHANNEL_CH2,
                           LL_TIM_ACTIVEINPUT_DIRECTTI);
  LL_TIM_IC_SetPrescaler(cfg->timer, LL_TIM_CHANNEL_CH2, LL_TIM_ICPSC_DIV1);
  LL_TIM_IC_SetFilter(cfg->timer, LL_TIM_CHANNEL_CH2, LL_TIM_IC_FILTER_FDIV1);
  LL_TIM_IC_SetPolarity(cfg->timer, LL_TIM_CHANNEL_CH2,
                        LL_TIM_IC_POLARITY_RISING);

  /* initialize timer */
  LL_TIM_StructInit(&init);

  init.Prescaler = cfg->prescaler;
  init.CounterMode = LL_TIM_COUNTERMODE_UP;
  init.Autoreload = 65535u;
  init.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
  init.RepetitionCounter = 0u;

  if (LL_TIM_Init(cfg->timer, &init) != SUCCESS) {
    LOG_ERR("Could not initialize timer");
    return -EIO;
  }

  LL_TIM_DisableARRPreload(cfg->timer);
  LL_TIM_SetTriggerOutput(cfg->timer, LL_TIM_TRGO_RESET);
  LL_TIM_DisableMasterSlaveMode(cfg->timer);
  LL_TIM_EnableCounter(cfg->timer);

  return 0;
}

#define DT_INST_CLK(index, inst)                               \
  {                                                            \
    .bus = DT_CLOCKS_CELL(DT_PARENT(DT_DRV_INST(index)), bus), \
    .enr = DT_CLOCKS_CELL(DT_PARENT(DT_DRV_INST(index)), bits) \
  }

#define ENCODER_DEVICE_INIT(index)                                          \
  static struct encoder_stm32_data encoder_stm32_data_##index;              \
                                                                            \
  static const struct soc_gpio_pinctrl encoder_pins_##index[] =             \
      ST_STM32_DT_INST_PINCTRL(index, 0);                                   \
                                                                            \
  static const struct encoder_stm32_config encoder_stm32_config_##index = { \
      .timer = (TIM_TypeDef *)DT_REG_ADDR(DT_PARENT(DT_DRV_INST(index))),   \
      .encoder_mode = DT_INST_PROP(index, st_encoder_mode),                 \
      .prescaler = DT_INST_PROP(index, st_prescaler),                       \
      .pclken = DT_INST_CLK(index, timer),                                  \
      .pinctrl = encoder_pins_##index,                                      \
      .pinctrl_len = ARRAY_SIZE(encoder_pins_##index),                      \
  };                                                                        \
                                                                            \
  DEVICE_DT_INST_DEFINE(                                                    \
      index, &encoder_stm32_init, NULL, &encoder_stm32_data_##index,        \
      &encoder_stm32_config_##index, POST_KERNEL,                           \
      CONFIG_KERNEL_INIT_PRIORITY_DEVICE, &encoder_stm32_driver_api);

DT_INST_FOREACH_STATUS_OKAY(ENCODER_DEVICE_INIT)