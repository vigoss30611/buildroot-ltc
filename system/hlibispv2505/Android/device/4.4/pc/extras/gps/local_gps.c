#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <pthread.h>
#include <cutils/properties.h>

#define  LOG_TAG  "local_gps"
#include <cutils/log.h>

#define GPS_PORT 22470

#define GPS_FIX_PROPERTY "powervr.gps.fix"

static int last_time = 0;

#define STRING_GPS_FIX "$GPGGA,%06d,%02d%02d.%04d,%c,%02d%02d.%04d,%c,1,08,,%.1g,M,0.,M,,,*47\n"

int main(int argc, char *argv[]) {
    int ssocket, main_socket;
    struct sockaddr_in srv_addr;
    long haddr;
    int i, err;

    /* Ignore SIGPIPE for the case the client closes the connection. We will
     * handle this below. */
    signal(SIGPIPE, SIG_IGN);

    // Listen for main connection
    bzero(&srv_addr, sizeof(srv_addr));
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_addr.s_addr = INADDR_ANY;
    srv_addr.sin_port = htons(GPS_PORT);

    if ((ssocket = socket(AF_INET, SOCK_STREAM, 0))<0) {
        ALOGE("Unable to create socket");
        exit(-1);
    }

    int yes = 1;
    setsockopt(ssocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

    if (bind(ssocket, (struct sockaddr *)&srv_addr, sizeof(srv_addr))<0) {
        ALOGE("Unable to bind socket, errno=%d", errno);
        exit(-1);
    }

    if (listen(ssocket, 1)<0) {
        ALOGE("Unable to listen to socket, errno=%d", errno);
        exit(-1);
    }

    main_socket=accept(ssocket, NULL, 0);
    if (main_socket<0) {
        ALOGE("Unable to accept socket for main connection, errno=%d", errno);
        exit(-1);
    }

    char str_gps_fix[PROPERTY_VALUE_MAX];
    char gps_fix[128];

    while (1) {
        property_get(GPS_FIX_PROPERTY, str_gps_fix, "51.707496 -0.440010 0.0");
        if (strlen(str_gps_fix)>0) {
            float i_lat=0, i_lng=0, i_alt=0;
            sscanf(str_gps_fix, "%f %f %f", &i_lat, &i_lng, &i_alt);

            float o_lat, o_lng, o_alt;
            int o_latdeg, o_latmin, o_lngdeg, o_lngmin;
            char o_clat, o_clng;
            if (i_lat<0) {
                o_lat = -i_lat;
                o_clat = 'S';
            }
            else {
                o_lat = i_lat;
                o_clat = 'N';
            }
            o_latdeg = (int)o_lat;
            o_lat = 60*(o_lat - o_latdeg);
            o_latmin = (int) o_lat;
            o_lat = 10000*(o_lat - o_latmin);

            if (i_lng<0) {
                o_lng = -i_lng;
                o_clng = 'W';
            }
            else {
                o_lng = i_lng;
                o_clng = 'E';
            }
            o_lngdeg = (int)o_lng;
            o_lng = 60*(o_lng - o_lngdeg);
            o_lngmin = (int) o_lng;
            o_lng = 10000*(o_lng - o_lngmin);

            sprintf(gps_fix, STRING_GPS_FIX, last_time++, o_latdeg, o_latmin, (int)o_lat, o_clat, o_lngdeg, o_lngmin, (int)o_lng, o_clng, i_alt);
            err = write(main_socket, gps_fix, strlen(gps_fix));
            if (err == -1)
            {
                if (errno == EPIPE)
                {
                    /* If the other side closed the connection, reopen it. */
                    close(main_socket);
                    main_socket=accept(ssocket, NULL, 0);
                    if (main_socket<0) {
                        ALOGE("Unable to accept socket for main connection, errno=%d", errno);
                        exit(-1);
                    }
                }else
                {
                    ALOGE("Write error, errno=%d", errno);
                    exit(-1);
                }
            }
        }
        sleep(3);
    }

    return 0;    
}
