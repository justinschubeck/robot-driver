#ifndef ROBOT_H_
#define ROBOT_H_

/*********************************************** Includes ********************************************************************/
#include <stdbool.h>
#include <stdint.h>
#include "G8RTOS_Lab5/G8RTOS.h"
#include "CC3100SupportPackage\cc3100_usage\cc3100_usage.h"
/*********************************************** Includes ********************************************************************/

/*********************************************** Externs ********************************************************************/
extern semaphore_t LCDMutex;
extern semaphore_t WifiMutex;
/*********************************************** Externs ********************************************************************/

/*********************************************** Global Defines ********************************************************************/
/* Used as status LEDs for Wi-Fi */
#define RED_LED_ON    P2->OUT  |= 1
#define RED_LED_OFF   P2->OUT  &= ~1
#define GREEN_LED_ON  P2->OUT  |= 2
#define GREEN_LED_OFF P2->OUT  &= ~2
#define BLUE_LED_ON   P2->OUT  |= 4
#define BLUE_LED_OFF  P2->OUT  &= ~4
#define ALL_LEDS_OFF  P2->OUT  &= ~7
#define ALL_LEDS_ON   P2->OUT  |= 7
/*********************************************** Global Defines ********************************************************************/

/*********************************************** Data Structures ********************************************************************/
#pragma pack ( push, 1)
/*
 * Struct to be sent from the client to the host
 */
typedef struct
{
    uint32_t IP_address;
    bool joined;
    bool acknowledge;
} DeviceInfo_t;

/*
 * Struct to be sent from the host to the client
 */
typedef struct
{
    uint8_t mode;
    uint16_t pulse;
} State_t;
#pragma pack ( pop )
/*********************************************** Data Structures ********************************************************************/


/*********************************************** Client Threads *********************************************************************/
/*
 * Thread for client to join game
 */
void JoinGame();

/*
 * Thread that receives game state packets from host
 */
void ReceiveDataFromHost();

/*********************************************** Client Threads *********************************************************************/


/*********************************************** Host Threads *********************************************************************/
/*
 * Thread for the host to create a game
 */
void CreateGame();

/*
 * Thread that sends game state to client
 */
void SendDataToClient();


/*
 * Thread to read host's joystick
 */
void ReadJoystickHost();

/*
 * APERIODIC
 * ISR For New Game Button
 * Should Just Set A Flag Saying A Touch Occurred
 */
void ButtonTap();

/*********************************************** Host Threads *********************************************************************/


/*********************************************** Common Threads *********************************************************************/
/*
 * Idle thread
 */
void IdleThread();
/*********************************************** Common Threads *********************************************************************/


/*********************************************** Public Functions *********************************************************************/
/*
 * Returns either Host or Client depending on button press
 */
playerType GetPlayerRole();
/*********************************************** Public Functions *********************************************************************/

#endif /* ROBOT_H_ */
