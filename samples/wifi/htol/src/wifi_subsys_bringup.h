/*
 * Copyright (c) 2026 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#ifndef WIFI_SUBSYS_BRINGUP_H__
#define WIFI_SUBSYS_BRINGUP_H__

/**
 * Bring up the nRF71 Wi-Fi hardware without the Zephyr Wi-Fi driver.
 *
 * Mirrors the Wi-Fi-related steps in @c nordicsemi_nrf71_init(): WICR
 * programming, antenna switch power/steering, RPU power/clock enable, and
 * LMAC/UMAC VPR start from WICR init PCs.
 * ROM LMAC/UMAC patch binaries are still merged via @kconfig{CONFIG_WIFI_NRF71_PATCH}.
 *
 * @return 0 on success, negative errno on failure.
 */
int htol_wifi_subsys_bringup(void);

#endif /* WIFI_SUBSYS_BRINGUP_H__ */