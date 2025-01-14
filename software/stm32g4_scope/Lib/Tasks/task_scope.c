/*
 * task_scope.c
 *
 *  Created on: Aug 31, 2023
 *      Author: jgpei
 */

#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

#include "tim.h"
#include "adc.h"
#include "dac.h"
#include "opamp.h"
#include "spi.h"

#include "lcd.h"
#include "scope.h"
#include "ui.h"

#include "scope_tasks.h"

#define ADC_BUFFER_LEN (512)

uint16_t buffer1[ADC_BUFFER_LEN];
uint16_t buffer2[ADC_BUFFER_LEN];
uint16_t buffer3[ADC_BUFFER_LEN];
uint16_t buffer4[ADC_BUFFER_LEN];
uint16_t buffer5[ADC_BUFFER_LEN];
uint16_t buffer6[ADC_BUFFER_LEN];
uint16_t buffer7[ADC_BUFFER_LEN];
uint16_t buffer8[ADC_BUFFER_LEN];

uint16_t buffer_tmp[ADC_BUFFER_LEN];

extern tUi ui;
extern tLcd lcd;


#include "framebuf.h"
#include "FontUbuntuBookRNormal16.h"

#define FB2_WIDTH    (240)
#define FB2_HEIGHT   (16)

uint16_t fb2_buf[FB2_WIDTH*FB2_HEIGHT];

/**
 * @brief Task to handle the oscilloscope functionality.
 *
 * This task manages the oscilloscope by initializing hardware components,
 * handling user commands, and continuously updating the display.
 *
 * Pseudocode:
 *
 * Initialize scope hardware components.
 * Configure initial scope settings.
 * Draw the initial scope display.
 *
 * While(1):
 *     Wait for a fixed time interval (xFrequency).
 *     Check for UI scope messages and update scope settings accordingly.
 *     If the oscilloscope is running:
 *         Acquire and display data.
 *         Update the display with grid and signal information.
 *         Check for trigger conditions.
 *         If the trigger conditions are met, mark the trigger point.
 *         Handle single-shot mode.
 *
@startuml
start
:Initialize scope hardware components;
:Configure initial scope settings;
:Draw the initial scope display;
while (true) is (true)
    :Wait for a fixed time interval (xFrequency);
    if (UI scope messages are available) then (yes)
        :Erase the previous scope display;
        :Update scope settings based on messages;
        :Redraw the scope display with updated settings;
    endif
    if (running) then (yes)
        :Acquire and display data;
        :Update the display with grid and signal information;
        :Handle single-shot mode;
    endif
endwhile
@enduml
 * @param argument Task argument.
 */
