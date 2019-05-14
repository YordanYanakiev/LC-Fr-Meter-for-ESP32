#include "arduino.h"
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>

#define serial

#define TFT_CS     5
#define TFT_RST    2 
#define TFT_DC     4

#define TFT_SCLK   23
#define TFT_MOSI   19

#define charge_PIN GPIO_NUM_27
#define freqIn_PIN GPIO_NUM_25


Adafruit_ST7735 * tft;


double pulse, frequency, capacitance, inductance;
unsigned long pl;

portMUX_TYPE myMutex = portMUX_INITIALIZER_UNLOCKED;

void setup()
{

	Serial.begin( 115200 );
	pinMode( freqIn_PIN, INPUT );
	pinMode( charge_PIN, OUTPUT );
	

	tft = new Adafruit_ST7735( TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST );
	tft->initR( INITR_BLACKTAB );//initialize a ST7735S chip, black tab  
	tft->setRotation( 1 );
	tft->fillScreen( ST7735_BLACK );
	
	delay( 300 );
}

void loop()
{
	taskENTER_CRITICAL( &myMutex );
		digitalWrite( charge_PIN, HIGH );
		delay( 5 );//give some time to charge inductor.
		digitalWrite( charge_PIN, LOW );
		delayMicroseconds( 100 ); //make sure resination is measured
	
		pulse = pulseIn( freqIn_PIN, HIGH, 5000 );//returns 0 if timeout
	taskEXIT_CRITICAL( &myMutex );
	//pl = pulseIn( freqIn, HIGH, 5000 );//returns 0 if timeout

	//Serial.println( pulse );
	if( pulse > 0.1 )
	{ //if a timeout did not occur and it took a reading:
		capacitance = 1.E-8; //10 nf
		frequency = 1.E6 / ( 2 * pulse );
		inductance = 1. / ( capacitance * frequency * frequency * 4. * 3.14159 * 3.14159 );//one of my profs told me just do squares like this
		inductance *= 1E6; //note that this is the same as saying inductance = inductance*1E6
		
		if( inductance >= 1000 )
		{
			drawTexxt( 50, 94, 100, String( pulse ) + " uS" );

			drawTexxt( 50, 34, 100, String( inductance / 1000 ) + " mH" );
			drawTexxt( 20, 66, 120, "Resonance : " + String( frequency ) + " Hz" );
			/*
			Serial.print( "High for uS:" );
			Serial.print( pulse );
			Serial.print( "\tfrequency Hz:" );
			Serial.print( frequency );
			Serial.print( "\tinductance mH:" );
			Serial.println( inductance / 1000 );
			*/
			delay( 300 );
		
		}
		else
		{

			drawTexxt( 50, 94, 100, String( pulse ) + " uS" );

			drawTexxt( 50, 34, 100, String( inductance ) + " uH" );
			drawTexxt( 20, 66, 120, "Resonance : " + String( frequency ) + " Hz" );
			/*
			Serial.print( "High for uS:" );
			Serial.print( pulse );
			Serial.print( "\tfrequency Hz:" );
			Serial.print( frequency );
			Serial.print( "\tinductance uH:" ); Serial.println( inductance );
			*/

			delay( 300 );
		
		}
	}
	else 
		if( pulse < 0.1 )
		{
			drawTexxt( 50, 94, 100, "" );
			drawTexxt( 50, 34, 100, "Insert Inductor" );
			drawTexxt( 20, 66, 120, "" );

			delay( 500 );
			
		}

}


void drawTexxt( int16_t x, int16_t y, int16_t maxTextWidth, String text )
{
	tft->fillRect( x, y, maxTextWidth, 20, ST7735_BLACK );
	tft->setCursor( x, y );
	tft->setTextColor( ST7735_WHITE );
	tft->setTextWrap( false );
	tft->print( text );
}
