#ifndef VideoSenderCommands_H
#define VideoSenderCommands_H

// For the next three "set" commands, a single argument
// expected, called "value", type "int", as a percent - e.g. {value:50} means 50%
#define Video_SetHue "SetHue"
#define Video_SetSaturation "SetSaturation"
#define Video_SetBright "SetBright"
#define Video_SetContrast "SetContrast"

// FPS: Arg 'fps', type int
#define Video_SetFPS "SetFPS"
// Size: Arg 'w', type 'int', Arg 'h', type 'int'
#define Video_SetSize "SetSize"

#endif
