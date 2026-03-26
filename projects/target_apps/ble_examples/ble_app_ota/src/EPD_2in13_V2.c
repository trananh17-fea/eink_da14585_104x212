/*****************************************************************************
* | File      	:   EPD_2in13_V2.c
* | Author      :   Waveshare team
* | Function    :   2.13inch e-paper V2
* | Info        :
*----------------
* |	This version:   V3.0
* | Date        :   2019-06-13
* | Info        :
* -----------------------------------------------------------------------------
* V3.0(2019-06-13):
* 1. Thay đổi tên hàm:
*    EPD_Reset() => EPD_2IN13_V2_Reset()
*    EPD_SendCommand() => EPD_2IN13_V2_SendCommand()
*    EPD_SendData() => EPD_2IN13_V2_SendData()
*    EPD_WaitUntilIdle() => EPD_2IN13_V2_ReadBusy()
*    EPD_Init() => EPD_2IN13_V2_Init()
*    EPD_Clear() => EPD_2IN13_V2_Clear()
*    EPD_Display() => EPD_2IN13_V2_Display()
*    EPD_Sleep() => EPD_2IN13_V2_Sleep()
* 2. Thêm hàm:
*    EPD_2IN13_V2_DisplayPartBaseImage()
* -----------------------------------------------------------------------------
* V2.0(2018-11-14):
* 1. Loại bỏ: ImageBuff[EPD_HEIGHT * EPD_WIDTH / 8]
* 2. Thay đổi: EPD_2IN13_V2_Display(UBYTE *Image)
*    Cần truyền tham số: con trỏ đến dữ liệu cache
* 3. Thay đổi hằng số chân:
*    EPD_RST -> EPD_RST_PIN
*    EPD_DC -> EPD_DC_PIN
*    EPD_CS -> EPD_CS_PIN
*    EPD_BUSY -> EPD_BUSY_PIN
#
******************************************************************************/
#include "EPD_2in13_V2.h"
#include "Debug.h"

const unsigned char EPD_2IN13_V2_lut_full_update[] = {
        0x80, 0x60, 0x40, 0x00, 0x00, 0x00, 0x00,             //LUT0: BB:     VS 0 ~7
        0x10, 0x60, 0x20, 0x00, 0x00, 0x00, 0x00,             //LUT1: BW:     VS 0 ~7
        0x80, 0x60, 0x40, 0x00, 0x00, 0x00, 0x00,             //LUT2: WB:     VS 0 ~7
        0x10, 0x60, 0x20, 0x00, 0x00, 0x00, 0x00,             //LUT3: WW:     VS 0 ~7
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,             //LUT4: VCOM:   VS 0 ~7

        0x03, 0x03, 0x00, 0x00, 0x02,                       // TP0 A~D RP0
        0x09, 0x09, 0x00, 0x00, 0x02,                       // TP1 A~D RP1
        0x03, 0x03, 0x00, 0x00, 0x02,                       // TP2 A~D RP2
        0x00, 0x00, 0x00, 0x00, 0x00,                       // TP3 A~D RP3
        0x00, 0x00, 0x00, 0x00, 0x00,                       // TP4 A~D RP4
        0x00, 0x00, 0x00, 0x00, 0x00,                       // TP5 A~D RP5
        0x00, 0x00, 0x00, 0x00, 0x00,                       // TP6 A~D RP6

        0x15, 0x41, 0xA8, 0x32, 0x30, 0x0A,
};

