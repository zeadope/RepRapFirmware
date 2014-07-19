/****************************************************************************************************

RepRapFirmware - Configuration

This is where all machine-independent configuration and other definitions are set up.  Nothing that
depends on any particular RepRap, RepRap component, or RepRap controller  should go in here.  Define 
machine-dependent things in Platform.h

-----------------------------------------------------------------------------------------------------

Version 0.1

18 November 2012

Adrian Bowyer
RepRap Professional Ltd
http://reprappro.com

Licence: GPL

****************************************************************************************************/

#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#define NAME "RepRapFirmware"
#define VERSION "0.89"
#define DATE "2014-07-14"
#define LAST_AUTHOR "reprappro"

// Other firmware that we might switch to be compatible with.

enum Compatibility
{
	me = 0,
	reprapFirmware = 1,
	marlin = 2,
	teacup = 3,
	sprinter = 4,
	repetier = 5
};

// Some numbers...

#define ABS_ZERO (-273.15)  // Celsius

#define INCH_TO_MM (25.4)

#define HEAT_SAMPLE_TIME (0.5) // Seconds

#define TEMPERATURE_CLOSE_ENOUGH (3.0) 		// Celsius
#define TEMPERATURE_LOW_SO_DONT_CARE (40.0)	// Celsius
#define HOT_ENOUGH_TO_EXTRUDE (170.0)       // Celsius
#define TIME_TO_HOT (120.0)					// Seconds

// If temperatures fall outside this range, something
// nasty has happened.

#define MAX_BAD_TEMPERATURE_COUNT 6
#define BAD_LOW_TEMPERATURE -10.0
#define BAD_HIGH_TEMPERATURE 300.0

#define STANDBY_INTERRUPT_RATE 2.0e-4 // Seconds

#define NUMBER_OF_PROBE_POINTS 5	  // Maximum number of probe points
#define Z_DIVE 8.0  				  // Height from which to probe the bed (mm)
#define TRIANGLE_0 -0.001			  // Slightly less than 0 for point-in-triangle tests

#define SILLY_Z_VALUE -9999.0

// Webserver stuff

#define DEFAULT_PASSWORD "reprap"
#define DEFAULT_NAME "My RepRap 1"
#define INDEX_PAGE "reprap.htm"
#define MESSAGE_FILE "messages.txt"
#define FOUR04_FILE "html404.htm"
#define CONFIG_FILE "config.g"         // The file that sets the machine's parameters
#define DEFAULT_FILE "default.g"       // If the config file isn't found
#define HOME_X_G "homex.g"
#define HOME_Y_G "homey.g"
#define HOME_Z_G "homez.g"
#define HOME_ALL_G "homeall.g"

#define WEB_DEBUG_TRUE 9
#define WEB_DEBUG_FALSE 8

#define LIST_SEPARATOR ':'						// Lists in G Codes
#define FILE_LIST_SEPARATOR ','					// Put this between file names when listing them
#define FILE_LIST_BRACKET '"'					// Put these round file names when listing them

#define LONG_TIME 300.0 // Seconds

#define EOF_STRING "<!-- **EoF** -->"           // For HTML uploads

#define FLASH_LED 'F' 							// Type byte of a message that is to flash an LED; the next two bytes define
                      	  	  	  	  	  	  	// the frequency and M/S ratio.
#define DISPLAY_MESSAGE 'L'  					// Type byte of a message that is to appear on a local display; the L is
                             	 	 	 	 	// not displayed; \f and \n should be supported.
#define HOST_MESSAGE 'H' 						// Type byte of a message that is to be sent to the host via USB; the H is not sent.
#define WEB_MESSAGE 'W'							// Type byte of message that is to be sent to the web
#define WEB_ERROR_MESSAGE 'E'					// Type byte of message that is to be sent to the web - flags an error
#define BOTH_MESSAGE 'B'						// Type byte of message that is to be sent to the web & host
#define BOTH_ERROR_MESSAGE 'A'					// Type byte of message that is to be sent to the web & host - flags an error

#endif
