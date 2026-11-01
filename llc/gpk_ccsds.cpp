#include "llc_ccsds.h"
#include "llc_chrono.h"

using llc::CCSDS_APID, llc::savePOD;

LLC_USING_TYPEINT();

sttc llc::err_t ccsdsPacketHeaders  (llc::au0_t & output, bool telemetry, CCSDS_APID apid, u1_t family, u1_t subfamily, u1_t payloadSize) {
    {
        const struct CCSDSHeader {
            ::llc::u1_t    DataLength      = {};
            ::llc::u1_t    SequenceCount   : 14;
            ::llc::u1_t    SequenceFlags   : 2;
            ::llc::u1_t    APID            : 11; //
            ::llc::u1_t    SecondaryHeader : 1; //
            ::llc::u1_t    Type            : 1; // Telemetry (report) or Telecommand (request)
            ::llc::u1_t    Version         : 3; // Currently always 000
        }           primary     = {payloadSize, 0, 3, apid, ::llc::u1_t(payloadSize ? 1 : 0), ::llc::u1_t(telemetry ? 1 : 0), 0};
        llc::reverse<u0_t>({(u0_t*)&primary, sizeof(primary)});
        if_fail_fe(savePOD(output, primary));
    }
    if(-1 == (int16_t)family)
        return 0;
    {
        const struct CCSDSSecondaryHeader {
            ::llc::u1_t    Subfamily       : 7;
            ::llc::u1_t    Family          : 9;
        }           secondary   = {subfamily, family};
        llc::reverse<u0_t>({(u0_t*)&secondary, sizeof(secondary)});
        if_fail_fe(savePOD(output, secondary));
    }    
    return 0;
}
tplt<tpnm _tPayload>
sttc llc::err_t ccsdsPacket         (llc::au0_t & output, bool telemetry, CCSDS_APID apid, u1_t family, u1_t subfamily, const _tPayload & payload) {
    if_fail_fe(::ccsdsPacketHeaders(output,telemetry,apid,family,subfamily,(u1_t)llc::size(payload)));
    return output.append(payload);
}
llc::err_t      llc::ccsdsPacket    (au0_t & output, bool telemetry, CCSDS_APID apid, u1_t family, u1_t subfamily, vcu0_c & payload)             { return ::ccsdsPacket(output, telemetry, apid, family, subfamily, payload); }
llc::err_t      llc::ccsdsPacket    (au0_t & output, bool telemetry, CCSDS_APID apid, u1_t family, u1_t subfamily, const CCSDSPayload & payload) { return ::ccsdsPacket(output, telemetry, apid, family, subfamily, payload); }
