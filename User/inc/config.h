#ifndef __CONFIG_H__
#define __CONFIG_H__


#define BOOT_VERSION "V01.1"
#define MCU_VERSION  "V002.00"


//#define BOARD_ALIENTEK
#define BOARD_KOBBLE

#define UPG_FLAG                       0xfeedface
#define UPG_FLAG_FLASH_OFFSET          0x3ff80       //256K(0x40000)-FLSH_PAGE_SIZE(0x800)
#define UPG_FLAG_SRAM_OFFSET           0xbe00        //max  0xc000

///////////////////////////////////////////////

/*      0~0x6000                    uboot
        //0x6000~0x8000             font
        0x8000~end                  app
        #define FONT_OFFSET         0x8000      */

//SYS Setting
#define APP_OFFSET              0x8000
#define APP_MAXLEN              (0x40000-APP_OFFSET)
#define XTAL_HZ                 12000000

////////////////////////////////////////////////
//LCD UI Setting
#define LCD_WIDTH               320
#define LCD_HEIGHT              240
#define ITEM_HEIGHT             32
#define ITEM_FOCUS_COLOR        MAGENTA
#define ITEM_FOCUS_BGCOLOR      GRAY
#define ITEM_EDIT_COLOR         RED

#define TOPBAR_RECT             {0,     0,      LCD_WIDTH,      20}
#define BODY_RECT               {0,     20,     LCD_WIDTH,      LCD_HEIGHT-20}
#define TITLE_RECT              {0,     30,     LCD_WIDTH,      30}
#define MENU_RECT               {80,    70,     LCD_WIDTH-160,  LCD_HEIGHT-100}
#define INPUTBOX_RECT           {0,     70,     LCD_WIDTH,      LCD_HEIGHT-100}
#define MSGBOX_RECT             {75,    60,     LCD_WIDTH-150,  LCD_HEIGHT-120}

#define LCD_FC                  WHITE   //0x0000
#define LCD_BC                  BLACK   //0xFFFF

/////////////////////////////////////////////////
#ifdef BOARD_ALIENTEK
    #define DBG_UART            UART_1
    
    #define DSP_UART            UART_4
    #define DSP_BAUDRATE        115200

    #define HDMI_UART           UART_5
    #define HDMI_BAUDRATE       115200

    #define RS485_UART          UART_1
    #define RS485_BAUDRATE      115200

    #define USE_I2C1
    #define E2P_I2C             I2C_1
    #define E2P_ADDR            0x50        //24LC512
    #define E2P_PAGE_SIZE       128
    #define E2P_I2C_SPEED       100000      //100k hz, max:400k

    #define KNOB_BAUDRATE       115200
    
    //LCD config
    #define LCD_SPI             SPI_1
    #define LCD_RS_GRP          GPIOC
    #define LCD_RS_PIN          GPIO_PIN_0
    #define LCD_CS_GRP          GPIOC
    #define LCD_CS_PIN          GPIO_PIN_2
    #define LCD_RST_GRP         GPIOC
    #define LCD_RST_PIN         GPIO_PIN_13
    #define LCD_PWR_GRP         GPIOC
    #define LCD_PWR_PIN         GPIO_PIN_3
     
#else
    #define DBG_UART            0

    #define KNOB_UART           UART_2
    #define KNOB_BAUDRATE       115200

    #define DSP_UART            UART_4
    #define DSP_BAUDRATE        115200

    #define HDMI_UART           UART_5
    #define HDMI_BAUDRATE       115200

    #define RS485_UART          UART_1
    #define RS485_BAUDRATE      115200
    
    #define E2P_I2C             I2C_1
    #define USE_24XX512         0x50        //24LC512
    #define E2P_PAGE_SIZE       128
    #define E2P_I2C_SPEED       100000      //100k hz, max:400k
    
    //LCD config
    #define LCD_RS_GRP          GPIOC
    #define LCD_RS_PIN          GPIO_Pin_4
    #define LCD_CS_GRP          GPIOC
    #define LCD_CS_PIN          GPIO_Pin_5  
    #define LCD_RST_GRP         GPIOB
    #define LCD_RST_PIN         GPIO_Pin_10
    #define LCD_PWR_GRP         GPIOA
    #define LCD_PWR_PIN         GPIO_Pin_5
    
    //#define LCD_SOFT_SPI
    #ifdef LCD_SOFT_SPI
        #define LCD_SCK_GRP     GPIOB
        #define LCD_SCK_PIN     GPIO_PIN_0
        #define LCD_SDA_GRP     GPIOB
        #define LCD_SDA_PIN     GPIO_PIN_1
    #else
        #define LCD_SPI         SPI_2
    #endif
    
    #define KA350
    //#define KA650i

#endif


#ifdef KA350
    #define MODEL_NAME         "KA350"
#endif

#ifdef KA650i
    #define USE_HDMI
    #define USE_RS485
    #define MODEL_NAME        "KA650i"
#endif









#endif