const unsigned char EPD_2IN13_V2_lut_partial_update[] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,             //LUT0: BB:     VS 0 ~7
        0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,             //LUT1: BW:     VS 0 ~7
        0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,             //LUT2: WB:     VS 0 ~7
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,             //LUT3: WW:     VS 0 ~7
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,             //LUT4: VCOM:   VS 0 ~7

        0x0A, 0x00, 0x00, 0x00, 0x00,                       // TP0 A~D RP0
        0x00, 0x00, 0x00, 0x00, 0x00,                       // TP1 A~D RP1
        0x00, 0x00, 0x00, 0x00, 0x00,                       // TP2 A~D RP2
        0x00, 0x00, 0x00, 0x00, 0x00,                       // TP3 A~D RP3
        0x00, 0x00, 0x00, 0x00, 0x00,                       // TP4 A~D RP4
        0x00, 0x00, 0x00, 0x00, 0x00,                       // TP5 A~D RP5
        0x00, 0x00, 0x00, 0x00, 0x00,                       // TP6 A~D RP6

        0x15, 0x41, 0xA8, 0x32, 0x30, 0x0A,
};

void EPD_GPIO_init(void){
    //EPD_VCC ENABLE
    GPIO_ConfigurePin(GPIO_PORT_2, GPIO_PIN_3, OUTPUT, PID_GPIO, true);
    // GPIO_SetActive(GPIO_PORT_2, GPIO_PIN_3);
    // DEV_Delay_ms(10);
    GPIO_ConfigurePin(GPIO_PORT_2, GPIO_PIN_0, INPUT, PID_GPIO, false);
    GPIO_ConfigurePin(GPIO_PORT_2, GPIO_PIN_1, OUTPUT, PID_GPIO, true);
    GPIO_ConfigurePin(GPIO_PORT_0, GPIO_PIN_7, OUTPUT, PID_GPIO, true);
    GPIO_ConfigurePin(GPIO_PORT_0, GPIO_PIN_5, OUTPUT, PID_GPIO, false);
    // GPIO_ConfigurePin(GPIO_PORT_0, GPIO_PIN_3, OUTPUT, PID_GPIO, true);
    // GPIO_ConfigurePin(GPIO_PORT_2, GPIO_PIN_0, OUTPUT, PID_GPIO, false);
  
}

void DEV_SPI_WriteByte(uint8_t sdbyte)
{
    spi_access(sdbyte);
}

/******************************************************************************
chức năng :	Reset phần mềm
tham số:
******************************************************************************/
static void EPD_2IN13_V2_Reset(void)
{
    EPD_RST_L;
    DEV_Delay_ms(2);
    EPD_RST_H;
    DEV_Delay_ms(5);
}

/******************************************************************************
chức năng :	Gửi lệnh
tham số:
     Reg : Thanh ghi lệnh
******************************************************************************/
static void EPD_2IN13_V2_SendCommand(UBYTE Reg)
{
    EPD_DC_L;
    EPD_CS_L;
    DEV_SPI_WriteByte(Reg);
    EPD_CS_H;
}

/******************************************************************************
chức năng :	Gửi dữ liệu
tham số:
    Data : Dữ liệu ghi
******************************************************************************/
static void EPD_2IN13_V2_SendData(UBYTE Data)
{
    EPD_DC_H;
    EPD_CS_L;
    DEV_SPI_WriteByte(Data);
    EPD_CS_H;
}

/******************************************************************************
chức năng :	Chờ cho đến khi chân busy xuống mức THẤP
tham số:
******************************************************************************/
void EPD_2IN13_V2_ReadBusy(void)
{
    Debug("e-Paper busy\r\n");
    while (EPD_BUSY !=0)
    { //Mức THẤP: rảnh, Mức CAO: bận
      
    }
    Debug("e-Paper busy release\r\n");
}

