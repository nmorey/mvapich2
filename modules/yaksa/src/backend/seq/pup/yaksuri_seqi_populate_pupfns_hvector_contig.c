/*
 * Copyright (C) by Argonne National Laboratory
 *     See COPYRIGHT in top-level directory
 *
 * DO NOT EDIT: AUTOMATICALLY GENERATED FILE !!
 */

#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include "yaksi.h"
#include "yaksu.h"
#include "yaksuri_seqi.h"
#include "yaksuri_seqi_populate_pupfns.h"
#include "yaksuri_seqi_pup.h"

int yaksuri_seqi_populate_pupfns_hvector_contig(yaksi_type_s * type)
{
    int rc = YAKSA_SUCCESS;
    yaksuri_seqi_type_s *seq = (yaksuri_seqi_type_s *) type->backend.seq.priv;
    
    char *str = getenv("YAKSA_ENV_MAX_NESTING_LEVEL");
    int max_nesting_level;
    if (str) {
        max_nesting_level = atoi(str);
    } else {
        max_nesting_level = YAKSI_ENV_DEFAULT_NESTING_LEVEL;
    }
    
    switch (type->u.hvector.child->u.contig.child->kind) {
        case YAKSI_TYPE_KIND__BUILTIN:
        switch (type->u.hvector.child->u.contig.child->u.builtin.handle) {
            case YAKSA_TYPE___BOOL:
            if (max_nesting_level >= 2) {
                seq->pack = yaksuri_seqi_pack_hvector_contig__Bool;
                seq->unpack = yaksuri_seqi_unpack_hvector_contig__Bool;
                seq->name = "yaksuri_seqi_op_hvector_contig__Bool";
            }
            break;
            case YAKSA_TYPE__CHAR:
            if (max_nesting_level >= 2) {
                seq->pack = yaksuri_seqi_pack_hvector_contig_char;
                seq->unpack = yaksuri_seqi_unpack_hvector_contig_char;
                seq->name = "yaksuri_seqi_op_hvector_contig_char";
            }
            break;
            case YAKSA_TYPE__WCHAR_T:
            if (max_nesting_level >= 2) {
                seq->pack = yaksuri_seqi_pack_hvector_contig_wchar_t;
                seq->unpack = yaksuri_seqi_unpack_hvector_contig_wchar_t;
                seq->name = "yaksuri_seqi_op_hvector_contig_wchar_t";
            }
            break;
            case YAKSA_TYPE__INT8_T:
            if (max_nesting_level >= 2) {
                seq->pack = yaksuri_seqi_pack_hvector_contig_int8_t;
                seq->unpack = yaksuri_seqi_unpack_hvector_contig_int8_t;
                seq->name = "yaksuri_seqi_op_hvector_contig_int8_t";
            }
            break;
            case YAKSA_TYPE__INT16_T:
            if (max_nesting_level >= 2) {
                seq->pack = yaksuri_seqi_pack_hvector_contig_int16_t;
                seq->unpack = yaksuri_seqi_unpack_hvector_contig_int16_t;
                seq->name = "yaksuri_seqi_op_hvector_contig_int16_t";
            }
            break;
            case YAKSA_TYPE__INT32_T:
            if (max_nesting_level >= 2) {
                seq->pack = yaksuri_seqi_pack_hvector_contig_int32_t;
                seq->unpack = yaksuri_seqi_unpack_hvector_contig_int32_t;
                seq->name = "yaksuri_seqi_op_hvector_contig_int32_t";
            }
            break;
            case YAKSA_TYPE__INT64_T:
            if (max_nesting_level >= 2) {
                seq->pack = yaksuri_seqi_pack_hvector_contig_int64_t;
                seq->unpack = yaksuri_seqi_unpack_hvector_contig_int64_t;
                seq->name = "yaksuri_seqi_op_hvector_contig_int64_t";
            }
            break;
            case YAKSA_TYPE__FLOAT:
            if (max_nesting_level >= 2) {
                seq->pack = yaksuri_seqi_pack_hvector_contig_float;
                seq->unpack = yaksuri_seqi_unpack_hvector_contig_float;
                seq->name = "yaksuri_seqi_op_hvector_contig_float";
            }
            break;
            case YAKSA_TYPE__DOUBLE:
            if (max_nesting_level >= 2) {
                seq->pack = yaksuri_seqi_pack_hvector_contig_double;
                seq->unpack = yaksuri_seqi_unpack_hvector_contig_double;
                seq->name = "yaksuri_seqi_op_hvector_contig_double";
            }
            break;
            case YAKSA_TYPE__LONG_DOUBLE:
            if (max_nesting_level >= 2) {
                seq->pack = yaksuri_seqi_pack_hvector_contig_long_double;
                seq->unpack = yaksuri_seqi_unpack_hvector_contig_long_double;
                seq->name = "yaksuri_seqi_op_hvector_contig_long_double";
            }
            break;
            case YAKSA_TYPE__C_COMPLEX:
            if (max_nesting_level >= 2) {
                seq->pack = yaksuri_seqi_pack_hvector_contig_c_complex;
                seq->unpack = yaksuri_seqi_unpack_hvector_contig_c_complex;
                seq->name = "yaksuri_seqi_op_hvector_contig_c_complex";
            }
            break;
            case YAKSA_TYPE__C_DOUBLE_COMPLEX:
            if (max_nesting_level >= 2) {
                seq->pack = yaksuri_seqi_pack_hvector_contig_c_double_complex;
                seq->unpack = yaksuri_seqi_unpack_hvector_contig_c_double_complex;
                seq->name = "yaksuri_seqi_op_hvector_contig_c_double_complex";
            }
            break;
            case YAKSA_TYPE__C_LONG_DOUBLE_COMPLEX:
            if (max_nesting_level >= 2) {
                seq->pack = yaksuri_seqi_pack_hvector_contig_c_long_double_complex;
                seq->unpack = yaksuri_seqi_unpack_hvector_contig_c_long_double_complex;
                seq->name = "yaksuri_seqi_op_hvector_contig_c_long_double_complex";
            }
            break;
            default:
                break;
        }
        break;
        default:
            break;
    }
    
    return rc;
}
