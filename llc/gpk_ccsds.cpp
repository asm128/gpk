#include "gpk_ccsds.h"
#include "gpk_chrono.h"

using gpk::CCSDS_APID, gpk::savePOD;

GPK_USING_TYPEINT();

sttc gpk::err_t ccsdsPacketHeaders  (gpk::au0_t & output, bool telemetry, CCSDS_APID apid, u1_t family, u1_t subfamily, u1_t payloadSize) {
    {
        const struct CCSDSHeader {
            ::gpk::u1_t    DataLength      = {};
            ::gpk::u1_t    SequenceCount   : 14;
            ::gpk::u1_t    SequenceFlags   : 2;
            ::gpk::u1_t    APID            : 11; //
            ::gpk::u1_t    SecondaryHeader : 1; //
            ::gpk::u1_t    Type            : 1; // Telemetry (report) or Telecommand (request)
            ::gpk::u1_t    Version         : 3; // Currently always 000
        }           primary     = {payloadSize, 0, 3, apid, ::gpk::u1_t(payloadSize ? 1 : 0), ::gpk::u1_t(telemetry ? 1 : 0), 0};
        gpk::reverse<u0_t>({(u0_t*)&primary, sizeof(primary)});
        if_fail_fe(savePOD(output, primary));
    }
    if(-1 == (int16_t)family)
        return 0;
    {
        const struct CCSDSSecondaryHeader {
            ::gpk::u1_t    Subfamily       : 7;
            ::gpk::u1_t    Family          : 9;
        }           secondary   = {subfamily, family};
        gpk::reverse<u0_t>({(u0_t*)&secondary, sizeof(secondary)});
        if_fail_fe(savePOD(output, secondary));
    }    
    return 0;
}
tplt<tpnm _tPayload>
sttc gpk::err_t ccsdsPacket         (gpk::au0_t & output, bool telemetry, CCSDS_APID apid, u1_t family, u1_t subfamily, const _tPayload & payload) {
    if_fail_fe(::ccsdsPacketHeaders(output,telemetry,apid,family,subfamily,(u1_t)gpk::size(payload)));
    return output.append(payload);
}
gpk::err_t      gpk::ccsdsPacket    (au0_t & output, bool telemetry, CCSDS_APID apid, u1_t family, u1_t subfamily, vcu0_c & payload)             { return ::ccsdsPacket(output, telemetry, apid, family, subfamily, payload); }
gpk::err_t      gpk::ccsdsPacket    (au0_t & output, bool telemetry, CCSDS_APID apid, u1_t family, u1_t subfamily, const CCSDSPayload & payload) { return ::ccsdsPacket(output, telemetry, apid, family, subfamily, payload); }