void StartTaskScope(void *argument) {
    static tScope scope = {0};
    pScope = &scope;

    struct sQueueUiScope msgScope = {0};

    TickType_t xLastWakeTime;
    const TickType_t xFrequency = 20;
    int wait_timeout_ms = 100;

    int running = 0;
    int single = 0;

    int is_collapsed = 0;
    int acquire_is_visible = 0;
    int horizontal_is_visible = 0;
    int vertical_is_visible = 0;
    int trigger_is_visible = 0;

    int slow_cnt = 0;

    tFramebuf fb2 = {0};

    framebuf_init( &fb2, FB2_WIDTH, FB2_HEIGHT, fb2_buf );

    // Initialize hardware components and buffers
    scope_init_ll(&scope,
        &htim2,  // horizontal.htim_clock
        &htim3,  // horizontal.htim_stop
        &hdac2,  // vertical.hdac
        &hopamp1, // vertical.hopamp1
        &hopamp3, // vertical.hopamp2
        &hopamp5, // vertical.hopamp3
        &hopamp6, // vertical.hopamp4
        &hadc1,  // trigger.hadc1
        &hadc3,  // trigger.hadc2
        &hadc5,  // trigger.hadc3
        &hadc4   // trigger.hadc4
    );

    scope_init(&scope,
        buffer1,
        buffer2,
        buffer3,
        buffer4,
        buffer5,
        buffer6,
        buffer7,
        buffer8,
        ADC_BUFFER_LEN
    );

    scope_config_horizontal(&scope,
    	0,		// offset
		100 	// scale [KHz]
	);
    scope_config_vertical(&scope,
		2048,	// offset
    	0, 		// scale1
		0, 		// scale2
		0, 		// scale3
		0 		// scale4
	);

    scope.vertical.offset1 = 192;
	scope.vertical.offset2 = 192;
	scope.vertical.offset3 = 192;
	scope.vertical.offset4 = 192;

	scope.vertical.enable1 = 1;
	scope.vertical.enable2 = 0;
	scope.vertical.enable3 = 0;
	scope.vertical.enable4 = 0;

	scope.vertical.scale1 = 80;
	scope.vertical.scale2 = 80;
	scope.vertical.scale3 = 80;
	scope.vertical.scale4 = 80;

    scope_config_trigger(&scope,
		0, 							// channel
		UI_TRIGGER_MODE_NORMAL, 	// mode
		3092, 						// level
		SCOPE_TRIGGER_SLOPE_RISING	// slope
	);

    // Draw the initial scope display
    if (osSemaphoreAcquire(semaphoreLcdHandle, portMAX_DELAY) == osOK) {
        scope_draw(&scope, &lcd,
            acquire_is_visible,
            horizontal_is_visible,
            vertical_is_visible,
            trigger_is_visible,
            is_collapsed);
        osSemaphoreRelease(semaphoreLcdHandle);
    }

    xLastWakeTime = xTaskGetTickCount();
    for (;;) {
        vTaskDelayUntil(&xLastWakeTime, xFrequency);

        // Check for UI scope messages and update scope settings
        if (osMessageQueueGet(queueUiScopeHandle, &msgScope, NULL, 0U) == osOK) {
            if (osSemaphoreAcquire(semaphoreLcdHandle, portMAX_DELAY) == osOK) {
                // Erase the previous scope display
                scope_erase(&scope, &lcd,
                    acquire_is_visible,
                    horizontal_is_visible,
                    vertical_is_visible,
                    trigger_is_visible,
                    is_collapsed);

                osSemaphoreRelease(semaphoreLcdHandle);
            }

            switch (msgScope.type) {
                case QUEUE_UI_SCOPE_TYPE_START:
                    running = 1;
                    single = !msgScope.data[0];
                    break;
                case QUEUE_UI_SCOPE_TYPE_STOP:
                    running = 0;
                    single = 0;
                    break;
                case QUEUE_UI_SCOPE_TYPE_HORIZONTAL:
                    scope_config_horizontal(&scope,
                        msgScope.data[0], // offset
                        msgScope.data[1]  // scale
                    );
                    break;
                case QUEUE_UI_SCOPE_TYPE_VERTICAL:
                    scope_config_vertical(&scope,
                        msgScope.data[0], // offset
                        msgScope.data[1], // scale1
                        msgScope.data[2], // scale2
                        msgScope.data[3], // scale3
                        msgScope.data[4] // scale4
                    );
                    scope.vertical.offset1 = msgScope.data[5];
                    scope.vertical.offset2 = msgScope.data[6];
                    scope.vertical.offset3 = msgScope.data[7];
                    scope.vertical.offset4 = msgScope.data[8];

                    scope.vertical.enable1 = msgScope.data[9];
                    scope.vertical.enable2 = msgScope.data[10];
                    scope.vertical.enable3 = msgScope.data[11];
                    scope.vertical.enable4 = msgScope.data[12];

                    scope.vertical.scale1 = msgScope.data[13];
                    scope.vertical.scale2 = msgScope.data[14];
                    scope.vertical.scale3 = msgScope.data[15];
                    scope.vertical.scale4 = msgScope.data[16];

                    if( scope.vertical.scale1 == 0 )
                    	scope.vertical.scale1 = 1;
                    if( scope.vertical.scale2 == 0 )
                    	scope.vertical.scale2 = 1;
                    if( scope.vertical.scale3 == 0 )
                    	scope.vertical.scale3 = 1;
                    if( scope.vertical.scale4 == 0 )
                    	scope.vertical.scale4 = 1;

                    break;
                case QUEUE_UI_SCOPE_TYPE_TRIGGER:
                    scope_config_trigger(&scope,
                        msgScope.data[0], // source
                        msgScope.data[1], // mode
                        msgScope.data[2], // level
                        msgScope.data[3]  // slope
                    );
                    break;
                case QUEUE_UI_SCOPE_TYPE_CHANGE_VISIBILITY:
                    if (msgScope.data[1] == 0)
                        acquire_is_visible = msgScope.data[0];
                    if (msgScope.data[1] == 1)
                        horizontal_is_visible = msgScope.data[0];
                    if (msgScope.data[1] == 2)
                        vertical_is_visible = msgScope.data[0];
                    if (msgScope.data[1] == 3)
                        trigger_is_visible = msgScope.data[0];
                    break;
                case QUEUE_UI_SCOPE_TYPE_CHANGE_COLLAPSED:
                    is_collapsed = msgScope.data[0];
                    break;
                default:
                    break;
            }

            if (osSemaphoreAcquire(semaphoreLcdHandle, portMAX_DELAY) == osOK) {
                // Redraw the scope display with updated settings
                scope_draw(&scope, &lcd,
                    acquire_is_visible,
                    horizontal_is_visible,
                    vertical_is_visible,
                    trigger_is_visible,
                    is_collapsed);
                osSemaphoreRelease(semaphoreLcdHandle);
            }
        }

        if (running) {
            // Compute the max timeout as 10 times the buffer*timer freq.
            //if( scope.trigger.mode == UI_TRIGGER_MODE_AUTO )//SCOPE_TRIGGER_MODE_NORMAL )
            {
				if( scope.horizontal.scale )
				{
					wait_timeout_ms = 10*(scope.len*1000.0f)/(scope.horizontal.scale*1000);
				}
				else
				{
					wait_timeout_ms = 1000;
				}
				if( wait_timeout_ms < 10 )
				{
					wait_timeout_ms = 10;
				}
            }
            //else
            {
            //	wait_timeout_ms = portMAX_DELAY;
            }
            scope_start(&scope, 0);
            scope_wait(&scope, single ? portMAX_DELAY : wait_timeout_ms);
            scope_stop(&scope);

            //if (slow_cnt == 0)
            {
                if (osSemaphoreAcquire(semaphoreLcdHandle, portMAX_DELAY) == osOK) {
                    // Redraw the scope grid (less frequently)
                    scope_draw_grid(&scope, &lcd, is_collapsed);
                    osSemaphoreRelease(semaphoreLcdHandle);
                }
            }

            if (osSemaphoreAcquire(semaphoreLcdHandle, portMAX_DELAY) == osOK) {
                if (scope.trigged) {
                    // Mark the trigger point if conditions are met
                    lcd_rect(&lcd, 10, 10, 4, 4, 0xFFFF);
                } else {
                    lcd_rect(&lcd, 10, 10, 4, 4, 0x0000);
                }
                // Redraw the scope signals
                scope_draw_signals(&scope, &lcd, is_collapsed);
                osSemaphoreRelease(semaphoreLcdHandle);
            }

            //if (slow_cnt == 0)
            {
                if (osSemaphoreAcquire(semaphoreLcdHandle, portMAX_DELAY) == osOK) {
                    // Redraw the scope display with updated data
                    scope_draw(&scope, &lcd,
                        acquire_is_visible,
                        horizontal_is_visible,
                        vertical_is_visible,
                        trigger_is_visible,
                        is_collapsed);
                    osSemaphoreRelease(semaphoreLcdHandle);
                }


            }

            int i = 0;
            int n = 0;

            int trigger = 0;
            trigger = (scope.len - scope.dma_cndtr) - scope.len/2;
            trigger = trigger % scope.len;

            uint16_t *pB = (scope.cnt & 0x01)?scope.buffer1:scope.buffer5;

            for( i = 0 ; i < scope.len ; i++ )
            {
            	n = trigger + i;
            	n = n % scope.len;
            	buffer_tmp[i] = pB[n];
            }

            uint8_t buffer[64] = {0};
            uint16_t vmin = 0;
            uint16_t vmax = 0;
            uint16_t vavg = 0;
            uint16_t period = 0;
            uint16_t duty = 0;

            uint16_t get_vavg( uint16_t *buffer, uint16_t len );
            uint16_t get_period( uint16_t *buffer, uint16_t len, uint16_t mx, uint16_t mn, uint16_t avg );
            uint16_t get_duty( uint16_t *buffer, uint16_t len, uint16_t mx, uint16_t mn, uint16_t avg );

            vmin = get_vmin( buffer_tmp, scope.len );
			vmax = get_vmax( buffer_tmp, scope.len );
			vavg = get_vavg( buffer_tmp, scope.len );
			period = get_period( buffer_tmp, scope.len, vmax, vmin, vavg );
			duty = get_duty( buffer_tmp, scope.len, vmax, vmin, vavg );


            framebuf_fill( &fb2, LCD_COLOR_BLACK );

            snprintf( buffer, sizeof(buffer), "%04X", vmin );
            framebuf_text( &fb2, &fontUbuntuBookRNormal16, 48*0, 0, buffer, LCD_COLOR_WHITE );
            snprintf( buffer, sizeof(buffer), "%04X", vmax );
            framebuf_text( &fb2, &fontUbuntuBookRNormal16, 48*1, 0, buffer, LCD_COLOR_WHITE );
            snprintf( buffer, sizeof(buffer), "%04X", vavg );
            framebuf_text( &fb2, &fontUbuntuBookRNormal16, 48*2, 0, buffer, LCD_COLOR_WHITE );
            snprintf( buffer, sizeof(buffer), "%04X", period );
			framebuf_text( &fb2, &fontUbuntuBookRNormal16, 48*3, 0, buffer, LCD_COLOR_WHITE );
			snprintf( buffer, sizeof(buffer), "%04X", duty );
			framebuf_text( &fb2, &fontUbuntuBookRNormal16, 48*4, 0, buffer, LCD_COLOR_WHITE );

            if( osSemaphoreAcquire( semaphoreLcdHandle, portMAX_DELAY ) == osOK )
            {
                lcd_bmp( &lcd, 0, lcd.height-2*fb2.height, fb2.width, fb2.height, fb2.buf);
                osSemaphoreRelease( semaphoreLcdHandle );
            }
            slow_cnt = (slow_cnt + 1) % 10;

            if (single) {
                running = 0;
            }
        }
    }
}
