#ifndef __SPV_APP_H__
#define __SPV_APP_H__

/**
 * Message type, use to notify the gui
 **/
typedef enum {
    MSG_PIN_SUCCEED,
    MSG_VALUE_UPDATED,
    MSG_DO_ACTION
} MSG_TYPE;

typedef enum {
    ERROR_INVALID_ARGS = -100,
    ERROR_INVALID_STATUS,
    ERROR_CAMERA_BUSY,
    ERROR_SET_CONFIG,
    ERROR_CAMERA_DOWN,
    ERROR_UNKNOWN
} ERROR_TYPE;


/**
 * Init the socket server
 **/
int init_socket_server();


/**
 * Notify the app that config value has been changed
 **/
int notify_app_value_updated(char *key[], char *value[], int argc, int type);

/**
 * New pin file
 **/
int new_pin(char *pin);

/**
 * Delete the pin file
 **/
int delete_pin();


/**
 * Send message to the gui, implemented by gui
 **/
int send_message_to_gui(MSG_TYPE msg, char *key[], char *value[], int argc);

#endif //__SPV_APP_H__
