#ifndef _USBAUDIO_H
#define _USBAUDIO_H

#include <usbdrvce.h>
#include <inttypes.h>

// defines and typedefs for usb shenanigans
#define DEFAULT_LANGID           0x0409

// interface audio subclasses
#define AUDIO_CONTROL_SUBCLASS   0x01
#define AUDIO_STREAMING_SUBCLASS 0x02

// class sepcific descriptor types
#define CS_INTERFACE             0x24
#define CS_ENDPOINT              0x25

// audio control descriptor subtypes
#define HEADER                   0x01
#define INPUT_TERMINAL           0x02
#define OUTPUT_TERMINAL          0x03

// audio streaming descriptor subtypes
#define AS_GENERAL               0x01
#define FORMAT_TYPE              0x02

// general audio streaming endpoint subtype
#define EP_GENERAL               0x01

// audio data format code
#define PCM8                     0x0002

typedef struct audio_control_interface_header {
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bDescriptorSubtype;
    uint16_t version;
    uint16_t wTotalLength;
    uint8_t  interfaces;
    uint8_t  interfaceNumber;
} audio_control_interface_header_t;

typedef struct audio_input_terminal_descriptor {
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bDescriptorSubtype;
    uint8_t  bTerminalID;
    uint16_t wTerminalType;
    uint8_t  bAssocTerminal;
    uint8_t  bNrChannels;
    uint16_t wChannelConfig;
    uint8_t  iChannelNames;
    uint8_t  iTerminal;
} audio_input_terminal_descriptor_t;

typedef struct audio_output_terminal_descriptor {
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bDescriptorSubtype;
    uint8_t  bTerminalID;
    uint16_t wTerminalType;
    uint8_t  bAssocTerminal;
    uint8_t  bSourceID;
    uint8_t  iTerminal;
} audio_output_terminal_descriptor_t;

typedef struct audio_streaming_interface_header {
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bDescriptorSubtype;
    uint8_t  bTerminalLink;
    uint8_t  bDelay;
    uint16_t wFormatTag;
} audio_streaming_interface_header_t;

typedef struct audio_type_i_format_descriptor {
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bDescriptorSubtype;
    uint8_t  bFormatType;
    uint8_t  bNrChannels;
    uint8_t  bSubFrameSize;
    uint8_t  bBitResolution;
    uint8_t  bSamFreqType;
    uint24_t tSamFreq;
} audio_type_i_format_descriptor_t;

typedef struct audio_isochronous_descriptor {
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bDescriptorSubtype;
    uint8_t  bmAttributes;
    uint8_t  bLockDelayUnits;
    uint16_t wLockDelay;
} audio_isochronous_descriptor_t;

void usbaudio_Play(void);
usb_error_t usbaudio_Init(void);
void usbaudio_Cleanup(void);

#endif
