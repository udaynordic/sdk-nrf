.. _wifi_htol_sample:

Wi-Fi: HTOL
###########

This sample is a starting point for **host-to-LMAC (HTOL)** and **Wi-Fi ROM patch**
bring-up on nRF71 Series SoCs.

It mirrors the layout of the :ref:`wifi_scan_sample`, but it does **not** enable
the Zephyr nRF71 Wi-Fi driver, WPA supplicant, or networking stack. Only SoC
ROM patch inclusion (``CONFIG_WIFI_NRF71_PATCH``, LMAC/UMAC binaries merged
into the flash image) and application-driven Wi-Fi hardware bring-up
(:file:`src/wifi_subsys_bringup.c`, modeled on :c:func:`nordicsemi_nrf71_init`)
are enabled. ``CONFIG_SOC_NRF71_WIFI_BOOT`` stays off so the LMAC is started
from ``main()`` instead of early SoC init.

Requirements
************

The sample supports the nRF7120 DK:

.. table-from-sample-yaml::

Building and running
********************

.. |sample path| replace:: :file:`samples/wifi/htol`

.. include:: /includes/build_and_run_ns.txt

Example:

.. code-block:: console

   west build -b nrf7120dk/nrf7120e/cpuapp samples/wifi/htol
   west flash

Flash the merged image (``zephyr/zephyr.signed.wifi.hex`` when MCUboot is enabled,
or ``zephyr/zephyr.wifi.hex`` otherwise) so ROM patch binaries are programmed at
the addresses defined in WICR.

Next steps
**********

Add HTOL / patch verification logic in :file:`src/main.c` without enabling
``CONFIG_WIFI_NRF71`` until the full driver is needed.