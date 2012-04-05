
#ifndef __myDefines
#define __myDefines

#define DEBUG   //needed before all #includes
#define CPLDport P4OUT    //port used to send data
#define CPLDportDir P4DIR 

// KEYCODES for the keypad
#define K1 20
#define K2 18
#define K3 24
#define K4 36
#define K5 34
#define K6 40
#define K7 68
#define K8 66
#define K9 72
#define KStar 132
#define K0 130
#define KPound 136

// GAME States
#define GS_LOGIN
#define GS_PLAY
#define GS_STOP

// SERIAL Commands
#define cmd_ClearScreen 'c'
#define cmd_Help '?'
#define cmd_Test 't'
#define cmd_NewUser 'n'
#define cmd_HighScores 'h'
#define cmd_Menu 'm'

// SERIAL Send Codes
#define CarriageReturn 0xD
#define NewLine "\033E"
#define ClearScreen "\033[2J\033[H"



#endif /* #ifndef __myDefines */
