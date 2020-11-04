#include "lcd.h"
#include "tf_card.h"
#include <string.h>

uint8_t mount_is_ok = 1; /* 0: mount successful ; 1: mount failed */
unsigned char image[12800];
FATFS fs;

void setup()
{
    FRESULT fr;     /* FatFs return code */

    /* init RGB LED */
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOC);
    gpio_init(GPIOC, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_13);
    gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_1|GPIO_PIN_2);

    Lcd_Init();			// init OLED
    LCD_Clear(WHITE);
    BACK_COLOR=WHITE;

    LEDR(1);
    LEDG(1);
    LEDB(1);

    fr = f_mount(&fs, "", 1);
    if (fr == 0)
        mount_is_ok = 0;
    else
        mount_is_ok = 1;

    if (mount_is_ok != 0)
    {
        LCD_ShowString(24,  0, (u8 *)("no card found!"), BLACK);
        LCD_ShowString(24, 16, (u8 *)("no card found!"), BLUE);
        LCD_ShowString(24, 32, (u8 *)("no card found!"), BRED);
        LCD_ShowString(24, 48, (u8 *)("no card found!"), GBLUE);
        LCD_ShowString(24, 64, (u8 *)("no card found!"), RED);
    }
}

void loop()
{
    int offset = 0;
    FIL fil;
    FRESULT fr;     /* FatFs return code */
    UINT br;

    if (mount_is_ok == 0)
    {
            offset = 0;
            fr = f_open(&fil, "logo.bin", FA_READ);
            if (fr) printf("open error: %d!\n\r", (int)fr);
            f_lseek(&fil, offset);
            fr = f_read(&fil, image, sizeof(image), &br);
            LCD_ShowPicture(0,0,159,39);
            offset += 12800;
            LEDB_TOG;
            f_lseek(&fil, offset);
            fr = f_read(&fil, image, sizeof(image), &br);
            LCD_ShowPicture(0,40,159,79);
            LEDB_TOG;
            delay_1ms(1500);
            f_close(&fil);

            fr = f_open(&fil, "bmp.bin", FA_READ);
            if (fr) printf("open error: %d!\n\r", (int)fr);
            offset = 0;

            for (int i=0; i<2189;i++)
            {
                fr = f_read(&fil, image, sizeof(image), &br);
                LCD_ShowPicture(0,0,159,39);
                offset += 12800;
                f_lseek(&fil, offset);
                LEDB_TOG;
                fr = f_read(&fil, image, sizeof(image), &br);
                LCD_ShowPicture(0,40,159,79);
                offset += 12800;
                f_lseek(&fil, offset);
                LEDB_TOG;
            }

            /* Close the file */
            f_close(&fil);
    }
    else
    {
            LEDR_TOG;
            delay_1ms(200);
            LEDG_TOG;
            delay_1ms(200);
            LEDB_TOG;
            delay_1ms(200);
    }
}

int _put_char(int ch)
{
    usart_data_transmit(USART0, (uint8_t) ch );
    while ( usart_flag_get(USART0, USART_FLAG_TBE)== RESET){
    }

    return ch;
}
