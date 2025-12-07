#include "usbaudio.h"
#include "soundasm.h"

#include <usbdrvce.h>
#include <tice.h>
#include <math.h>

static const usb_string_descriptor_t product_name = {
    .bLength = sizeof(product_name) + 13*2,
    .bDescriptorType = USB_STRING_DESCRIPTOR,
    .bString = L"track84 audio",
};

static const usb_string_descriptor_t *strings[] = { &product_name };
static const usb_string_descriptor_t langids = {
    .bLength = 4,
    .bDescriptorType = USB_STRING_DESCRIPTOR,
    .bString = {
        [0] = DEFAULT_LANGID,
    },
};

static struct {
    usb_configuration_descriptor_t configuration;
    usb_interface_descriptor_t interface0;
    audio_control_interface_header_t audio_header;
    audio_input_terminal_descriptor_t in_terminal;
    audio_output_terminal_descriptor_t out_terminal;
    //usb_interface_descriptor_t interface1alt0;
    usb_interface_descriptor_t interface1alt1;
    audio_streaming_interface_header_t streaming_header;
    audio_type_i_format_descriptor_t format_descriptor;
    usb_endpoint_descriptor_t endpoint0;
    audio_isochronous_descriptor_t iso_descriptor;
} configuration1 = {
    .configuration = {
        .bLength = sizeof(configuration1.configuration),
        .bDescriptorType = USB_CONFIGURATION_DESCRIPTOR,
        .wTotalLength = sizeof(configuration1),
        .bNumInterfaces = 2,
        .bConfigurationValue = 1,
        .iConfiguration = 1,
        .bmAttributes = USB_BUS_POWERED | USB_NO_REMOTE_WAKEUP,
        .bMaxPower = 500 / 2,
    },
    .interface0 = {
        .bLength = sizeof(configuration1.interface0),
        .bDescriptorType = USB_INTERFACE_DESCRIPTOR,
        .bInterfaceNumber = 0,
        .bAlternateSetting = 0,
        .bNumEndpoints = 0,
        .bInterfaceClass = USB_AUDIO_CLASS,
        .bInterfaceSubClass = AUDIO_CONTROL_SUBCLASS,
        .bInterfaceProtocol = 0,
        .iInterface = 0,
    },
    .audio_header = {
        .bLength = sizeof(configuration1.audio_header),
        .bDescriptorType = CS_INTERFACE,
        .bDescriptorSubtype = HEADER,
        .version = 0x0100,
        .wTotalLength = sizeof(configuration1.audio_header) +
                        sizeof(configuration1.in_terminal) +
                        sizeof(configuration1.out_terminal),
        .interfaces = 1,
        .interfaceNumber = 1,
    },
    .in_terminal = {
        .bLength = sizeof(configuration1.in_terminal),
        .bDescriptorType = CS_INTERFACE,
        .bDescriptorSubtype = INPUT_TERMINAL,
        .bTerminalID = 1,
        .wTerminalType = 0x0713, // (embedded) synthesizer
        .bAssocTerminal = 0,
        .bNrChannels = 1,
        .wChannelConfig = 0,
        .iChannelNames = 0,
        .iTerminal = 0,
    },
    .out_terminal = {
        .bLength = sizeof(configuration1.out_terminal),
        .bDescriptorType = CS_INTERFACE,
        .bDescriptorSubtype = OUTPUT_TERMINAL,
        .bTerminalID = 2,
        .wTerminalType = 0x0101,
        .bAssocTerminal = 0,
        .bSourceID = 1,
        .iTerminal = 0,
    },
    /*.interface1alt0 = {
        .bLength = sizeof(configuration1.interface1alt0),
        .bDescriptorType = USB_INTERFACE_DESCRIPTOR,
        .bInterfaceNumber = 1,
        .bAlternateSetting = 0,
        .bNumEndpoints = 0,
        .bInterfaceClass = USB_AUDIO_CLASS,
        .bInterfaceSubClass = AUDIO_STREAMING_SUBCLASS,
        .bInterfaceProtocol = 0,
        .iInterface = 0,
    },*/
    .interface1alt1 = {
        .bLength = sizeof(configuration1.interface1alt1),
        .bDescriptorType = USB_INTERFACE_DESCRIPTOR,
        .bInterfaceNumber = 1,
        .bAlternateSetting = 0, // was once 1
        .bNumEndpoints = 1,
        .bInterfaceClass = USB_AUDIO_CLASS,
        .bInterfaceSubClass = AUDIO_STREAMING_SUBCLASS,
        .bInterfaceProtocol = 0,
        .iInterface = 0,
    },
    .streaming_header = {
        .bLength = sizeof(configuration1.streaming_header),
        .bDescriptorType = CS_INTERFACE,
        .bDescriptorSubtype = AS_GENERAL,
        .bTerminalLink = 2,
        .bDelay = 1,
        .wFormatTag = PCM8,
    },
    .format_descriptor = {
        .bLength = sizeof(configuration1.format_descriptor),
        .bDescriptorType = CS_INTERFACE,
        .bDescriptorSubtype = FORMAT_TYPE,
        .bFormatType = 1,
        .bNrChannels = 1,
        .bSubFrameSize = 1,
        .bBitResolution = 0x08,
        .bSamFreqType = 1,
        .tSamFreq = SAMPLE_FREQ,
    },
    .endpoint0 = {
        .bLength = sizeof(configuration1.endpoint0),
        .bDescriptorType = USB_ENDPOINT_DESCRIPTOR,
        .bEndpointAddress = USB_DEVICE_TO_HOST | 1,
        .bmAttributes = USB_ISOCHRONOUS_TRANSFER,
        .wMaxPacketSize = 0x0008,
        .bInterval = 1
    },
    .iso_descriptor = {
        .bLength = sizeof(configuration1.iso_descriptor),
        .bDescriptorType = CS_ENDPOINT,
        .bDescriptorSubtype = EP_GENERAL,
        .bmAttributes = 0,
        .bLockDelayUnits = 0,
        .wLockDelay = 0,
    },
};

