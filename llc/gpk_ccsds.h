#include "llc_enum.h"
#include "llc_apod_serialize.h"

#ifndef LLC_CCSDS_H
#define LLC_CCSDS_H

namespace llc
{
    GDEFINE_ENUM_TYPE (CCSDS_VALUE, u0_t);
    GDEFINE_ENUM_VALUE(CCSDS_VALUE, B8      , 0);
    GDEFINE_ENUM_VALUE(CCSDS_VALUE, S0      , 1);
    GDEFINE_ENUM_VALUE(CCSDS_VALUE, S1      , 2);
    GDEFINE_ENUM_VALUE(CCSDS_VALUE, S2      , 3);
    GDEFINE_ENUM_VALUE(CCSDS_VALUE, S3      , 4);
    GDEFINE_ENUM_VALUE(CCSDS_VALUE, U0      , 5);
    GDEFINE_ENUM_VALUE(CCSDS_VALUE, U1      , 6);
    GDEFINE_ENUM_VALUE(CCSDS_VALUE, U2      , 7);
    GDEFINE_ENUM_VALUE(CCSDS_VALUE, U3      , 8);
    GDEFINE_ENUM_VALUE(CCSDS_VALUE, F2      , 9);
    GDEFINE_ENUM_VALUE(CCSDS_VALUE, F3      , 0xA);
    GDEFINE_ENUM_VALUE(CCSDS_VALUE, NTSTRING, 0xB);
    GDEFINE_ENUM_VALUE(CCSDS_VALUE, ARRAY   , 0xC);

#pragma pack(push, 1)

    // The CCSDSPayloadValue constructor takes a C/C++ `fundamental type` parameter and stores its byte values and byte size such that it can be retrieved
    class CCSDSPayloadValue {
        CCSDS_VALUE             ValueType           = {};
        u3_t                    EncodedValue        = {};
        u0_t                    ValueSizeInBytes    = 0;

        tplt<tpnm _tValue>
        err_t                   AssignValue         (CCSDS_VALUE type, _tValue value) {
            ValueType   = type;
            memcpy(&EncodedValue, &value, ValueSizeInBytes = sizeof(value));
            reverse<u0_t>({(u0_t*)&EncodedValue, ValueSizeInBytes});
            return ValueSizeInBytes;
        }
    public:
                                CCSDSPayloadValue   ()                          = default;
                                CCSDSPayloadValue   (cnst CCSDSPayloadValue&)   = default;
                                CCSDSPayloadValue   (f2_t value)                { AssignValue(CCSDS_VALUE_F2, value); }
                                CCSDSPayloadValue   (f3_t value)                { AssignValue(CCSDS_VALUE_F3, value); }
                                CCSDSPayloadValue   (b8_t value)                { AssignValue(CCSDS_VALUE_B8, value); }
                                CCSDSPayloadValue   (s0_t value)                { AssignValue(CCSDS_VALUE_S0, value); }
                                CCSDSPayloadValue   (s1_t value)                { AssignValue(CCSDS_VALUE_S1, value); }
                                CCSDSPayloadValue   (s2_t value)                { AssignValue(CCSDS_VALUE_S2, value); }
                                CCSDSPayloadValue   (s3_t value)                { AssignValue(CCSDS_VALUE_S3, value); }
                                CCSDSPayloadValue   (u0_t value)                { AssignValue(CCSDS_VALUE_U0, value); }
                                CCSDSPayloadValue   (u1_t value)                { AssignValue(CCSDS_VALUE_U1, value); }
                                CCSDSPayloadValue   (u2_t value)                { AssignValue(CCSDS_VALUE_U2, value); }
                                CCSDSPayloadValue   (u3_t value)                { AssignValue(CCSDS_VALUE_U3, value); }

        inline  oper            vcu0_t              ()                  cnst    { return {(u0_c*)&ValueType, ValueSizeInBytes + 1U}; }
    };
#pragma pack(pop)

    class CCSDSPayload {
        au0_t                   EncodedPayload  = {};
        u2_t                    ValueCount      = {};
    public:
                                CCSDSPayload    ()                              = default;
                                CCSDSPayload    (const CCSDSPayload&)           = default;
        tplt<tpnm... _tArgs>
                                CCSDSPayload    (const _tArgs... values)      {
            err_t         err         = 0;
            const err_t   results[]   = {err = (((err == -1) || failed(err)) ? -1 : AppendValue(values))..., 0};
            if_fail_e(err);
        }
        inline  oper            vcu0_t          ()                                      cnst    { return EncodedPayload; }

