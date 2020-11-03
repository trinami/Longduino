#ifndef AUDIO_USBD_CONF_H
#define AUDIO_USBD_CONF_H

/* USB feature -- Self Powered */
//#define USBD_SELF_POWERED

/* USB user string supported */
/* #define USB_SUPPORT_USER_STRING_DESC */

//#define USBD_DYNAMIC_DESCRIPTOR_CHANGE_ENABLED

 #define USBD_AUDIO_FREQ                16000  /* Audio frequency in Hz for GD32VF103 devices family when 25MHz HSE value
                                                  is used. */

/* Maximum number of supported media (Flash) */
#define MAX_USED_MEMORY_MEDIA        1

#define USB_STRING_COUNT             6

/* Audio endpoint define */
#define AUDIO_TOTAL_IF_NUM              0x02
#define AUDIO_OUT_EP                    EP1_OUT
#define AUDIO_IN_EP                     EP2_IN

#define USB_SERIAL_STRING_SIZE        0x06
#define DEVICE_ID                     (0x40022100)
#define DEFAULT_VOLUME                  65    /* Default volume in % (Mute=0%, Max = 100%) in Logarithmic values.
                                                 To get accurate volume variations, it is possible to use a logarithmic
                                                 coversion table to convert from percentage to logarithmic law.
                                                 In order to keep this example code simple, this conversion is not used.*/
#endif /* AUDIO_USBD_CONF_H */
