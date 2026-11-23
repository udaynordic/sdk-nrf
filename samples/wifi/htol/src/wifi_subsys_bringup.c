/*
 * Copyright (c) 2026 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <errno.h>

#include <zephyr/autoconf.h>
#include <zephyr/devicetree.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/sys_io.h>

#include <nrfx.h>
#include <hal/nrf_gpio.h>
#include <hal/nrf_lrcconf.h>
#include <hal/nrf_vpr.h>
#include <soc.h>
#include <wicr_setup.h>

LOG_MODULE_DECLARE(htol, CONFIG_LOG_DEFAULT_LEVEL);

/* WIFISYS RPUSYS — same as nrf/drivers/wifi/nrf71/src/system/wifi_pm.c */
#define WIFICORE_RPUSYS_BASE            0x48080000UL
#define WIFI_AUTOCG_SET_ALL             (WIFICORE_RPUSYS_BASE + 0x1B014UL)
#define WIFI_AUTOCG_CLR_ALL             (WIFICORE_RPUSYS_BASE + 0x1B018UL)
#define WIFI_RF_CLOCK_CTRL_SET          (WIFICORE_RPUSYS_BASE + 0x1B024UL)
#define WIFI_RAM_ENABLE_SET_ALL         (WIFICORE_RPUSYS_BASE + 0x1B044UL)
#define WIFI_ENABLE_ALL_TRIGGER         1UL
#define WIFI_RF_CLOCK_INT_EXT           0x3UL
#define WIFI_SYSTEM_OFF_TOKEN           0x280002FCUL

#if DT_HAS_COMPAT_STATUS_OKAY(nordic_nrf71_antsw)
#define ANTSW_NODE DT_COMPAT_GET_ANY_STATUS_OKAY(nordic_nrf71_antsw)

BUILD_ASSERT(DT_HAS_COMPAT_STATUS_OKAY(nordic_nrf_pwr_antswc),
	     "antsw steering requires pwr_antswc to power the antenna switch");

static void antsw_setup(void)
{
	uint32_t sel_psel = NRF_DT_GPIOS_TO_PSEL(ANTSW_NODE, sel_gpios);

	if (IS_ENABLED(CONFIG_SOC_SERIES_NRF71_ANTSW_DEFAULT_BLE)) {
		nrf_gpio_pin_set(sel_psel);
	} else {
		nrf_gpio_pin_clear(sel_psel);
	}
	nrf_gpio_cfg_output(sel_psel);
}
#endif /* nordic_nrf71_antsw */

static void wifi_rpu_power_on(void)
{
	nrf_lrcconf_poweron_force_set(NRF_WIFICORE_LRCCONF_LRC0,
				      NRF_LRCCONF_POWER_MAIN, true);

	sys_write32(0, WIFI_SYSTEM_OFF_TOKEN);
	sys_write32(WIFI_ENABLE_ALL_TRIGGER, WIFI_AUTOCG_SET_ALL);
	sys_write32(WIFI_ENABLE_ALL_TRIGGER, WIFI_RAM_ENABLE_SET_ALL);
	sys_write32(WIFI_RF_CLOCK_INT_EXT, WIFI_RF_CLOCK_CTRL_SET);
	__DSB();
}

static void wifi_vpr_start(NRF_VPR_Type *vpr, uint32_t init_pc)
{
	nrf_vpr_initpc_set(vpr, init_pc);
	nrf_vpr_cpurun_set(vpr, true);
}

static void wifi_processors_start(void)
{
	/*
	 * Same sequence as soc.c wifi_setup() / nrf_wifi_power_on(), plus
	 * explicit UMAC VPR start from WICR UMACINITPC.
	 */
	wifi_rpu_power_on();

	wifi_vpr_start(NRF_WIFICORE_LMAC_VPR,
		       (uint32_t)(uintptr_t)NRF_WICR->FIRMWARE.LMACINITPC);
	wifi_vpr_start(NRF_WIFICORE_UMAC_VPR,
		       (uint32_t)(uintptr_t)NRF_WICR->FIRMWARE.UMACINITPC);
}

int htol_wifi_subsys_bringup(void)
{
	int err = 0;

	LOG_INF("Wi-Fi subsystem bring-up (no driver)");

#if defined(CONFIG_SOC_NRF7120_WICR_SETUP) && !defined(CONFIG_BUILD_WITH_TFM)
	err = wicr_setup();
	if (err != 0) {
		LOG_ERR("WICR setup failed: %d", err);
		return err;
	}
	LOG_INF("WICR programmed");
#elif defined(CONFIG_BUILD_WITH_TFM)
	LOG_DBG("WICR setup handled by TF-M secure firmware");
#endif

#if DT_HAS_COMPAT_STATUS_OKAY(nordic_nrf_pwr_antswc)
	*(volatile uint32_t *)PWR_ANTSWC_REG |= PWR_ANTSWC_ENABLE;
	LOG_DBG("Antenna switch powered");
#endif

#if DT_HAS_COMPAT_STATUS_OKAY(nordic_nrf71_antsw)
	antsw_setup();
	LOG_DBG("Antenna switch steered to default radio");
#endif

#if defined(NRF_APPLICATION)
	wifi_processors_start();
	LOG_INF("LMAC VPR @ %p, UMAC VPR @ %p",
		(void *)(uintptr_t)NRF_WICR->FIRMWARE.LMACINITPC,
		(void *)(uintptr_t)NRF_WICR->FIRMWARE.UMACINITPC);
#else
	LOG_ERR("Wi-Fi bring-up requires the application core");
	return -ENOTSUP;
#endif

	return err;
}