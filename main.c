#include "xparameters.h"
#include "xiicps.h"
#include "sleep.h"
#include <stdio.h>

#define IIC_DEVICE_ID       0
#define LCD_I2C_ADDR        0x27   // hoặc 0x3F tùy module của bạn

XIicPs Iic;

void I2C_LCD_SendByte(u8 data, u8 mode);
void LCD_Init(void);
void LCD_SendCommand(u8 cmd);
void LCD_SendData(u8 data);
void LCD_SetCursor(u8 row, u8 col);
void LCD_Print(char *str);

int main() {
    XIicPs_Config *IicConfig;
    int Status;

    // Khởi tạo I2C
    IicConfig = XIicPs_LookupConfig(IIC_DEVICE_ID);
    Status = XIicPs_CfgInitialize(&Iic, IicConfig, IicConfig->BaseAddress);
    if (Status != XST_SUCCESS) return XST_FAILURE;

    XIicPs_SetSClk(&Iic, 100000); // 100kHz

    LCD_Init();

    LCD_SetCursor(0, 0);
    LCD_Print(" ZYNQ + I2C LCD ");

    LCD_SetCursor(1, 0);
    LCD_Print(" Vo Nhat Truong ");

    //while (1);
    return 0;
}

// ======= LCD LOW-LEVEL FUNCTIONS =======
#define LCD_BACKLIGHT 0x08
#define ENABLE 0x04

void I2C_LCD_SendByte(u8 data, u8 mode) {
    u8 data_u, data_l;
    u8 data_t[4];
    data_u = (data & 0xF0);
    data_l = ((data << 4) & 0xF0);
    data_t[0] = data_u | mode | LCD_BACKLIGHT | ENABLE;
    data_t[1] = data_u | mode | LCD_BACKLIGHT;
    data_t[2] = data_l | mode | LCD_BACKLIGHT | ENABLE;
    data_t[3] = data_l | mode | LCD_BACKLIGHT;

    XIicPs_MasterSendPolled(&Iic, data_t, 4, LCD_I2C_ADDR);
    usleep(2000);
}

void LCD_SendCommand(u8 cmd) { I2C_LCD_SendByte(cmd, 0); }
void LCD_SendData(u8 data) { I2C_LCD_SendByte(data, 1); }

void LCD_Init(void) {
    usleep(50000);
    LCD_SendCommand(0x33);
    LCD_SendCommand(0x32);
    LCD_SendCommand(0x28);
    LCD_SendCommand(0x0C);
    LCD_SendCommand(0x06);
    LCD_SendCommand(0x01);
    usleep(5000);
}

void LCD_SetCursor(u8 row, u8 col) {
    u8 row_offsets[] = {0x00, 0x40};
    LCD_SendCommand(0x80 | (col + row_offsets[row]));
}

void LCD_Print(char *str) {
    while (*str) LCD_SendData(*str++);
}
