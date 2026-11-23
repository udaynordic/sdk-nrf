/*
 * Copyright (c) 2026 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

/** @file
 *  @brief Minimal nRF71 HTOL / Wi-Fi ROM patch test harness
 *
 *  Wi-Fi ROM patches are merged into the application image (see
 *  CONFIG_WIFI_NRF71_PATCH). The Zephyr nRF71 Wi-Fi driver and WPA
 *  supplicant are intentionally disabled; use this sample to exercise
 *  host-to-LMAC patch loading before pulling in the full Wi-Fi stack.
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "wifi_subsys_bringup.h"

LOG_MODULE_REGISTER(htol, CONFIG_LOG_DEFAULT_LEVEL);

int main(void)
{
	int err;

	LOG_INF("nRF71 HTOL sample (ROM patches in image, no Wi-Fi driver)");

	err = htol_wifi_subsys_bringup();
	if (err != 0) {
		LOG_ERR("Bring-up failed (%d)", err);
		return err;
	}

	while (true) {
		k_sleep(K_SECONDS(CONFIG_HTOL_HEARTBEAT_INTERVAL_S));
		LOG_INF("Heartbeat — add HTOL patch test steps here");
	}

	return 0;
}