/*
 * Copyright (c) 2019, Erich Styger
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef GUI_H_
#define GUI_H_

#include "platform.h"
#include "McuGDisplaySSD1306.h"
#include "lv.h"

#if PL_CONFIG_USE_GUI_KEY_NAV
  #include "LittlevGL/lvgl/lv_core/lv_group.h"

  lv_group_t *GUI_GroupPeek(void);
  void GUI_GroupPull(void);
  void GUI_GroupPush(void);

  void GUI_AddObjToGroup(lv_obj_t *obj);
  void GUI_RemoveObjFromGroup(lv_obj_t *obj);
#endif

void GUI_SwitchToMainScreen(void);

void GUI_ChangeOrientation(McuGDisplaySSD1306_DisplayOrientation orientation);

void GUI_Init(void);

static void btn_band_1(lv_obj_t *obj, lv_event_t event);

int gain_value(lv_obj_t *obj);

#endif /* GUI_H_ */
