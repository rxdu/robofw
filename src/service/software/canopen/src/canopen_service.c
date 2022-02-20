/*
 * Copyright (c) 2019 Vestas Wind Systems A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "canopen/canopen_service.h"

#include <zephyr.h>
#include <drivers/gpio.h>
#include <sys/reboot.h>
#include <settings/settings.h>
#include <canopennode.h>

#define LOG_LEVEL CONFIG_CANOPEN_LOG_LEVEL
#include <logging/log.h>
LOG_MODULE_REGISTER(app);

#if DT_NODE_HAS_PROP(DT_ALIAS(sw0), gpios)
#define BUTTON_PORT DT_GPIO_LABEL(DT_ALIAS(sw0), gpios)
#define BUTTON_PIN DT_GPIO_PIN(DT_ALIAS(sw0), gpios)
#define BUTTON_FLAGS DT_GPIO_FLAGS(DT_ALIAS(sw0), gpios)
static struct gpio_callback button_callback;
#endif

static void CanopenServiceLoop(void *p1, void *p2, void *p3);

static uint32_t counter;

/**
 * @brief Button press counter object dictionary handler function.
 *
 * This function is called upon SDO access to the button press counter
 * object (index 0x2102) in the object dictionary.
 *
 * @param odf_arg object dictionary function argument.
 *
 * @return SDO abort code.
 */
static CO_SDO_abortCode_t odf_2102(CO_ODF_arg_t *odf_arg)
{
	uint32_t value;

	value = CO_getUint32(odf_arg->data);

	if (odf_arg->reading) {
		return CO_SDO_AB_NONE;
	}

	if (odf_arg->subIndex != 0U) {
		return CO_SDO_AB_NONE;
	}

	if (value != 0) {
		/* Preserve old value */
		memcpy(odf_arg->data, odf_arg->ODdataStorage, sizeof(uint32_t));
		return CO_SDO_AB_DATA_TRANSF;
	}

	LOG_INF("Resetting button press counter");
	counter = 0;

	return CO_SDO_AB_NONE;
}

bool StartCanopenService(CanopenServiceConf *cfg)
{
	// create and start thread
	cfg->tid = k_thread_create(cfg->thread, cfg->stack, cfg->stack_size, CanopenServiceLoop,
				   cfg, NULL, NULL, cfg->priority, 0, cfg->delay);
	return true;
}

/**
 * @brief Main application entry point.
 *
 * The main application thread is responsible for initializing the
 * CANopen stack and doing the non real-time processing.
 */
// void main(void)
void CanopenServiceLoop(void *p1, void *p2, void *p3)
{
	CanopenServiceConf *cfg = (CanopenServiceConf *)p1;

	CO_NMT_reset_cmd_t reset = CO_RESET_NOT;
	CO_ReturnError_t err;
	struct canopen_context can;
	uint16_t timeout;
	uint32_t elapsed;
	int64_t timestamp;
#ifdef CONFIG_CANOPENNODE_STORAGE
	int ret;
#endif /* CONFIG_CANOPENNODE_STORAGE */

	can.dev = cfg->can_cfg->dd_can->device;
	if (!can.dev) {
		LOG_ERR("CAN interface not found");
		return;
	}
	printk("CAN bitrate: %dk\n", cfg->can_cfg->dd_can->bitrate / 1000);

#ifdef CONFIG_CANOPENNODE_STORAGE
	ret = settings_subsys_init();
	if (ret) {
		LOG_ERR("failed to initialize settings subsystem (err = %d)", ret);
		return;
	}

	ret = settings_load();
	if (ret) {
		LOG_ERR("failed to load settings (err = %d)", ret);
		return;
	}
#endif /* CONFIG_CANOPENNODE_STORAGE */

	OD_powerOnCounter++;

	while (reset != CO_RESET_APP) {
		elapsed = 0U; /* milliseconds */

		err = CO_init(&can, CONFIG_CANOPEN_NODE_ID, cfg->can_cfg->dd_can->bitrate / 1000);
		if (err != CO_ERROR_NO) {
			LOG_ERR("CO_init failed (err = %d)", err);
			return;
		}

		LOG_INF("CANopen stack initialized");

#ifdef CONFIG_CANOPENNODE_STORAGE
		canopen_storage_attach(CO->SDO[0], CO->em);
#endif /* CONFIG_CANOPENNODE_STORAGE */

		CO_OD_configure(CO->SDO[0], OD_2102_buttonPressCounter, odf_2102, NULL, 0U, 0U);

		if (IS_ENABLED(CONFIG_CANOPENNODE_PROGRAM_DOWNLOAD)) {
			canopen_program_download_attach(CO->NMT, CO->SDO[0], CO->em);
		}

		CO_CANsetNormalMode(CO->CANmodule[0]);

		while (true) {
			timeout = 1U; /* default timeout in milliseconds */
			timestamp = k_uptime_get();
			reset = CO_process(CO, (uint16_t)elapsed, &timeout);

			if (reset != CO_RESET_NOT) {
				break;
			}

			if (timeout > 0) {
				CO_LOCK_OD();
				OD_buttonPressCounter = counter;
                OD_speedFeedback[ODA_speedFeedback_rpm_left] = 250;
                OD_speedFeedback[ODA_speedFeedback_rpm_right] = 450;
                OD_encoderFeedback[ODA_encoderFeedback_encoder_left] = 550;
                OD_encoderFeedback[ODA_encoderFeedback_encoder_right] = 650;
				CO_UNLOCK_OD();

                CO->TPDO[0]->sendRequest = true;
                CO->TPDO[1]->sendRequest = true;

#ifdef CONFIG_CANOPENNODE_STORAGE
				ret = canopen_storage_save(CANOPEN_STORAGE_EEPROM);
				if (ret) {
					LOG_ERR("failed to save EEPROM");
				}
#endif /* CONFIG_CANOPENNODE_STORAGE */
				/*
				 * Try to sleep for as long as the
				 * stack requested and calculate the
				 * exact time elapsed.
				 */
				k_sleep(K_MSEC(timeout));
				elapsed = (uint32_t)k_uptime_delta(&timestamp);
			} else {
				/*
				 * Do not sleep, more processing to be
				 * done by the stack.
				 */
				elapsed = 0U;
			}
		}

		if (reset == CO_RESET_COMM) {
			LOG_INF("Resetting communication");
		}
	}

	LOG_INF("Resetting device");

	CO_delete(&can);
	sys_reboot(SYS_REBOOT_COLD);
}
