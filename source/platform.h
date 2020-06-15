/*
 * Copyright (c) 2019, Erich Styger
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef PLATFORM_H_
#define PLATFORM_H_

#define PL_CONFIG_USE_I2C               (1)
#define PL_CONFIG_USE_HW_I2C            (0 && PL_CONFIG_USE_I2C) /* otherwise uses bit-banging */
#define PL_CONFIG_USE_FT6206            (0 && PL_CONFIG_USE_I2C) /* capacitive touch controller */
#define PL_CONFIG_USE_STMPE610          (1) /* resistive touch controller */
#define PL_CONFIG_USE_GUI               (1)
#define PL_CONFIG_USE_SHELL             (1)
#define PL_CONFIG_USE_USB_CDC           (0)
#define PL_CONFIG_USE_GUI_KEY_NAV       (0)
#define PL_CONFIG_USE_GUI_TOUCH_NAV     (1 && (PL_CONFIG_USE_FT6206 || PL_CONFIG_USE_STMPE610)) /* if using touch on display */
#define PL_CONFIG_USE_GUI_KEYPAD_NAV    (0) /* keyboard on LCD */
#define PL_CONFIG_USE_GUI_SCREEN_SAVER  (0) /* By default, it turns off the display */
#define PL_CONFIG_USE_TOASTER           (0 && PL_CONFIG_USE_GUI_SCREEN_SAVER) /* Not yet implemented! */
#define PL_CONFIG_USE_GUI_SYSMON        (1)

#if PL_CONFIG_USE_FT6206 && PL_CONFIG_USE_STMPE610
  #error "only one touch controller can be active"
#endif

void PL_Init(void);
void PL_Deinit(void);

#endif /* PLATFORM_H_ */
