#include "lcd.h"
#define _XTAL_FREQ 8000000 

unsigned char lcd_busy_check(void)
{
    __delay_us(80);             // Must wait at least 80us after last instruction
                                // to check busy flag.
    ei();
    RW_PIN = 1;                 // Set the control bits for read
    RS_PIN = 0;
    E_PIN = 1;                  // Clock in the command
    _delay(2);                  // Data setup time: Max 320ns Actual 400ns
    if(DATA_PORT&0x08)
    {
        _delay(1);              // Remainder of enable pulse width: Min 480ns Actual 600ns
        E_PIN = 0;              // Reset clock line
        _delay(3);              // Data hold time and enable cycle time: Total 1200ns
        E_PIN = 1;              // Clock out other nibble
        _delay(3);              // Data setup time: Max 320ns Actual 400ns
        E_PIN = 0;
        _delay(3);              // Data hold time and enable cycle time: Total 1200ns
        RW_PIN = 0;             // Reset control line
        di();
        return 1;               // Return TRUE
    }
    else                            // Busy bit is low
    {
        _delay(1);              // Remainder of enable pulse width: Min 480ns Actual 600ns
        E_PIN = 0;              // Reset clock line
        _delay(3);              // Data hold time and enable cycle time: Total 1200ns
        E_PIN = 1;              // Clock out other nibble
        _delay(3);              // Data setup time: Max 320ns Actual 400ns
        E_PIN = 0;
        _delay(3);              // Data hold time and enable cycle time: Total 1200ns
        RW_PIN = 0;             // Reset control line
        di();
        return 0;               // Return FALSE
    }
}

void lcd_write_cmd(unsigned char cmd)
{
    ei();
    TRIS_DATA_PORT &= 0xf0;
    RS_PIN = 0;
    RW_PIN = 0;                     // Set control signals for command

    DATA_PORT &= 0xf0;
    DATA_PORT |= (cmd>>4)&0x0f;
    E_PIN = 1;                      // Clock command in
    _delay(1);                      // Data setup time: Max 80ns Actual 200ns
    _delay(2);                      // Remainder of enable pulse width: Min 460ns Actual 600ns
    E_PIN = 0;
    _delay(3);                      // Data hold time and enable cycle time: Total 1200ns

    DATA_PORT &= 0xf0;
    DATA_PORT |= cmd&0x0f;
    E_PIN = 1;                      // Clock command in
    _delay(1);                      // Data setup time: Max 80ns Actual 200ns
    _delay(2);                      // Remainder of enable pulse width: Min 460ns Actual 600ns
    E_PIN = 0;
    _delay(3);                      // Data hold time and enable cycle time: Total 1200ns

    TRIS_DATA_PORT |= 0x0f;
    di();

    return;
}

void lcd_set_ddram_address(unsigned char addr)
{
    ei();
    TRIS_DATA_PORT &= 0xf0;                 // Make port output
    RW_PIN = 0;                             // Set control bits
    RS_PIN = 0;

    DATA_PORT &= 0xf0;                      // and write upper nibble
    DATA_PORT |= (((addr | 0b10000000)>>4) & 0x0f);
    E_PIN = 1;                              // Clock the cmd and address in
    _delay(1);                      // Data setup time: Max 80ns Actual 200ns
    _delay(2);                      // Remainder of enable pulse width: Min 460ns Actual 600ns
    E_PIN = 0;
    _delay(3);                      // Data hold time and enable cycle time: Total 1200ns

    DATA_PORT &= 0xf0;                      // Write lower nibble
    DATA_PORT |= (addr&0x0f);
    E_PIN = 1;                              // Clock the cmd and address in
    _delay(1);                      // Data setup time: Max 80ns Actual 200ns
    _delay(2);                      // Remainder of enable pulse width: Min 460ns Actual 600ns
    E_PIN = 0;
    _delay(3);                      // Data hold time and enable cycle time: Total 1200ns

    TRIS_DATA_PORT |= 0x0f;                 // Make port input
    di();

    return;
}