        inline  u2_c            Size            ()                                      cnst    { return EncodedPayload.size(); }
        inline  err_t           Serialize       (au0_t & output)                        cnst    { return output.append(EncodedPayload); }

        inline  err_t           AppendValue     (const CCSDSPayloadValue & valueToAppend)       { return failed(EncodedPayload.append(valueToAppend)) ? -1 : ++ValueCount; }

        inline  err_t           AppendValue     (b8_c            & value)                       { return failed(EncodedPayload.append(CCSDSPayloadValue(value))) ? -1 : ++ValueCount; }
        inline  err_t           AppendValue     (s0_c            & value)                       { return failed(EncodedPayload.append(CCSDSPayloadValue(value))) ? -1 : ++ValueCount; }
        inline  err_t           AppendValue     (s1_c            & value)                       { return failed(EncodedPayload.append(CCSDSPayloadValue(value))) ? -1 : ++ValueCount; }
        inline  err_t           AppendValue     (s2_c            & value)                       { return failed(EncodedPayload.append(CCSDSPayloadValue(value))) ? -1 : ++ValueCount; }
        inline  err_t           AppendValue     (s3_c            & value)                       { return failed(EncodedPayload.append(CCSDSPayloadValue(value))) ? -1 : ++ValueCount; }
        inline  err_t           AppendValue     (u0_c            & value)                       { return failed(EncodedPayload.append(CCSDSPayloadValue(value))) ? -1 : ++ValueCount; }
        inline  err_t           AppendValue     (u1_c            & value)                       { return failed(EncodedPayload.append(CCSDSPayloadValue(value))) ? -1 : ++ValueCount; }
        inline  err_t           AppendValue     (u2_c            & value)                       { return failed(EncodedPayload.append(CCSDSPayloadValue(value))) ? -1 : ++ValueCount; }
        inline  err_t           AppendValue     (u3_c            & value)                       { return failed(EncodedPayload.append(CCSDSPayloadValue(value))) ? -1 : ++ValueCount; }
        inline  err_t           AppendValue     (f2_c            & value)                       { return failed(EncodedPayload.append(CCSDSPayloadValue(value))) ? -1 : ++ValueCount; }
        inline  err_t           AppendValue     (f3_c            & value)                       { return failed(EncodedPayload.append(CCSDSPayloadValue(value))) ? -1 : ++ValueCount; }
        inline  err_t           AppendValue     (cnst view<f2_c> & values)                      { for(auto value : values) if_fail_fe(AppendValue(CCSDSPayloadValue(value))); return 0; }
        inline  err_t           AppendValue     (cnst view<f3_c> & values)                      { for(auto value : values) if_fail_fe(AppendValue(CCSDSPayloadValue(value))); return 0; }
        inline  err_t           AppendValue     (vcs0_c          & values)                      { return AppendValue(view<u0_c>((u0_c*)values.begin(), values.size())); }
        err_t                   AppendValue     (vcu0_c          & values)                      {
            if_fail_fe(savePOD(EncodedPayload, CCSDS_VALUE_ARRAY));
            if_fail_fe(savePOD(EncodedPayload, (u1_t)values.size()));
            if_fail_fe(EncodedPayload.append(values));
            return ++ValueCount;
        }

    };
    
    inline  u2_t            size            (const CCSDSPayload & payload)  { return payload.Size(); }

    GDEFINE_ENUM_TYPE (CCSDS_APID, ::llc::u1_t);
    GDEFINE_ENUM_VALUE(CCSDS_APID, Telemetry    , 1);
    GDEFINE_ENUM_VALUE(CCSDS_APID, Energy       , 2);
    GDEFINE_ENUM_VALUE(CCSDS_APID, Compute      , 3);
    GDEFINE_ENUM_VALUE(CCSDS_APID, Storage      , 4);
    GDEFINE_ENUM_VALUE(CCSDS_APID, Comms        , 5);
    GDEFINE_ENUM_VALUE(CCSDS_APID, Sensors      , 6);
    GDEFINE_ENUM_VALUE(CCSDS_APID, Actuators    , 7);
    GDEFINE_ENUM_VALUE(CCSDS_APID, Idle         , 0xFFFF);

    err_t ccsdsPacket         (au0_t & output, bool telemetry, CCSDS_APID apid, u1_t family = -1, u1_t subfamily = -1, vcu0_c & payload = {});
    err_t ccsdsPacket         (au0_t & output, bool telemetry, CCSDS_APID apid, u1_t family = -1, u1_t subfamily = -1, const CCSDSPayload & payload = {});
} // namespace

#endif // LLC_CCSDS_H
