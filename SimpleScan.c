#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <sys/socket.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

int main(int argc, char** argv)
{
    // Buffers for reading in data
    char address_buffer[19] = { 0 };
    char name_buffer[248] = { 0 };

    // Opens a socket to the first available bluetooth adapter on this computer
    int adapter_id = hci_get_route(NULL);
    int adapter_socket = hci_open_dev(adapter_id);

    int len = 8;
    int max_responses = 255;
    int flags = IREQ_CACHE_FLUSH;

    // array of inquiry info structs
    // inquiry_info->bdaddr is the address of the device
    // inquiry_info->dev_class[3] tells what kind of device is connected (see Bluetooth Assigned Numbers)
    inquiry_info* the_inquiry_info = calloc(max_responses, sizeof(inquiry_info));

    // Perform a bluetooth device discovery and some basic info in the_inquiry_info
    int num_responses = hci_inquiry(adapter_id, len, max_responses, NULL, &the_inquiry_info, flags);
    if (num_responses < 0) perror("hci_inquiry");

    // Iterate through each response and print some basic information
    for (int i = 0; i < num_responses; i++) 
    {
        // Obtain address string
        ba2str(&(the_inquiry_info[i].bdaddr), address_buffer);

        // Obtain device name
        memset(name_buffer, 0, sizeof(name_buffer));
        int rc = hci_read_remote_name(adapter_socket,                 // int sock
                                      &(the_inquiry_info[i].bdaddr),  // const bdaddr_t* bluetooth_address
                                      sizeof(name_buffer),            // int name_buffer_size
                                      name_buffer,                    // char* name_buffer
                                      0);                             // int timeout
        
        // Mark unknown devices
        if (rc < 0) {
            strcpy(name_buffer, "[unknown]");
        }

        printf("%s %s\n", address_buffer, name_buffer);
    }

    // Free resources
    free(the_inquiry_info);
    close(adapter_socket);
    return EXIT_SUCCESS;
}
