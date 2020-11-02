#include <stdio.h>
#include "cdc_acm_uart.h"
#include "lcd.h"

#if USE_DISPLAY
static char tmpbuf[21];
static uint8_t lcd_stat = 0;
#endif

#if UART_USE_BREAK_TO_CHANGE
static uint64_t break_time = 0;
#endif

__IO uint32_t uart_receive_count = 0;
__IO uint32_t uart_send_count = 0;
__IO uint32_t uart_error_count = 0;
__IO uint32_t active_uart = 0;
__IO uint32_t uart_change_req = 0;

//-------------------------
void lcd_showUartSettings(void)
{
    #if USE_DISPLAY
    LCD_ShowStr(0, 16, (u8 *)("\r"), CYAN, OPAQUE);
    LCD_ShowStr(0, 64, (u8 *)("\r"), CYAN, OPAQUE);
    sprintf(tmpbuf, "DTR=%d RTS=%d\r", cdc_acm_dtr, cdc_acm_rts);
    LCD_ShowStr(0, 48, (u8 *)tmpbuf, CYAN, OPAQUE);
    sprintf(tmpbuf, "%s", (cdc_acm_brk) ? "BREAK" : "     ");
    LCD_ShowStr(120, 48, (u8 *)tmpbuf, CYAN, OPAQUE);

    char parity = 'N';
    if (linecoding.bParityType == 2) parity = 'E';
    else if (linecoding.bParityType == 3) parity = 'O';
    char stbits[4] = "1";
    if (linecoding.bCharFormat == 1) sprintf(stbits, "0.5");
    else if (linecoding.bCharFormat == 2) sprintf(stbits, "2");
    else if (linecoding.bCharFormat == 3) sprintf(stbits, "1.5");
    char actu = '?';
    if (active_uart == USART0) actu = '0';
    else if (active_uart == USART2) actu = '1';
    sprintf(tmpbuf, "UART%c: %lu %u%c%s\r", actu, linecoding.dwDTERate, linecoding.bDataBits, parity, stbits);
    LCD_ShowStr(0, 32, (u8 *)tmpbuf, YELLOW, OPAQUE);
    #endif
}

//------------------------------
static void lcd_showStatus(void)
{
    #if USE_DISPLAY
    char actu = '?';
    if (active_uart == USART0) actu = '0';
    else if (active_uart == USART2) actu = '1';
    else {
        LCD_ShowStr(0, 16, (u8 *)("\n"), BLACK, OPAQUE);
        return;
    }
    sprintf(tmpbuf, "Active UART: %c\r", actu);
    LCD_ShowStr(0, 16, (u8 *)tmpbuf, GREEN, OPAQUE);
    sprintf(tmpbuf, "S> %lu\r", uart_send_count);
    LCD_ShowStr(0, 32, (u8 *)tmpbuf, CYAN, OPAQUE);
    sprintf(tmpbuf, "S< %lu\r", uart_receive_count);
    LCD_ShowStr(0, 48, (u8 *)tmpbuf, CYAN, OPAQUE);
    uint8_t pos = strlen(tmpbuf) * 8;
    sprintf(tmpbuf, " %lu\r", uart_error_count);
    LCD_ShowStr(pos, 48, (u8 *)tmpbuf, RED, OPAQUE);
    sprintf(tmpbuf, "U  %lu %lu\r", cdc_acm_send_count, cdc_acm_receive_count);
    LCD_ShowStr(0, 64, (u8 *)tmpbuf, YELLOW, OPAQUE);
    lcd_stat = 1;
    #endif
}

void cdc_acm_set_control_line_state(uint8_t rts, uint8_t dtr)
{
    if (active_uart == USART0) {
        (rts) ? gpio_bit_set(GPIOA, UART0_RTSPIN) : gpio_bit_reset(GPIOA, UART0_RTSPIN);
        (dtr) ? gpio_bit_set(GPIOA, UART0_DTRPIN) : gpio_bit_reset(GPIOA, UART0_DTRPIN);
    }
    else if (active_uart == USART2) {
        (rts) ? gpio_bit_set(GPIOA, UART2_RTSPIN) : gpio_bit_reset(GPIOA, UART2_RTSPIN);
        (dtr) ? gpio_bit_set(GPIOA, UART2_DTRPIN) : gpio_bit_reset(GPIOA, UART2_DTRPIN);
    }
    lcd_showUartSettings();
}

uint8_t cdc_acm_send_break(uint8_t brk)
{
    uint8_t ret = 0;

    #if UART_USE_BREAK_TO_CHANGE
    if ((get_time() - break_time) > 2000) {
        if (active_uart == USART0) uart_change_req = 2;
        else uart_change_req = 1;
        break_time = get_time();
    }
    #else
    if ((active_uart == USART0) || (active_uart == USART2)) {
        usart_send_break(active_uart);
        ret = brk;
        lcd_showUartSettings();
    }
    #endif

    return ret;
}

void cdc_acm_set_line_coding(line_coding_struct* linecoding)
{
    if ((active_uart == USART0) || (active_uart == USART2)) {
        usart_baudrate_set(active_uart, linecoding->dwDTERate);
        usart_word_length_set(active_uart, linecoding->bDataBits);
        usart_stop_bit_set(active_uart, linecoding->bCharFormat);
        usart_parity_config(active_uart, linecoding->bParityType);
    }
    lcd_showUartSettings();
}

uint8_t cdc_acm_custom_req_handler(usb_dev *pudev, usb_req *req)
{
    switch (req->bRequest)
    {
#if ENABLE_CUSTOM_REQUESTS
        case SHOW_STATUS:
            lcd_showStatus();
            if (req->wValue & 1) {
                uart_send_count = 0;
                uart_receive_count = 0;
                uart_error_count = 0;
                cdc_acm_send_count = 0;
                cdc_acm_receive_count = 0;
            }
            break;
        case SELECT_UART:
            if (req->wValue == 0) uart_change_req = 1;
            else if (req->wValue == 1) uart_change_req = 2;
            break;
        case GET_STATUS:
            cdc_acm_u8tocmdbuf(0, (active_uart == USART0) ? 0 : 1);
            cdc_acm_u8tocmdbuf(1, cdc_acm_dtr | (cdc_acm_rts << 1));
            cdc_acm_u32tocmdbuf(2, cdc_acm_send_count);
            cdc_acm_u32tocmdbuf(6, cdc_acm_receive_count);
            cdc_acm_u32tocmdbuf(10, uart_send_count);
            cdc_acm_u32tocmdbuf(14, uart_receive_count);
            cdc_acm_u32tocmdbuf(18, uart_error_count);
            /* send the request data to the host */
            pudev->dev.transc_in[0].xfer_buf = cdc_acm_get_cmdbuf();
            pudev->dev.transc_in[0].remain_len = req->wLength;
            break;
#endif
        default:
            break;
    }

    return USBD_OK;
}