/******************************************************************************
chức năng :	Bật hiển thị
tham số:
******************************************************************************/
 void EPD_2IN13_V2_TurnOnDisplay(void)
{
    EPD_2IN13_V2_SendCommand(0x22);
    EPD_2IN13_V2_SendData(0xC4);
    EPD_2IN13_V2_SendCommand(0x20);
    // EPD_2IN13_V2_ReadBusy();
}

 void EPD_2IN13_V2_TurnOnDisplay_RED(void)
{
    EPD_2IN13_V2_SendCommand(0x22);
    EPD_2IN13_V2_SendData(0xF7);
    EPD_2IN13_V2_SendCommand(0x20);
    EPD_2IN13_V2_ReadBusy();
}
/******************************************************************************
chức năng :	Bật hiển thị
tham số:
******************************************************************************/
 void EPD_2IN13_V2_TurnOnDisplayPart(void)
{
    EPD_2IN13_V2_SendCommand(0x22);
    EPD_2IN13_V2_SendData(0x04);
    EPD_2IN13_V2_SendCommand(0x20);
    // EPD_2IN13_V2_ReadBusy();
}



/******************************************************************************
chức năng :	Khởi tạo các thanh ghi e-Paper
tham số:
******************************************************************************/
void EPD_2IN13_V2_Init(UBYTE Mode)
{
     UBYTE count;
    EPD_2IN13_V2_Reset();

    if (Mode == EPD_2IN13_V2_FULL)
    {
        EPD_2IN13_V2_ReadBusy();
        EPD_2IN13_V2_SendCommand(0x12); // reset phần mềm
        EPD_2IN13_V2_ReadBusy();

        EPD_2IN13_V2_SendCommand(0x74); //thiết lập điều khiển khối analog
        EPD_2IN13_V2_SendData(0x54);
        EPD_2IN13_V2_SendCommand(0x7E); //thiết lập điều khiển khối kỹ thuật số
        EPD_2IN13_V2_SendData(0x3B);

        EPD_2IN13_V2_SendCommand(0x01); //Điều khiển đầu ra Driver
        EPD_2IN13_V2_SendData((EPD_2IN13_V2_HEIGHT-1)&0xFF);    //0xF9-->(249+1)=250
        EPD_2IN13_V2_SendData(((EPD_2IN13_V2_HEIGHT-1)&0xFF00)>>8);
        EPD_2IN13_V2_SendData(0x00);
        EPD_2IN13_V2_SendData(0x00);


        EPD_2IN13_V2_SendCommand(0x11); //chế độ nhập dữ liệu
        EPD_2IN13_V2_SendData(0x01);

        EPD_2IN13_V2_SendCommand(0x44); //thiết lập vị trí bắt đầu/kết thúc địa chỉ Ram-X
        EPD_2IN13_V2_SendData(0x00);
        EPD_2IN13_V2_SendData(EPD_2IN13_V2_WIDTH/8-1); //0x0C-->(15+1)*8=128

        EPD_2IN13_V2_SendCommand(0x45); //thiết lập vị trí bắt đầu/kết thúc địa chỉ Ram-Y
        EPD_2IN13_V2_SendData((EPD_2IN13_V2_HEIGHT-1)&0xFF);    //0xF9-->(249+1)=250
        EPD_2IN13_V2_SendData(((EPD_2IN13_V2_HEIGHT-1)&0xFF00)>>8);
        EPD_2IN13_V2_SendData(0x00);
        EPD_2IN13_V2_SendData(0x00);

  

        EPD_2IN13_V2_SendCommand(0x3C); //BorderWavefrom
        EPD_2IN13_V2_SendData(0x01);

        EPD_2IN13_V2_SendCommand(0x2C); //Điện áp VCOM
        EPD_2IN13_V2_SendData(0x50);    //

        EPD_2IN13_V2_SendCommand(0x03);
        EPD_2IN13_V2_SendData(EPD_2IN13_V2_lut_full_update[70]);

        EPD_2IN13_V2_SendCommand(0x04); //
        EPD_2IN13_V2_SendData(EPD_2IN13_V2_lut_full_update[71]);
        EPD_2IN13_V2_SendData(EPD_2IN13_V2_lut_full_update[72]);
        EPD_2IN13_V2_SendData(EPD_2IN13_V2_lut_full_update[73]);

        EPD_2IN13_V2_SendCommand(0x3A); //Dummy Line
        EPD_2IN13_V2_SendData(EPD_2IN13_V2_lut_full_update[74]);
        EPD_2IN13_V2_SendCommand(0x3B); //Gate time
        EPD_2IN13_V2_SendData(EPD_2IN13_V2_lut_full_update[75]);
        EPD_2IN13_V2_SendCommand(0x32);
        for (count = 0; count < 70; count++)
        {
            EPD_2IN13_V2_SendData(EPD_2IN13_V2_lut_full_update[count]);
        }

        EPD_2IN13_V2_ReadBusy();
        EPD_2IN13_V2_SendCommand(0x4E); // set RAM x address count to 0;
        EPD_2IN13_V2_SendData(0x00);
        EPD_2IN13_V2_SendCommand(0x4F); // set RAM y address count to 0X127;
        EPD_2IN13_V2_SendData((EPD_2IN13_V2_HEIGHT-1)&0xFF);    //0xF9-->(249+1)=250
        EPD_2IN13_V2_SendData(((EPD_2IN13_V2_HEIGHT-1)&0xFF00)>>8);
        EPD_2IN13_V2_SendData(EPD_2IN13_V2_HEIGHT-1);
        EPD_2IN13_V2_SendData(0x00);
       
    }
    else if (Mode == EPD_2IN13_V2_PART)
    {
        EPD_2IN13_V2_SendCommand(0x2C); //Điện áp VCOM
        EPD_2IN13_V2_SendData(0x28);

        EPD_2IN13_V2_ReadBusy();

        EPD_2IN13_V2_SendCommand(0x32);
        for (count = 0; count < 70; count++)
        {
            EPD_2IN13_V2_SendData(EPD_2IN13_V2_lut_partial_update[count]);
        }

        EPD_2IN13_V2_SendCommand(0x37);
        EPD_2IN13_V2_SendData(0x00);
        EPD_2IN13_V2_SendData(0x00);
        EPD_2IN13_V2_SendData(0x00);
        EPD_2IN13_V2_SendData(0x00);
        EPD_2IN13_V2_SendData(0x40);
        EPD_2IN13_V2_SendData(0x00);
        EPD_2IN13_V2_SendData(0x00);

        EPD_2IN13_V2_SendCommand(0x22);
        EPD_2IN13_V2_SendData(0xC0);

        EPD_2IN13_V2_SendCommand(0x20);
        EPD_2IN13_V2_ReadBusy();

        // EPD_2IN13_V2_SendCommand(0x3C); //BorderWavefrom
        // EPD_2IN13_V2_SendData(0x01);
    }
    else
    {
        Debug("lỗi, Chế độ phải là EPD_2IN13_FULL hoặc EPD_2IN13_PART");
    }
}

