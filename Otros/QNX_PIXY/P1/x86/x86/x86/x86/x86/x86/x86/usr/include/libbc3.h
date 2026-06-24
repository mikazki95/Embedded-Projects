/*
 * libbc3.h
 *
 * (c) Copyright 2009 Stefan Strobl, Gersys GmbH <stefan.strobl@gersys.de>
 *
 * Header file to access functions in libbc3.so.x.x
 *
 * This software is licensed under a proprietary licence and generally
 * not made available open source.
 *
 */

#ifndef _LIBBC3_H_
#define _LIBBC3_H_

#ifdef __cplusplus
extern "C" {
#endif

#define LIBBC3_VERSION "0.0.2"

    typedef int bool;


/* Init()
 *
 * Description:
 *  Initialization function. This function will initialise communications with
 *  the environment controller and will set some default values such as
 *  brightness level, CCFL  control mode (auto or manual), CCFL state (on/off),
 *  buzzer state (on/off), key repetition (on/off), maximum bright level,
 *  minimum bright level.
 *
 * Parameter:
 *  int brightness:      brightness level at power-on (0 to 255 dec)
 *  bool autoBrightness: auto/man brightness control at power-on (TRUE=auto)
 *  bool dispEnable:     En/Disable LCD Backlight at power-on
 *  bool buzzerEnable:   En/Disable Buzzer at power-on
 *  bool keyRepetition:  En/Disable Autorepeat at power-on
 *  int maxBright:       maximum brightness at autom. control (0 to 255 dec)
 *  int minBright:       minimum brightness at autom. control (0 to 255 dec)
 *
 * Return Value:
 *   0 = no error
 *  -1 = elsewise
 *
 * Limitations @ BC35XX Prototype:
 *  - the brightness level is to specified as an integer between 0 and 255
 *    but will be reduced to a level between 0 and 49.
 *  - setting buzzerEnable will not have any effect
 *  - setting maxBright will not have any effect
 */
    int Init( int brightness, bool autoBrightness, bool dispEnable,
	      bool buzzerEnable, bool keyRepetition, int maxBright,
	      int minBright );


/*
 *
 * Description:
 *  Callback function for critical temperature. This function will notify if
 *  the device has reached critical temperature or not.
 *
 * Parameter:
 *  -/-
 *
 * Return Value:
 *  -/-
 *
 * Limitations @ BC35XX Prototype:
 *  - Currently not supported.
 */


/* SetLedStat()
 *
 * Description:
 *  Function for modifying LED state. This function will switch on/off the
 *  specified LED.
 *
 * Parameter:
 *  unsigned short ledID: ID of LED => 1 or 2
 *  bool stateOn:         En/Disable LED
 *
 * Return Value:
 *  -/-
 *
 * Limitations @ BC35XX Prototype:
 *  -/-
 */
    void SetLedState( unsigned short ledID, bool stateOn );


/* SetBeepState()
 *
 * Description:
 *  Function for modifying beep state. This function will switch on/off the
 *  internal and external buzzer for the specified time in seconds.
 *
 * Parameter:
 *  bool stateOn:   En/Disable Buzzer
 *  double soundTm: Beep duration in seconds (0.0 to 25.5s)
 *
 * Return Value:
 *  -/-
 *
 * Limitations @ BC35XX Prototype:
 *  - The maximum resolution is 100ms
 *  - The maximum total beep duration is 25.5s
 */
    void SetBeepState( bool stateOn, double soundTm );


/* SetDisplayBrightness()
 *
 * Description:
 *  Function for manual CCFL control setting. This function will set the
 *  specified brightness level.
 *
 * Parameter:
 *  unsigned long density: brightness density in cd/m² (0 to 500 cd/m²)
 *
 * Return Value:
 *  -/-
 *
 * Limitations @ BC35XX Prototype:
 *  - No relation between specified value and actual brightness density in
 *    cd/m². Specified value between 0 and 500 will be reduced to a
 *    simplified brightness level of 0 to 49.
 */
    void SetDisplayBrightness( unsigned long density );


/* SetAutomaticBrightness()
 *
 * Description:
 *  Function for automatic LCD backlight control setting. This function will
 *  enable the automatic LCD backlight control and will add an offset to the
 *  brightness level.
 *
 * Parameter:
 *  bool on:    En/Disable automatic backlight control (Disable = manual ctrl)
 *  int offset: Offset that will be added to auto brightness level. (-7 to +7)
 *
 * Return Value:
 *  -/-
 *
 * Limitations @ BC345X Prototype:
 *  -/-
 */
    void SetAutomaticBrightness( bool on, int offset );


/* SwitchDisplayOn()
 *
 * Description:
 *  Function for changing display state. This function will switch on/off
 *  the display backlight.
 *
 * Parameter:
 *  bool on: En/Disable LCD backlight
 *
 * Return Value:
 *  -/-
 *
 * Limitations @ BC35XX Prototype:
 *  -/-
 */
    void SwitchDisplayOn( bool on );


/* GetTempOffLow()
 *
 * Description:
 *  Function for obtaining the device temperature Off Low Limit. This function will return
 *  the temperature in degree Celsius which the device has to pass under to temperature
 *  emergency shutdown.
 *
 * Parameter:
 *  int *temp: Pointer to an integer variable where the temperature in deg C
 *             is to be stored.
 *
 * Return Value:
 *   0 = no error
 *  -1 = elsewise
 *
 * Limitations @ BC35XX Prototype:
 *  -/-
 */
    int GetTempOffLow( int *temp );


/* GetTempOffHigh()
 *
 * Description:
 *  Function for obtaining the device temperature Off High Limit. This function will return
 *  the temperature in degree Celsius which the device has to pass over to temperature
 *  emergency shutdown.
 *
 * Parameter:
 *  int *temp: Pointer to an integer variable where the temperature in deg C
 *             is to be stored.
 *
 * Return Value:
 *   0 = no error
 *  -1 = elsewise
 *
  * Limitations @ BC35XX Prototype:
 *  -/-
 */
    int GetTempOffHigh( int *temp );


/* GetDisplayTemperature()
 *
 * Description:
 *  Function for obtaining the device temperature. This function will return
 *  the temperature in degree Celsius inside the device.
 *
 * Parameter:
 *  int *temp: Pointer to an integer variable where the temperature in deg C
 *             is to be stored.
 *
 * Return Value:
 *   0 = no error
 *  -1 = elsewise
 *
 * Limitations @ BC35XX Prototype:
 *  -/-
 */
    int GetDisplayTemperature( int *temp );


/* GetDisplayBrightness()
 *
 * Description:
 *  Function for obtaining applied brightness in cd/m². This function will
 *  obtain the display current brightness level.
 *
 * Parameter:
 *  int *bright: Pointer to an integer variable where the brightness in cd/m²
 *               is to be stored.
 *
 * Return Value:
 *   0 = no error
 *  -1 = elsewise
 *
 * Limitations @ BC35XX Prototype:
 *  - No relation between given value and actual brightness density in cd/m².
 *    Specified value between 0 and 500 will be calculated from a brightness
 *    level of 0 to 49.
 */
    int GetDisplayBrightness( int *bright );


/* GetAmbientBrightness()
 *
 * Description:
 *  Function for obtaining the ambient brightness in lux. This function
 *  will obtain the ambient brightness read by the light sensor.
 *
 * Parameter:
 *  int *bright: Pointer to an integer variable where the illuminance
 *               in lux is stored.
 *
 * Return Value:
 *   0 = no error
 *  -1 = elsewise
 *
 * Limitations @ BC35XX Prototype:
 *  -/-
 */
    int GetAmbientBrightness( int *bright );


/* GetCabineIdent()
 *
 * Description:
 *  Function for obtaining the cabin identifier. This function will return
 *  cabin 1 if no bridge is made in input pins, and 2 if any of the pins
 *  are bridged.
 *
 * Parameter:
 *  -/-
 *
 * Return Value:
 *  unsigned int: Identifier, cabine 1 or 2
 *
 * Limitations @ BC35XX Prototype:
 *  -/-
 */
    unsigned int GetCabineIdent( void );


/* GetNumberOfLeds()
 *
 * Description:
 *  Function for obtaining the number of LED's. This function will obtain
 *  the number of LEDs the device has.
 *
 * Parameter:
 *  -/-
 *
 * Return Value:
 *  unsigned int: number or LEDs => 2
 *
 * Limitations @ BC35XX Prototype:
 *  -/-
 */
    unsigned int GetNumberOfLeds( void );


/* UpdateWatchdog()
 *
 * Description:
 *  Function for updating the watchdog. This function will reset the
 *  watchdog counter.
 *
 * Parameter:
 *  -/-
 *
 * Return Value:
 *  -/-
 *
 * Limitations @ BC35XX Prototype:
 *  - Calling this function will reset the counter to 255 seconds which is
 *    the maximum time. If WDT is not enabled calling this function will
 *    not do anything.
 */
    void UpdateWatchdog( void );


/* StartWatchdog()
 *
 * Description:
 *  Function for launching the watchdog and setting the timeout. This
 *  function will start/stop the watchdog and will set the maximum wait
 *  wait period in milliseconds.
 *
 * Parameter:
 *  bool start:            En/Disable WDT
 *  unsigned long timeout: maximum wait period in milliseconds
 *
 * Return Value:
 *  -/-
 *
 * Limitations @ BC35XX Prototype:
 *  - maximum resolution is 1 second, hence milliseconds will be
 *    rounded up to the next full second.
 *  - the maximum timeout is 255 seconds.
 */
    void StartWatchdog( bool start, unsigned long timeout );


/* Enable Watchdog()
 *
 * Description:
 *  Function for en/disabling the watchdog.
 *
 * Parameter:
 *  bool enable: En/Disable WDT
 *
 * Return Value:
 *  -/-
 *
 * Limitations @ BC35XX Prototype:
 *  - When enabling WDT the timout will always be set to 255 seconds
 *    which is the maximum. This function will hence be equal to calling
 *    calling StartWatchdog ( bool enable, 255 );
 */
    void EnableWatchdog( bool enable );


/* EnableKeyboardData()
 *
 * Description:
 *  Function for en/disabling the keyboard.
 *
 * Parameter:
 *  bool enable: En/Disable Keyboard
 *
 * Return Value:
 *  -/-
 *
 * Limitations @ BC35XX Prototype:
 *  -/-
 */
    void EnableKeyboardData( bool enable );


#ifdef __cplusplus
}
#endif

#endif /* _LIBBC3_H_ */
