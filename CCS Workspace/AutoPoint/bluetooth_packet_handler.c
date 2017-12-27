#include "bluetooth_packet_handler.h"

/* Packet Format (for now, keeping it ASCII & fixed width)
 *
 * $[ ID x3 ]:[ data ]\n
 *
 * UTC Set
 * $1:[ year x4]:[ month x2]:[ day x2]:[ hour x2]:[ min x2]:[ sec x2]:[ ms x3]\n
 *s
 *
 */

uint32_t pkt_errors = 0;

void handle_new_packet(char* pkt, uint32_t len) {

    /* Do some basic sanity checks on the packet... */
    if(pkt[0] != '$') goto err;

    if(pkt[1] == '1') {
        /* UTC Set */
        return;
    }

    return;
    err:
        pkt_errors++;
}
