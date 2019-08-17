#ifndef _BOOTLOADER_H_
#define _BOOTLOADER_H_

    /* Includes: */
        #include <avr/wdt.h>
        #include <avr/io.h>
        #include <util/delay.h>

        #include <LUFA/Common/Common.h>
        #include <LUFA/Drivers/USB/USB.h>

    /* Macros: */
        #define MAGIC_BOOT_KEY              0xD351DA80
        #define FLASH_SIZE_BYTES            0x8000
        #define BOOTSEC_SIZE_BYTES          0x1000
        #define BOOTLOADER_START_ADDRESS    ((FLASH_SIZE_BYTES - BOOTSEC_SIZE_BYTES) >> 1)


    /* Function Prototypes: */
        void Bootloader_Jump_Check(void) ATTR_INIT_SECTION(3);

        /**
         * Jump to the programmed bootloader of the chip.
         */
        void Jump_To_Bootloader(void);

#endif /* _BOOTLOADER_H_ */