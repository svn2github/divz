#ifndef GLPlayerCommandNames_H
#define GLPlayerCommandNames_H

// login/setup
#define GLPlayer_Login "Login"
#define GLPlayer_Ping "Ping"
#define GLPlayer_ListVideoInputs "ListVideoInputs"

// player operation
#define GLPlayer_SetBlackout "SetBlackout"
#define GLPlayer_SetCrossfadeSpeed "SetCrossfadeSpeed"

#define GLPlayer_LoadSlideGroup "LoadSlideGroup"
#define GLPlayer_SetSlide "SetSlide"
#define GLPlayer_SetLayout "SetLayout"
#define GLPlayer_SetUserProperty "SetUserProperty"
#define GLPlayer_SetVisibility "SetVisibility"
#define GLPlayer_DownloadFile "DownloadFile"
#define GLPlayer_QueryProperty "QueryProperty"
#define GLPlayer_QueryLayout "QueryLayout"

// basic output config
#define GLPlayer_SetIgnoreAR "SetIgnoreAR"
#define GLPlayer_SetViewport "SetViewport"
#define GLPlayer_SetCanvasSize "SetCanvasSize"
#define GLPlayer_SetScreen "SetScreen"

// subviews
#define GLPlayer_AddSubview "AddSubview"
#define GLPlayer_RemoveSubview "RemoveSubview"
#define GLPlayer_ClearSubviews "ClearSubviews"

// playlist commands
#define GLPlayer_SetPlaylistPlaying "SetPlaylistPlaying"
#define GLPlayer_UpdatePlaylist "UpdatePlaylist"
#define GLPlayer_SetPlaylistTime "SetPlaylistTime"
// The next 2 are sent FROM the player TO the director
#define GLPlayer_PlaylistTimeChanged "PlaylistTimeChanged"
#define GLPlayer_CurrentPlaylistItemChanged "CurrentPlaylistItemChanged"

// TODO not implemented below, should we implement these since they are all in subviews?
#define GLPlayer_SetAlphaMask "SetAlphaMask"
#define GLPlayer_SetKeystone "SetKeystone"
#define GLPlayer_SetWindow "SetWindow"
#define GLPlayer_SetFlip "SetFlip"
#define GLPlayer_SetColors "SetColors"

#endif
