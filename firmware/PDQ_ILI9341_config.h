/**
   PDQ_ILI9341 configuration
*/
#define	ILI9341_CS_PIN		6			// <= /CS pin (chip-select, LOW to get attention of ILI9341, HIGH and it ignores SPI bus)
#define	ILI9341_DC_PIN		7			// <= DC pin (1=data or 0=command indicator line) also called RS
#define	ILI9341_RST_PIN		8			// <= RST pin (optional)

// other PDQ library options
#define	ILI9341_SAVE_SPCR	0			// <= 0/1 with 1 to save/restore AVR SPI control register (to "play nice" when other SPI use)
