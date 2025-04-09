
/**
 * @file main
 *
 */

/*********************
 *      INCLUDES
 *********************/
#define _DEFAULT_SOURCE /* needed for usleep() */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "lvgl/lvgl.h"
#include "lvgl/examples/lv_examples.h"
#include "lvgl/demos/lv_demos.h"
#include "glob.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_display_t * hal_init(int32_t w, int32_t h);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

extern void freertos_main(void);

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *      VARIABLES
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_style_t background_grid_array[64];
lv_obj_t **pause_address;
lv_obj_t **reset_address;
lv_obj_t **options_address;
bool play = false;
bool pieces[64];

 void lv_grid_1(void)
 {
     static int32_t col_dsc[] = {40, 40, 40, 40, 40, 40, 40, 40, LV_GRID_TEMPLATE_LAST};
     static int32_t row_dsc[] = {40, 40, 40, 40, 40, 40, 40, 40, LV_GRID_TEMPLATE_LAST};

     /*Create a container with grid*/
     lv_obj_t * cont = lv_obj_create(lv_screen_active());
     lv_obj_set_style_grid_column_dsc_array(cont, col_dsc, 0);
     lv_obj_set_style_grid_row_dsc_array(cont, row_dsc, 0);
     lv_obj_set_size(cont, 480, 320);
     lv_obj_center(cont);
     lv_obj_set_layout(cont, LV_LAYOUT_GRID);
     lv_obj_set_style_pad_left(cont, 0, 0);
     lv_obj_set_style_pad_top(cont, 0, 0);
     lv_obj_set_style_pad_row(cont, 0, 0);
     lv_obj_set_style_pad_column(cont, 0, 0);

     lv_obj_t * label;
     lv_obj_t * obj;

     uint8_t i;
     for(i = 0; i < 64; i++) {
         uint8_t col = i % 8;
         uint8_t row = i / 8;

         obj = lv_button_create(cont);
         /*Stretch the cell horizontally and vertically too
         *Set span to 1 to make the cell 1 column/row sized*/
         lv_style_t* style_cell = &background_grid_array[i];
         lv_style_init(style_cell);
         printf("i: %d", i);
         bool black = col % 2 ^ row % 2;
         if (!black) {
            lv_style_set_bg_color(style_cell, lv_color_white());
         }
         else {
            lv_style_set_bg_color(style_cell, lv_color_black());
         }
         lv_style_set_radius(style_cell, 0);
         lv_style_set_bg_opa(style_cell, LV_OPA_100);
         lv_style_set_border_color(style_cell, lv_color_black());
         lv_style_set_border_side(style_cell, LV_BORDER_SIDE_RIGHT);
         if (i % 8 == 7) {
          lv_style_set_border_width(style_cell, 5);
         }
         else {
          lv_style_set_border_width(style_cell, 0);
         }
         // lv_style_set_pad_all(style_cell, 0);
         lv_obj_add_style(obj, style_cell, 0);
        //  lv_obj_set_style_pad_row(obj, 0, 0);
        //  lv_obj_set_style_pad_column(obj, 0, 0);
        //  lv_obj_set_style_pad_all(obj, 0, 0);
         lv_obj_set_grid_cell(obj, LV_GRID_ALIGN_STRETCH, col, 1,
                              LV_GRID_ALIGN_STRETCH, row, 1);

        //  label = lv_label_create(obj);
        //  lv_label_set_text_fmt(label, "c%d, r%d", col, row);
        //  lv_obj_center(label);
     }

    static lv_style_t style_grid;
    lv_style_init(&style_grid);
    lv_style_set_bg_color(&style_grid, lv_color_white());
    lv_style_set_bg_opa(&style_grid, LV_OPA_100);
    lv_style_set_border_width(&style_grid, 0);
    lv_style_set_border_color(&style_grid, lv_color_black());
    lv_obj_add_style(cont, &style_grid, 0);

 }

 static void pause_cb(lv_event_t * e)
 {
     lv_event_code_t code = lv_event_get_code(e);
     lv_obj_t * btn = lv_event_get_target(e);
     printf(e);
     if(e == &pause_address && code == LV_EVENT_CLICKED) {
        if (play) {
          play = false;
          // pause game
        }
        else {
          play = true;
        }
     }
 }

 void lv_buttons() {
    lv_obj_t * pause_btn = lv_btn_create(lv_scr_act());     /*Add a button the current screen*/
    pause_address = pause_btn;
    lv_obj_set_pos(pause_btn, 325, 0);                            /*Set its position*/
    lv_obj_set_size(pause_btn, 155, 80);                          /*Set its size*/
    lv_obj_add_event_cb(pause_btn, pause_cb, LV_EVENT_ALL, NULL);           /*Assign a callback to the button*/

    lv_obj_t * label = lv_label_create(pause_btn);          /*Add a label to the button*/
    if (play) {
      lv_label_set_text(label, "Play");                    /*Set the labels text*/
    }
    else {
      lv_label_set_text(label, "Pause");
    }
    lv_obj_center(label);
 }

int main(int argc, char **argv)
{
  (void)argc; /*Unused*/
  (void)argv; /*Unused*/

  /*Initialize LVGL*/
  lv_init();

  /*Initialize the HAL (display, input devices, tick) for LVGL*/
  hal_init(480, 320);

  lv_grid_1();
  lv_buttons();
  //lv_example_style_2();

  #if LV_USE_OS == LV_OS_NONE


  while(1) {
    /* Periodically call the lv_task handler.
     * It could be done in a timer interrupt or an OS task too.*/
    lv_timer_handler();
    usleep(5 * 1000);
  }

  #elif LV_USE_OS == LV_OS_FREERTOS

  /* Run FreeRTOS and create lvgl task */
  freertos_main();

  #endif

  return 0;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Initialize the Hardware Abstraction Layer (HAL) for the LVGL graphics
 * library
 */
static lv_display_t * hal_init(int32_t w, int32_t h)
{

  lv_group_set_default(lv_group_create());

  lv_display_t * disp = lv_sdl_window_create(w, h);

  lv_indev_t * mouse = lv_sdl_mouse_create();
  lv_indev_set_group(mouse, lv_group_get_default());
  lv_indev_set_display(mouse, disp);
  lv_display_set_default(disp);

  LV_IMAGE_DECLARE(mouse_cursor_icon); /*Declare the image file.*/
  lv_obj_t * cursor_obj;
  cursor_obj = lv_image_create(lv_screen_active()); /*Create an image object for the cursor */
  lv_image_set_src(cursor_obj, &mouse_cursor_icon);           /*Set the image source*/
  lv_indev_set_cursor(mouse, cursor_obj);             /*Connect the image  object to the driver*/

  lv_indev_t * mousewheel = lv_sdl_mousewheel_create();
  lv_indev_set_display(mousewheel, disp);
  lv_indev_set_group(mousewheel, lv_group_get_default());

  lv_indev_t * kb = lv_sdl_keyboard_create();
  lv_indev_set_display(kb, disp);
  lv_indev_set_group(kb, lv_group_get_default());

  return disp;
}
