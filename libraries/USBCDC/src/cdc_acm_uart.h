#ifndef CDC_ACM_UART_H
#define CDC_ACM_UART_H

#ifdef __cplusplus
extern "C" {
#endif

// ==== Conditional compile ====
#ifdef LCD_SS_PIN
#define USE_DISPLAY                             1
#endif
#define USE_LEDS                                1
#define UART_USE_BREAK_TO_CHANGE                1

// ==== Constants ====
#define UART_RXBUFFER_SIZE                      4096
#define UART_DEFAULT_BDR                        115200

#define UART0_TXPIN                             GPIO_PIN_9
#define UART0_RXPIN                             GPIO_PIN_10
#define UART0_RTSPIN                            GPIO_PIN_13
#define UART0_DTRPIN                            GPIO_PIN_14
#define UART0_PORT                              GPIOA
#define UART0_RTS_DTR_MODE                      GPIO_MODE_AF_OD

#define UART2_TXPIN                             GPIO_PIN_10
#define UART2_RXPIN                             GPIO_PIN_11
#define UART2_RTSPIN                            GPIO_PIN_13
#define UART2_DTRPIN                            GPIO_PIN_14
#define UART2_PORT                              GPIOB
#define UART2_RTS_DTR_MODE                      GPIO_MODE_AF_PP

extern __IO uint32_t uart_receive_count;
extern __IO uint32_t uart_send_count;
extern __IO uint32_t uart_error_count;
extern __IO uint32_t active_uart;
extern __IO uint32_t uart_change_req;

void lcd_showUartSettings(void);

#ifdef __cplusplus
}
#endif

#endif