/**
 * @brief  Thiết lập vị trí và kích thước cửa sổ hiển thị EPD.
 * @param  x Vị trí X bắt đầu của cửa sổ hiển thị.
 * @param  y_x8 Vị trí Y bắt đầu của cửa sổ hiển thị, đặt 1 tương ứng với 8 pixel.
 * @param  x_size Kích thước theo phương X của cửa sổ hiển thị.
 * @param  y_size_x8 Kích thước theo phương Y của cửa sổ hiển thị, đặt 1 tương ứng với 8 pixel.
 * @note   Con trỏ đã được tự động đặt tại góc trên bên trái của cửa sổ.
 */
void EPD_SetWindow(uint16_t x, uint8_t y_x8, uint16_t x_size, uint8_t y_size_x8) {
   
    x = EPD_2IN13_V2_HEIGHT - 1 - x;
    x_size = x - x_size + 1;          
    y_size_x8 = y_size_x8 + y_x8 - 1;

    EPD_2IN13_V2_SendCommand(0x44);
    EPD_2IN13_V2_SendData(y_x8);
    EPD_2IN13_V2_SendData(y_size_x8 & 0xFF);
    EPD_2IN13_V2_SendCommand(0x45);
    EPD_2IN13_V2_SendData(x & 0xFF);
    EPD_2IN13_V2_SendData(0);
    EPD_2IN13_V2_SendData(x_size & 0xFF);
    EPD_2IN13_V2_SendData(0);

    EPD_2IN13_V2_SendCommand(0x4E);
    EPD_2IN13_V2_SendData(y_x8);
    EPD_2IN13_V2_SendCommand(0x4F);
    EPD_2IN13_V2_SendData(x & 0xFF);
    EPD_2IN13_V2_SendData(0);
}
/**
 * @brief  Gửi dữ liệu hiển thị với kích thước chỉ định tới bộ điều khiển EPD.
 * @param  data Con trỏ đến dữ liệu cần gửi.
 * @param  data_size Kích thước dữ liệu cần gửi.
 */
