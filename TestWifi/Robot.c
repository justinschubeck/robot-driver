#include <G8RTOS.h>
#include <G8RTOS_Scheduler.h>

/* Struct For Host To Hold Game Data */
State_t ourGame;

/* Mutexes For Both Host And Client To Update */
semaphore_t LEDMutex, LCDMutex, WifiMutex;

/* Struct For Client To Hold Its Personal Data */
DeviceInfo_t clientInfo;

/* JoyStick Coordinates */
int16_t X, Y;

///*********************************************** Client Threads *********************************************************************/
///*
// * Thread for client to join game
// */
//void JoinGame()
//{
//    /* Initializes The Host Paddle Center, Color, Position, and Previous Center */
//    ourGame.players[BOTTOM].currentCenter = PADDLE_X_CENTER;
//    ourGame.players[BOTTOM].color = PLAYER_BLUE;
//    ourGame.players[BOTTOM].position = BOTTOM;
//    hostPrev.Center = PADDLE_X_CENTER;
//
//    /* Initializes The Client Paddle Center, Color, Position, and Previous Center */
//    ourGame.players[TOP].currentCenter = PADDLE_X_CENTER;
//    ourGame.players[TOP].color = PLAYER_RED;
//    ourGame.players[TOP].position = TOP;
//    clientPrev.Center = PADDLE_X_CENTER;
//
//    /* Set Initial SpecificPlayerInfo_t Struct Attributes */
//    clientInfo.IP_address = getLocalIP();
//    clientInfo.displacement = 0;
//    clientInfo.playerNumber = TOP;
//    clientInfo.ready = true;
//    clientInfo.joined = false;
//    clientInfo.acknowledge = false;
//
//    /* Send Player To The Host */
//    _i32 returnVal = -1;
//    while((returnVal == -1) || (clientInfo.acknowledge == false))
//    {
//        clientInfo.acknowledge = true;
//        SendData((uint8_t *) &clientInfo, HOST_IP_ADDR, sizeof(clientInfo));
//        clientInfo.acknowledge = false;
//        returnVal = ReceiveData((uint8_t *) &clientInfo, sizeof(clientInfo));
//    }
//
//    /* Join Game */
//    clientInfo.joined = true;
//    SendData((uint8_t *)&clientInfo, HOST_IP_ADDR, sizeof(clientInfo));
//
//    /* If Joined, Acknowledge Joined To Host And Show Connection With Blue LED */
//    RED_LED_OFF;
//    BLUE_LED_ON;
//
//    /* Initialize The Board's Arena, Players, and Scores */
//    InitBoardState();
//
//    /* Add Starting Threads */
//    G8RTOS_AddThread(ReadJoystickClient, 5, "JoyStick");
//    G8RTOS_AddThread(SendDataToHost, 5, "Send Data");
//    G8RTOS_AddThread(ReceiveDataFromHost, 5, "Receive Data");
//    G8RTOS_AddThread(DrawObjects, 5, "Draw Stuff");
//    G8RTOS_AddThread(MoveLEDs, 10, "LEDs");
//    G8RTOS_AddThread(IdleThread, 255, "Idle");
//
//    /* Kill Self */
//    G8RTOS_KillSelf();
//}
//
///*
// * Thread that receives game state packets from host
// */
//void ReceiveDataFromHost()
//{
//    while(1)
//    {
//        /* Does Not Update ourGame While Drawing To Avoid Glitches */
//        G8RTOS_WaitSemaphore(&LCDMutex);
//
//        /* Save Previous Locations */
//        hostPrev.Center = ourGame.players[BOTTOM].currentCenter;
//        clientPrev.Center = ourGame.players[TOP].currentCenter;
//        for(uint8_t i = 0; i < MAX_NUM_OF_BALLS; i++)
//        {
//            prevBalls[i].CenterX = ourGame.balls[i].currentCenterX;
//            prevBalls[i].CenterY = ourGame.balls[i].currentCenterY;
//        }
//
//        /* Continually Receive Data Until A Return Value Greater Than 0 Is Returned
//         * Release And Take Semaphore Again To Send Data, Sleep 1ms To Avoid Deadlock
//         * Empty Packet */
//        _i32 returnVal = -1;
//        while(returnVal == -1)
//        {
//            G8RTOS_WaitSemaphore(&WifiMutex);
//            returnVal = ReceiveData((uint8_t *) &ourGame, sizeof(ourGame));
//            G8RTOS_SignalSemaphore(&WifiMutex);
//            sleep(1);
//        }
//        G8RTOS_SignalSemaphore(&LCDMutex);
//
//        /* If Game Done, Add EndOfGameClient With Highest Priority */
//        if(ourGame.winner)
//        {
//            clientInfo.acknowledge = true;
//            G8RTOS_WaitSemaphore(&WifiMutex);
//            SendData((uint8_t *)&clientInfo, HOST_IP_ADDR, sizeof(clientInfo));
//            G8RTOS_SignalSemaphore(&WifiMutex);
//            clientInfo.acknowledge = false;
//            G8RTOS_AddThread(EndOfGameClient, 0, "End Game");
//        }
//
//        /* Sleep For 25ms */
//        sleep(25);
//    }
//}
//
///*********************************************** Client Threads *********************************************************************/
//
//
///*********************************************** Host Threads *********************************************************************/
///*
// * Thread for the host to create a game.
// * Only Thread Created Before Launching OS
// */
//void CreateGame()
//{
//    /* Initializes The Host */
//    ourGame.players[BOTTOM].currentCenter = PADDLE_X_CENTER;
//    ourGame.players[BOTTOM].color = PLAYER_BLUE;
//    ourGame.players[BOTTOM].position = BOTTOM;
//    hostPrev.Center = PADDLE_X_CENTER;
//
//    /* Initializes The Client */
//    ourGame.players[TOP].currentCenter = PADDLE_X_CENTER;
//    ourGame.players[TOP].color = PLAYER_RED;
//    ourGame.players[TOP].position = TOP;
//    clientPrev.Center = PADDLE_X_CENTER;
//
//    /* Establish Connection With Client
//     * Should Be Trying To Receive A Packet From The Client
//     * Should Acknowledge Client Once Client Has Joined
//     * Red LED Indicating No Wi-Fi Connection */
//
//    /* Wait For Client To Sent Specific Player Information */
//    _i32 returnVal = -1;
//    while((returnVal == -1) || (ourGame.player.acknowledge == false))
//    {
//        returnVal = ReceiveData((uint8_t *)&ourGame.player, sizeof(ourGame.player));
//    }
//
//    /* Acknowledge That We Have Connected With Client */
//    ourGame.player.acknowledge = true;
//    SendData((uint8_t *)&ourGame.player, ourGame.player.IP_address, sizeof(ourGame.player));
//    ourGame.player.acknowledge = false;
//
//    /* Wait For Client To Join */
//    returnVal = -1;
//    while((returnVal == -1) || (ourGame.player.joined == false))
//    {
//        returnVal = ReceiveData((uint8_t *)&ourGame.player, sizeof(ourGame.player));
//    }
//
//    /* Blue LED Indicating Good Connection */
//    RED_LED_OFF;
//    BLUE_LED_ON;
//
//    /* Initialize The Board's Arena, Players, and Scores */
//    InitBoardState();
//
//    /* Add Starting Threads */
//    G8RTOS_AddThread(GenerateBall, 5, "Add Ball");
//    G8RTOS_AddThread(DrawObjects, 5, "Draw Stuff");
//    G8RTOS_AddThread(ReadJoystickHost, 5, "JoyStick");
//    G8RTOS_AddThread(SendDataToClient, 5, "Send Data");
//    G8RTOS_AddThread(ReceiveDataFromClient, 5, "Receive Data");
//    G8RTOS_AddThread(MoveLEDs, 10, "LEDs");
//    G8RTOS_AddThread(IdleThread, 255, "Idle");
//
//    /* Kill Self */
//    G8RTOS_KillSelf();
//}
//
///*
// * Thread that sends game state to client
// */
//void SendDataToClient()
//{
//    while(1)
//    {
//        /* Fill And Send Packet For Client */
//        G8RTOS_WaitSemaphore(&WifiMutex);
//        SendData((uint8_t *)&ourGame, clientInfo.IP_address, sizeof(ourGame));
//        G8RTOS_SignalSemaphore(&WifiMutex);
//
//        /* Check If Game Is Done, If So, Add EndOfGameHost With Highest Priority */
//        if(ourGame.winner)
//        {
//            _i32 returnVal = -1;
//            G8RTOS_WaitSemaphore(&WifiMutex);
//            ourGame.player.acknowledge = false;
//            while((returnVal == -1) || (ourGame.player.acknowledge == false))
//            {
//                SendData((uint8_t *) &ourGame, clientInfo.IP_address, sizeof(ourGame));
//                returnVal = ReceiveData((uint8_t *) &ourGame.player, sizeof(ourGame.player));
//            }
//            G8RTOS_SignalSemaphore(&WifiMutex);
//            G8RTOS_AddThread(EndOfGameHost, 0, "End Game");
//        }
//
//        /* Sleep For 15ms (Experimentally For Synchronization) */
//        sleep(15);
//    }
//}
//
///*
// * Thread to read host's joystick
// */
//void ReadJoystickHost()
//{
//    int8_t displacement = 0;
//    while(1)
//    {
//        /* Read Joystick Coordinates */
//        GetJoystickCoordinates(&X, &Y);
//
//        /* Add Bias And Change Self.displacement*/
//        if(X > 1500){displacement = -1;}
//        else if(X < -1500){displacement = 1;}
//        else{displacement = 0;}
//
//        /* Sleep For 10ms */
//        sleep(10);
//
//        /* Add Displacement To Bottom Player */
//        hostPrev.Center = ourGame.players[BOTTOM].currentCenter;
//        if(displacement == 1)
//        {
//            if(ourGame.players[BOTTOM].currentCenter < HORIZ_CENTER_MAX_PL)
//            {
//                ourGame.players[BOTTOM].currentCenter += displacement;
//            }
//        }
//        else if(displacement == -1)
//        {
//            if(ourGame.players[BOTTOM].currentCenter >= HORIZ_CENTER_MIN_PL)
//            {
//                ourGame.players[BOTTOM].currentCenter += displacement;
//            }
//        }
//    }
//}
//
//void ButtonTap()
//{
//    newGame = true;
//    P4->IFG &= ~BIT5;
//}
///*********************************************** Host Threads *********************************************************************/


/*********************************************** Common Threads *********************************************************************/
/*
 * Idle thread
 */
void IdleThread()
{
    while(1);
}
/*********************************************** Common Threads *********************************************************************/

/*********************************************** Public Functions *********************************************************************/
/*
 * Returns either Host or Client depending on button press
 */
playerType GetPlayerRole()
{
    /* Initialize Top Button S1 P1.1 For Client */
    P1->DIR &= ~BIT1;
    P1->REN |= BIT1;
    P1->OUT |= BIT1;

    /* Initialize Bottom Button S2 P1.4 For Host */
    P1->DIR &= ~BIT4;
    P1->REN |= BIT4;
    P1->OUT |= BIT4;

    /* Initialize WiFi Connection LEDs For Both Roles */
    P2->SEL1 &= ~7;
    P2->SEL0 &= ~7;
    P2->DIR  |= 7;
    ALL_LEDS_OFF;
    RED_LED_ON;

    while(1)
    {
        if(!(P1->IN & BIT1)){return Client;}
        if(!(P1->IN & BIT4)){return Host;}
    }
}
/*********************************************** Public Functions *********************************************************************/
