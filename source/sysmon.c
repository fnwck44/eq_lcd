/**
 * @file lv_sysmon.c
 *
 */

#include "platform.h"
#if PL_CONFIG_USE_GUI_SYSMON
#include "sysmon.h"
#include "LittlevGL/lvgl/lvgl.h"
#include <stdio.h>
#include "FreeRTOS.h"

/*********************
 *      DEFINES
 *********************/
#define CPU_LABEL_COLOR     "FF0000"
#define MEM_LABEL_COLOR     "0000FF"
#define CHART_POINT_NUM     100
#define REFR_TIME           500

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_obj_t *win;
static lv_obj_t *chart;
static lv_chart_series_t * cpu_ser;
static lv_chart_series_t * mem_ser;
static lv_obj_t * info_label;
static lv_task_t * refr_task;

/**
 * Called when the window's close button is clicked
 * @param btn pointer to the close button
 * @return LV_ACTION_RES_INV because the window is deleted in the function
 */
static void win_close_action(lv_obj_t *btn, lv_event_t event) {
  if (event==LV_EVENT_RELEASED) {
    lv_obj_del(win);
    win = NULL;

    lv_task_del(refr_task);
    refr_task = NULL;
  }
}

/**
 * Called periodically to monitor the CPU and memory usage.
 * @param param unused
 */
static void sysmon_task(struct _lv_task_t *param) {
    /*Get CPU and memory information */
    uint8_t cpu_busy;
    cpu_busy = 100 - lv_task_get_idle();

    uint8_t mem_used_pct = 0; /* used percentage */
#if  LV_MEM_CUSTOM == 0 /* built-in memory manager */
    lv_mem_monitor_t mem_mon;
    lv_mem_monitor(&mem_mon);
    mem_used_pct = mem_mon.used_pct;
#else /* FreerRTOS */
  #if configSUPPORT_DYNAMIC_ALLOCATION && configUSE_HEAP_SCHEME!=3 /* wrapper to malloc() does not have xPortGetFreeHeapSize() */
    size_t freeHeapSize = xPortGetFreeHeapSize();
  #endif
    mem_used_pct = (configTOTAL_HEAP_SIZE-freeHeapSize) / (configTOTAL_HEAP_SIZE/100);
#endif

    /*Add the CPU and memory data to the chart*/
    lv_chart_set_next(chart, cpu_ser, cpu_busy);
    lv_chart_set_next(chart, mem_ser, mem_used_pct);

    /*Refresh the and windows*/
    char buf_long[128];
    snprintf(buf_long, sizeof(buf_long), "%s%s CPU: %d%%%s\n\n",
            LV_TXT_COLOR_CMD,
            CPU_LABEL_COLOR,
            cpu_busy,
            LV_TXT_COLOR_CMD);
#if LV_MEM_CUSTOM == 0 /* built-in memory manager */
    snprintf(buf_long, sizeof(buf_long), "%s"LV_TXT_COLOR_CMD"%s MEMORY: %d%%"LV_TXT_COLOR_CMD"\n"
                      "Total: %d bytes\n"
                      "Used: %d bytes\n"
                      "Free: %d bytes\n"
                      "Frag: %d%%",
                      buf_long,
                      MEM_LABEL_COLOR,
                      mem_used_pct,
                      (int)mem_mon.total_size,
                      (int)mem_mon.total_size - mem_mon.free_size, mem_mon.free_size, mem_mon.frag_pct);

#elif 1 /* FreeRTOS */
    snprintf(buf_long, sizeof(buf_long), "%s"LV_TXT_COLOR_CMD"%s MEMORY: %d%%"LV_TXT_COLOR_CMD"\n"
                      "Total: %d bytes\n"
                      "Used: %d bytes\n"
                      "Free: %d bytes\n",
                      buf_long,
                      MEM_LABEL_COLOR,
					  mem_used_pct, /* percentage used */
					  configTOTAL_HEAP_SIZE, /* total */
                      configTOTAL_HEAP_SIZE-freeHeapSize, /* used */
					  freeHeapSize /* free */
					  );
#else
    sprintf(buf_long, "%s"LV_TXT_COLOR_CMD"%s MEMORY: N/A"LV_TXT_COLOR_CMD,
            buf_long,
            MEM_LABEL_COLOR);
#endif
    lv_label_set_text(info_label, buf_long);
}

/**
 * Initialize the system monitor
 */
void GUI_SysMon_Create(void) {
  lv_obj_t *closeBtn;

  refr_task = lv_task_create(sysmon_task, REFR_TIME, LV_TASK_PRIO_LOW, NULL);

  win = lv_win_create(lv_scr_act(), NULL);
  lv_win_set_title(win, "System Monitor");
  closeBtn = lv_win_add_btn(win, LV_SYMBOL_CLOSE);
  lv_obj_set_event_cb(closeBtn, win_close_action);

  /*Make the window content responsive*/
  lv_win_set_layout(win, LV_LAYOUT_PRETTY);

  /*Create a chart with two data lines*/
  chart = lv_chart_create(win, NULL);
  lv_obj_set_size(chart, LV_HOR_RES / 2, LV_VER_RES / 2);
  lv_obj_set_pos(chart, LV_DPI / 10, LV_DPI / 10);
  lv_chart_set_point_count(chart, CHART_POINT_NUM);
  lv_chart_set_range(chart, 0, 100);
  lv_chart_set_type(chart, LV_CHART_TYPE_LINE);
  lv_chart_set_series_width(chart, 4);
  cpu_ser =  lv_chart_add_series(chart, LV_COLOR_RED);
  mem_ser =  lv_chart_add_series(chart, LV_COLOR_BLUE);

  /*Set the data series to zero*/
  uint16_t i;
  for(i = 0; i < CHART_POINT_NUM; i++) {
      lv_chart_set_next(chart, cpu_ser, 0);
      lv_chart_set_next(chart, mem_ser, 0);
  }

  /*Create a label for the details of Memory and CPU usage*/
  info_label = lv_label_create(win, NULL);
  lv_label_set_recolor(info_label, true);
  lv_obj_align(info_label, chart, LV_ALIGN_OUT_RIGHT_TOP, LV_DPI / 4, 0);

  /*Refresh the chart and label manually at first*/
  sysmon_task(NULL);
}
#endif /* PL_CONFIG_USE_GUI_SYSMON */
