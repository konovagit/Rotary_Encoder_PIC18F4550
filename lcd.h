#ifndef LCD_H
#define	LCD_H

#ifdef	__cplusplus
extern "C" {
#endif
#include <p18f4550.h>    
    
#define _XTAL_FREQ 8000000 
//Pour enlever les Warnings et redefinir
#undef DATA_PORT
#undef TRIS_DATA_PORT
#undef E_PIN
#undef TRIS_E
#undef RW_PIN
#undef TRIS_RW
#undef RS_PIN
#undef TRIS_RS
#define DATA_PORT      		PORTA
#define TRIS_DATA_PORT 		TRISA
#define E_PIN    LATEbits.LATE0  		/* PORT for E  */
#define TRIS_E   TRISEbits.TRISE0    	/* TRIS for E  */
#define RW_PIN   LATEbits.LATE1   		/* PORT for RW */
#define TRIS_RW  TRISEbits.TRISE1    	/* TRIS for RW */
#define RS_PIN   LATEbits.LATE2   		/* PORT for RS */
#define TRIS_RS  TRISEbits.TRISE2    	/* TRIS for RS */

unsigned char lcd_busy_check(void);
void lcd_write_cmd(unsigned char cmd);
void lcd_set_ddram_address(unsigned char addr);
void lcd_write_data(char data);
void lcd_init(unsigned char lcdtype);
void lcd_clear_display();
void lcd_put_string(char *buffer);


#ifdef	__cplusplus
}
#endif

#endif	/* LCD_H */