#include "neat_property_helpers.h"
#include "neat.h"

uint8_t neat_property_translate_protocols(uint64_t propertyMask,
        int protocols[])
{
    uint8_t nr_of_protocols;

    nr_of_protocols = 0;

    /* Check for stupid settings */
    if ((propertyMask & NEAT_PROPERTY_SCTP_BANNED) &&
        (propertyMask & NEAT_PROPERTY_TCP_BANNED) &&
        (propertyMask & NEAT_PROPERTY_UDP_BANNED) &&
        (propertyMask & NEAT_PROPERTY_UDPLITE_BANNED))
        return nr_of_protocols;
    if ((propertyMask & NEAT_PROPERTY_CONGESTION_CONTROL_REQUIRED) &&
        (propertyMask & NEAT_PROPERTY_CONGESTION_CONTROL_BANNED))
        return nr_of_protocols;
    if ((propertyMask & NEAT_PROPERTY_RETRANSMISSIONS_REQUIRED) &&
        (propertyMask & NEAT_PROPERTY_RETRANSMISSIONS_BANNED))
        return nr_of_protocols;
    if ((propertyMask & NEAT_PROPERTY_SCTP_REQUIRED) &&
        (propertyMask & NEAT_PROPERTY_SCTP_BANNED))
        return nr_of_protocols;
    if ((propertyMask & NEAT_PROPERTY_TCP_REQUIRED) &&
        (propertyMask & NEAT_PROPERTY_TCP_BANNED))
        return nr_of_protocols;
    if ((propertyMask & NEAT_PROPERTY_UDP_REQUIRED) &&
        (propertyMask & NEAT_PROPERTY_UDP_BANNED))
        return nr_of_protocols;
    if ((propertyMask & NEAT_PROPERTY_UDPLITE_REQUIRED) &&
        (propertyMask & NEAT_PROPERTY_UDPLITE_BANNED))
        return nr_of_protocols;
    if ((propertyMask & NEAT_PROPERTY_TURN_REQUIRED) &&
        (propertyMask & NEAT_PROPERTY_TCP_BANNED) &&
        (propertyMask & NEAT_PROPERTY_UDP_BANNED))
        return nr_of_protocols;

    /* Check explicit protocol requests first */
    if (propertyMask & NEAT_PROPERTY_SCTP_REQUIRED) {
        if (((propertyMask & NEAT_PROPERTY_TCP_REQUIRED) == 0) &&
            ((propertyMask & NEAT_PROPERTY_UDP_REQUIRED) == 0) &&
            ((propertyMask & NEAT_PROPERTY_UDPLITE_REQUIRED) == 0) &&
            ((propertyMask & NEAT_PROPERTY_CONGESTION_CONTROL_BANNED) == 0) &&
            ((propertyMask & NEAT_PROPERTY_TURN_REQUIRED) == 0) &&
            ((propertyMask & NEAT_PROPERTY_TURN_OPTIONAL) == 0))
            protocols[nr_of_protocols++] = IPPROTO_SCTP;
        return nr_of_protocols;
    }
    if (propertyMask & NEAT_PROPERTY_TCP_REQUIRED) {
        if (((propertyMask & NEAT_PROPERTY_SCTP_REQUIRED) == 0) &&
            ((propertyMask & NEAT_PROPERTY_UDP_REQUIRED) == 0) &&
            ((propertyMask & NEAT_PROPERTY_UDPLITE_REQUIRED) == 0) &&
            ((propertyMask & NEAT_PROPERTY_MESSAGE) == 0) &&
            ((propertyMask & NEAT_PROPERTY_CONGESTION_CONTROL_BANNED) == 0) &&
            ((propertyMask & NEAT_PROPERTY_RETRANSMISSIONS_BANNED) == 0)
            ((propertyMask & NEAT_PROPERTY_TURN_REQUIRED) == 0) &&
            ((propertyMask & NEAT_PROPERTY_TURN_OPTIONAL) == 0))
            protocols[nr_of_protocols++] = IPPROTO_TCP;
        return nr_of_protocols;
    }
    if (propertyMask & NEAT_PROPERTY_UDP_REQUIRED) {
        if (((propertyMask & NEAT_PROPERTY_SCTP_REQUIRED) == 0) &&
            ((propertyMask & NEAT_PROPERTY_TCP_REQUIRED) == 0) &&
            ((propertyMask & NEAT_PROPERTY_UDPLITE_REQUIRED) == 0) &&
            ((propertyMask & NEAT_PROPERTY_CONGESTION_CONTROL_REQUIRED) == 0) &&
            ((propertyMask & NEAT_PROPERTY_RETRANSMISSIONS_REQUIRED) == 0)
            ((propertyMask & NEAT_PROPERTY_TURN_REQUIRED) == 0) &&
            ((propertyMask & NEAT_PROPERTY_TURN_OPTIONAL) == 0))
            protocols[nr_of_protocols++] = IPPROTO_UDP;
        return nr_of_protocols;
    }
    if (propertyMask & NEAT_PROPERTY_UDPLITE_REQUIRED) {
        if (((propertyMask & NEAT_PROPERTY_SCTP_REQUIRED) == 0) &&
            ((propertyMask & NEAT_PROPERTY_TCP_REQUIRED) == 0) &&
            ((propertyMask & NEAT_PROPERTY_UDP_REQUIRED) == 0) &&
            ((propertyMask & NEAT_PROPERTY_CONGESTION_CONTROL_REQUIRED) == 0) &&
            ((propertyMask & NEAT_PROPERTY_RETRANSMISSIONS_REQUIRED) == 0)
            ((propertyMask & NEAT_PROPERTY_TURN_REQUIRED) == 0) &&
            ((propertyMask & NEAT_PROPERTY_TURN_OPTIONAL) == 0))
            protocols[nr_of_protocols++] = IPPROTO_UDPLITE;
        return nr_of_protocols;
    }

    if (propertyMask & NEAT_PROPERTY_TURN_REQUIRED) {
        if (((propertyMask & NEAT_PROPERTY_TCP_BANNED) == 0) &&
            (propertyMask & NEAT_PROPERTY_UDP_BANNED)) {
            protocols[nr_of_protocols++] = IPPROTO_TURN_TCP;
            return nr_of_protocols;
        }
        if (((propertyMask & NEAT_PROPERTY_UDP_BANNED) == 0) &&
            (propertyMask & NEAT_PROPERTY_TCP_BANNED)) {
            protocols[nr_of_protocols++] = IPPROTO_TURN_UDP;
            return nr_of_protocols;
        }
    }

    /* Finally the more complex part */
    if (propertyMask & NEAT_PROPERTY_CONGESTION_CONTROL_REQUIRED) {
        if ((propertyMask & NEAT_PROPERTY_SCTP_BANNED) == 0)
            protocols[nr_of_protocols++] = IPPROTO_SCTP;
        if (((propertyMask & NEAT_PROPERTY_MESSAGE) == 0) &&
            ((propertyMask & NEAT_PROPERTY_RETRANSMISSIONS_BANNED) == 0) &&
            ((propertyMask & NEAT_PROPERTY_TCP_BANNED) == 0))
            protocols[nr_of_protocols++] = IPPROTO_TCP;
            if (propertyMask & NEAT_PROPERTY_TURN_OPTIONAL)
                protocols[nr_of_protocols++] = IPPROTO_TURN_TCP;
    } else if (propertyMask & NEAT_PROPERTY_CONGESTION_CONTROL_BANNED) {
        if ((propertyMask & NEAT_PROPERTY_RETRANSMISSIONS_REQUIRED) == 0) {
            if ((propertyMask & NEAT_PROPERTY_UDP_BANNED) == 0)
                protocols[nr_of_protocols++] = IPPROTO_UDP;
                if (propertyMask & NEAT_PROPERTY_TURN_OPTIONAL)
                    protocols[nr_of_protocols++] = IPPROTO_TURN_UDP;
            if ((propertyMask & NEAT_PROPERTY_UDPLITE_BANNED) == 0)
                protocols[nr_of_protocols++] = IPPROTO_UDPLITE;
        }
    } else {
        if ((propertyMask & NEAT_PROPERTY_SCTP_BANNED) == 0)
            protocols[nr_of_protocols++] = IPPROTO_SCTP;
        if (((propertyMask & NEAT_PROPERTY_MESSAGE) == 0) &&
            ((propertyMask & NEAT_PROPERTY_RETRANSMISSIONS_BANNED) == 0) &&
            ((propertyMask & NEAT_PROPERTY_TCP_BANNED) == 0))
            protocols[nr_of_protocols++] = IPPROTO_TCP;
            if (propertyMask & NEAT_PROPERTY_TURN_OPTIONAL)
                protocols[nr_of_protocols++] = IPPROTO_TURN_TCP;
        if ((propertyMask & NEAT_PROPERTY_RETRANSMISSIONS_REQUIRED) == 0) {
            if ((propertyMask & NEAT_PROPERTY_UDP_BANNED) == 0)
                protocols[nr_of_protocols++] = IPPROTO_UDP;
                if (propertyMask & NEAT_PROPERTY_TURN_OPTIONAL)
                    protocols[nr_of_protocols++] = IPPROTO_TURN_UDP;
            if ((propertyMask & NEAT_PROPERTY_UDPLITE_BANNED) == 0)
                protocols[nr_of_protocols++] = IPPROTO_UDPLITE;
        }
    }

    return nr_of_protocols;
}
