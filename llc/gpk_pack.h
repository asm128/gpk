#include "llc_enum.h"
#include "llc_functional.h"

#ifndef LLC_PACK_H
#define LLC_PACK_H

namespace llc
{
    GDEFINE_ENUM_TYPE(PACK_TYPE, u0_t);
    GDEFINE_ENUM_VALUE(PACK_TYPE, NONE      , 0);
    GDEFINE_ENUM_VALUE(PACK_TYPE, DEFLATE   , 1);
    GDEFINE_ENUM_VALUE(PACK_TYPE, RLENGTH   , 2);
    GDEFINE_ENUM_VALUE(PACK_TYPE, INTEGER   , 3);
    GDEFINE_ENUM_VALUE(PACK_TYPE, CHARMAP   , 4);

    GDEFINE_ENUM_TYPE(PACK_FILTER, u0_t);
    GDEFINE_ENUM_VALUE(PACK_FILTER, 0, 0);
    GDEFINE_ENUM_VALUE(PACK_FILTER, 1, 1);
    GDEFINE_ENUM_VALUE(PACK_FILTER, 2, 2);
    GDEFINE_ENUM_VALUE(PACK_FILTER, 3, 3);
    GDEFINE_ENUM_VALUE(PACK_FILTER, 4, 4);

    tplTInTOut    err_t   packFilter0 (apod<TOut> & filtered, view<const TIn> unfiltered)                         { return unfiltered.for_each([&filtered](const TIn & element) { return filtered.push_back(element); } ); }
    tplTInTOut    err_t   packFilter1 (apod<TOut> & filtered, view<const TIn> unfiltered)                         { return unfiltered.for_each([&filtered](const TIn & element) { return filtered.push_back(element); } ); }
    tplTInTOut    err_t   packFilter2 (apod<TOut> & filtered, view<const TIn> unfiltered)                         { return unfiltered.for_each([&filtered](const TIn & element) { return filtered.push_back(element); } ); }
    tplTInTOut    err_t   packFilter3 (apod<TOut> & filtered, view<const TIn> unfiltered)                         { return unfiltered.for_each([&filtered](const TIn & element) { return filtered.push_back(element); } ); }
    tplTInTOut    err_t   packFilter4 (apod<TOut> & filtered, view<const TIn> unfiltered)                         { return unfiltered.for_each([&filtered](const TIn & element) { return filtered.push_back(element); } ); }
    tplTInTOut    err_t   packFilter  (apod<TOut> & filtered, view<const TIn> unfiltered, PACK_FILTER filterType) {
        switch(filterType) {
        default: llc_enum_value_error(filterType); return -1;
        case PACK_FILTER_0: return filter0(filtered, unfiltered);
        case PACK_FILTER_1: return filter1(filtered, unfiltered);
        case PACK_FILTER_2: return filter2(filtered, unfiltered);
        case PACK_FILTER_3: return filter3(filtered, unfiltered);
        case PACK_FILTER_4: return filter4(filtered, unfiltered);
        }
    }

    tplTInTOut    err_t   viewPack    (apod<TOut> & /*packed*/, view<const TIn> /*unpacked*/, PACK_TYPE /*packType*/) { return -1; }
    tplTInTOut    err_t   viewPack    (apod<TOut> & /*packed*/, view<const TIn> /*unpacked*/)                         { return -1; }
    tplTInTOut    err_t   viewUnpack  (apod<TOut> & /*packed*/, view<const TIn> /*unpacked*/, PACK_TYPE /*packType*/) { return -1; }
    tplTInTOut    err_t   viewUnpack  (apod<TOut> & /*packed*/, view<const TIn> /*unpacked*/)                         { return -1; }
} // namespace

#endif // LLC_PACK_H