static const usb_configuration_descriptor_t *configurations[] = {
    &configuration1.configuration,
};

static const usb_device_descriptor_t device = {
    .bLength = sizeof(device),
    .bDescriptorType = USB_DEVICE_DESCRIPTOR,
    .bcdUSB = 0x0200,
    .bDeviceClass = 0,
    .bDeviceSubClass = 0,
    .bDeviceProtocol = 0,
    .bMaxPacketSize0 = 0x40,
    .idVendor = 0x0451,
    .idProduct = 0x1337,
    .bcdDevice = 0x0100,
    .iManufacturer = 0,
    .iProduct = 1,
    .iSerialNumber = 0,
    .bNumConfigurations = sizeof(configurations) / sizeof(*configurations),
};

static const usb_standard_descriptors_t standard = {
    .device = &device,
    .configurations = configurations,
    .langids = &langids,
    .numStrings = sizeof(strings) / sizeof(*strings),
    .strings = strings,
};

static usb_error_t handleUsbEvent(usb_event_t event, void *eventData, usb_callback_data_t *callbackData) {
    usb_error_t error = USB_SUCCESS;

    if (event == USB_DEFAULT_SETUP_EVENT) {
        if (((usb_control_setup_t *)eventData)->bRequest == USB_GET_DESCRIPTOR_REQUEST &&
            ((usb_control_setup_t *)eventData)->wValue == 0x0200 &&
            ((usb_control_setup_t *)eventData)->wLength == sizeof(configuration1)) {
            usb_Transfer(usb_GetDeviceEndpoint(usb_FindDevice(NULL, NULL, USB_SKIP_HUBS), 0), &configuration1, sizeof(configuration1), 5, NULL);
            error = USB_IGNORE;
        }
    }

    return error;
}

static usb_error_t usbaudio_Init(void) {
    return usb_Init(handleUsbEvent, NULL, &standard, USB_DEFAULT_INIT_FLAGS);
}

__attribute__((destructor)) static void usbaudio_Cleanup(void) {
    usb_Cleanup();
}

void usbaudio_Play(void) {
    uint8_t audioBlock[BLOCK_LENGTH];
    uint32_t last = 0;
    usb_endpoint_t endpoint;
    
    soundasm_Setup();
    usbaudio_Init();
    while (!os_GetCSC()) {
        usb_HandleEvents();
        if (usb_GetCycleCounter() >= last + BLOCK_CYCLES) {
            last += BLOCK_CYCLES;
            endpoint = usb_GetDeviceEndpoint(usb_FindDevice(NULL, NULL, USB_SKIP_HUBS | USB_SKIP_DISABLED), USB_DEVICE_TO_HOST | 1);
            if (endpoint) {
                for (uint8_t i = 0; i < BLOCK_LENGTH; i++) {
                    audioBlock[i] = (uint8_t)soundasm_GenerateSample() + 128;
                }
                usb_IsochronousTransfer(endpoint, &audioBlock, BLOCK_LENGTH, 1, NULL);
            }
        }
    }
    usbaudio_Cleanup();
}