void EPD_SendRAM(const uint8_t *data, uint16_t data_size)
{
    uint16_t i;
    EPD_2IN13_V2_SendCommand(0x24);
    for(i=0;i<data_size;i++){
        EPD_2IN13_V2_SendData(*(data+i));
    }
}

void EPD_SendOLDRAM(const uint8_t *data, uint16_t data_size)
{
    uint16_t i;
    EPD_2IN13_V2_SendCommand(0x26);
    for(i=0;i<data_size;i++){
        EPD_2IN13_V2_SendData(*(data+i));
    }
}
/**
 * @brief  Xóa vùng hiển thị RAM chỉ định của bộ điều khiển EPD.
 * @note   Sau khi thực hiện xong cửa sổ sẽ khôi phục về toàn màn hình.
 */
void EPD_ClearArea(uint16_t x, uint8_t y_x8, uint16_t x_size, uint8_t y_size_x8, uint8_t color)
{
    uint16_t i, data_size;

    data_size = x_size * (y_size_x8>>3);
    EPD_SetWindow(x, y_x8, x_size, y_size_x8>>3);
    EPD_2IN13_V2_SendCommand(0x24);
    for (i = 0; i < data_size; i++)
    {
        EPD_2IN13_V2_SendData(color);
    }
    EPD_SetWindow(0, 0, EPD_2IN13_V2_HEIGHT, EPD_2IN13_V2_WIDTH>>3);
}


void EPD_DrawImage( const uint8_t *image, uint16_t x, uint16_t y, uint16_t w, uint16_t h ,bool part)
{
    uint16_t i = 0;
    uint16_t size = w * h / 8;
    EPD_SetWindow(x, y, w, h / 8);
    EPD_2IN13_V2_SendCommand(0x24);
    for (i = 0; i < size; i++)
    {
        EPD_2IN13_V2_SendData(image[i]);
    }
    if (part)
    {
        EPD_2IN13_V2_SendCommand(0x26);
        for (i = 0; i < size; i++)
        {
            EPD_2IN13_V2_SendData(image[i]);
        }
    }
}


/******************************************************************************
chức năng :	Xóa màn hình
tham số:
******************************************************************************/
void EPD_2IN13_V2_Clear(void)
{
    UWORD Width, Height;
    Width = (EPD_2IN13_V2_WIDTH % 8 == 0) ? (EPD_2IN13_V2_WIDTH / 8) : (EPD_2IN13_V2_WIDTH / 8 + 1);
    Height = EPD_2IN13_V2_HEIGHT;

    EPD_2IN13_V2_SendCommand(0x24);
    for (UWORD j = 0; j < Height; j++)
    {
        for (UWORD i = 0; i < Width; i++)
        {
            EPD_2IN13_V2_SendData(0xff);
        }
    }
     EPD_2IN13_V2_SendCommand(0x26);
    for (UWORD j = 0; j < Height; j++)
    {
        for (UWORD i = 0; i < Width; i++)
        {
            EPD_2IN13_V2_SendData(0xff);
        }
    }

    EPD_2IN13_V2_TurnOnDisplay();
}


