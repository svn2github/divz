#ifndef ServerCommandNames_H
#define ServerCommandNames_H

// login/setup
#define Server_Login "Login"
#define Server_Ping "Ping"
#define Server_ListVideoInputs "ListVideoInputs"

#define Server_SetStreamURL "SetStreamURL"

// player operation
#define Server_SetBlackout "SetBlackout"
#define Server_SetCrossfadeSpeed "SetCrossfadeSpeed"

// basic output config
#define Server_SetIgnoreAR "SetIgnoreAR"
#define Server_SetScreen "SetScreen"

// overlays
#define Server_AddOverlay "AddOverlay"
#define Server_RemoveOverlay "RemoveOverlay"

/// For detecting dead connections by the client...
#define Server_DeadPing "DeadPing"

#endif