void lcd_write_data(char data)
{
    ei();
    TRIS_DATA_PORT &= 0xf0;
    RS_PIN = 1;                     // Set control bits
    RW_PIN = 0;

    DATA_PORT &= 0xf0;
    DATA_PORT |= ((data>>4)&0x0f);
    E_PIN = 1;                      // Clock nibble into LCD
    _delay(1);                      // Data setup time: Max 80ns Actual 200ns
    _delay(2);                      // Remainder of enable pulse width: Min 460ns Actual 600ns
    E_PIN = 0;
    _delay(3);                      // Data hold time and enable cycle time: Total 1200ns

    DATA_PORT &= 0xf0;
    DATA_PORT |= (data&0x0f);
    E_PIN = 1;                      // Clock nibble into LCD
    _delay(1);                      // Data setup time: Max 80ns Actual 200ns
    _delay(2);                      // Remainder of enable pulse width: Min 460ns Actual 600ns
    E_PIN = 0;
    _delay(3);                      // Data hold time and enable cycle time: Total 1200ns

    TRIS_DATA_PORT |= 0x0f;
    di();

    return;
}

void lcd_init(unsigned char lcdtype)
{
    // The data bits must be either a 8-bit port or the upper or
    // lower 4-bits of a port. These pins are made into inputs
    DATA_PORT &= 0xf0;
    TRIS_DATA_PORT &= 0xF0;
    TRIS_RW = 0;                    // All control signals made outputs
    TRIS_RS = 0;
    TRIS_E = 0;
    RW_PIN = 0;                     // R/W pin made low
    RS_PIN = 0;                     // Register select pin made low
    E_PIN = 0;                      // Clock pin made low

    // Delay for 100ms to allow for LCD Power on reset
    __delay_ms(50);
    __delay_ms(50);

    //-------------------reset procedure through software----------------------
    lcd_write_cmd(0x30);
    __delay_ms(5);      // Delay more than 4.1ms

    lcd_write_cmd(0x30);
    __delay_us(100);     // Delay more than 100us

    lcd_write_cmd(0x30);
    __delay_us(100);     // Delay more than 100us

    lcd_write_cmd(0x20);
    __delay_us(100);     // Delay more than 100us

    // Set data interface width, # lines, font
    while(lcd_busy_check());              // Wait if LCD busy
    lcd_write_cmd(lcdtype);          // Function set cmd

    // Turn the display on then off
    while(lcd_busy_check());              // Wait if LCD busy
    lcd_write_cmd(DOFF&CURSOR_OFF&BLINK_OFF);        // Display OFF/Blink OFF
    while(lcd_busy_check());              // Wait if LCD busy
    lcd_write_cmd(DON&CURSOR_ON&BLINK_ON);           // Display ON/Blink ON

    // Clear display
    while(lcd_busy_check());              // Wait if LCD busy
    lcd_write_cmd(0x01);             // Clear display

    // Set entry mode inc, no shift
    while(lcd_busy_check());              // Wait if LCD busy
    lcd_write_cmd(0b00000110);       // Shift cursor right, increment DRAM. Don't shift display.

    // Set DD Ram address to 0
    while(lcd_busy_check());              // Wait if LCD busy
    lcd_set_ddram_address(0x00);            // Set Display data ram address to 0
    
    return;
}

void lcd_clear_display()
{
    // Clear the LCD by writing "20H" to all DDRAM addresses
    while(lcd_busy_check());              // Wait if LCD busy
    lcd_set_ddram_address(0x00);            // Set Display data ram address to 0

    for(int i=0; i<16; i++)
    {
        while(lcd_busy_check());          // Wait if LCD busy
        lcd_write_data(' ');         // Write character to LCD
    }

    while(lcd_busy_check());              // Wait if LCD busy
    lcd_set_ddram_address(0x40);            // Set Display data ram address to second line

    for(int i=0; i<16; i++)
    {
        while(lcd_busy_check());          // Wait if LCD busy
        lcd_write_data(' ');         // Write character to LCD
    }

    while(lcd_busy_check());              // Wait if LCD busy
    lcd_set_ddram_address(0x00);            // Set Display data ram address to 0

    return;
}

void lcd_put_string(char *buffer)
{
    lcd_clear_display();

    while(*buffer)               // Write data to LCD up to null
    {
        while(lcd_busy_check());        // Wait while LCD is busy
        if(*buffer=='\n')
          lcd_set_ddram_address(0x40);    // Set Display data ram address to second line
        else
          lcd_write_data(*buffer); // Write character to LCD
        buffer++;                // Increment buffer
    }

    return;
}