/******************************************************************************
chức năng :	Gửi bộ đệm hình ảnh trong RAM đến e-Paper và hiển thị
tham số:
******************************************************************************/
void EPD_2IN13_V2_Display(UBYTE *Image)
{
    UWORD Width, Height;
    Width = (EPD_2IN13_V2_WIDTH % 8 == 0) ? (EPD_2IN13_V2_WIDTH / 8) : (EPD_2IN13_V2_WIDTH / 8 + 1);
    Height = EPD_2IN13_V2_HEIGHT;

    EPD_2IN13_V2_SendCommand(0x24);
    for (UWORD j = 0; j < Height; j++)
    {
        for (UWORD i = 0; i < Width; i++)
        {
            EPD_2IN13_V2_SendData(Image[i + j * Width]);
        }
    }
    EPD_2IN13_V2_TurnOnDisplay();
}

/******************************************************************************
chức năng :	Hình ảnh của khung hình trước đó phải được tải lên, nếu không
		        vài giây đầu tiên sẽ hiển thị ngoại lệ.
tham số:
******************************************************************************/
void EPD_2IN13_V2_DisplayPartBaseImage(UBYTE *Image)
{
    UWORD Width, Height;
    Width = (EPD_2IN13_V2_WIDTH % 8 == 0) ? (EPD_2IN13_V2_WIDTH / 8) : (EPD_2IN13_V2_WIDTH / 8 + 1);
    Height = EPD_2IN13_V2_HEIGHT;

    UDOUBLE Addr = 0;
    EPD_2IN13_V2_SendCommand(0x24);
    for (UWORD j = 0; j < Height; j++)
    {
        for (UWORD i = 0; i < Width; i++)
        {
            Addr = i + j * Width;
            EPD_2IN13_V2_SendData(Image[Addr]);
        }
    }
    EPD_2IN13_V2_SendCommand(0x26);
    for (UWORD j = 0; j < Height; j++)
    {
        for (UWORD i = 0; i < Width; i++)
        {
            Addr = i + j * Width;
            EPD_2IN13_V2_SendData(Image[Addr]);
        }
    }
    //   EPD_2IN13_V2_TurnOnDisplayPart();
    // EPD_2IN13_V2_TurnOnDisplay();
}

void EPD_2IN13_V2_DisplayPart(UBYTE *Image)
{
    UWORD Width, Height;
    Width = (EPD_2IN13_V2_WIDTH % 8 == 0) ? (EPD_2IN13_V2_WIDTH / 8) : (EPD_2IN13_V2_WIDTH / 8 + 1);
    Height = EPD_2IN13_V2_HEIGHT;
    UDOUBLE Addr = 0;
     EPD_2IN13_V2_SendCommand(0x24);
    for (UWORD j = 0; j < Height; j++)
    {
        for (UWORD i = 0; i < Width; i++)
        {
            Addr = i + j * Width;
            EPD_2IN13_V2_SendData(Image[Addr]);
        }
    }
    EPD_2IN13_V2_SendCommand(0x26);
    for (UWORD j = 0; j < Height; j++)
    {
        for (UWORD i = 0; i < Width; i++)
        {
            Addr = i + j * Width;
            EPD_2IN13_V2_SendData(~Image[Addr]);
        }
    }

    // EPD_2IN13_V2_SendCommand(0x26);
  
}

/******************************************************************************
chức năng :	Vào chế độ ngủ
tham số:
******************************************************************************/
void EPD_2IN13_V2_Sleep(void)
{
    // EPD_2IN13_V2_SendCommand(0x22); //POWER OFF
    // EPD_2IN13_V2_SendData(0xC3);
    // EPD_2IN13_V2_SendCommand(0x20);

    EPD_2IN13_V2_SendCommand(0x10); //vào chế độ ngủ sâu
    EPD_2IN13_V2_SendData(0x01);
    // DEV_Delay_ms(100);
}
