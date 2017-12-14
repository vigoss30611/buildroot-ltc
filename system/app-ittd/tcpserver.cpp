//******************************************************************************/
//* File: tcpserver.cpp                                                        */
//******************************************************************************/

#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <printf.h>
#include "tcpserver.h"
#include <qsdk/videobox.h>
#include <stdlib.h>


// QSDK version 2.0 command mapping table.

QsdkVersion g_QsdkVersion[2] =
{
#if 0
    { //version 0
        1,
        {
            GEN_GET_ITTD_VER_CMD_VER_V0,                // GEN_GET_ITTD_VERSION.
            GEN_GET_QSDK_VER_CMD_VER_V0,                // GEN_GET_QSDK_VERSION.
            GEN_SET_DEBUG_INFO_ENABLE_CMD_VER_V0,       // GEN_SET_DEBUG_INFO_ENABLE.
            GEN_SET_JSON_FILE_CMD_VER_V0,               // GEN_SET_JSON_FILE.
            GEN_GET_JSON_FILE_CMD_VER_V0,               // GEN_GET_JSON_FILE.
            GEN_GET_ISP_FLX_FILE_CMD_VER_V0,            // GEN_GET_ISP_FLX_FILE.
            GEN_GET_ISP_RAW_FILE_CMD_VER_V0,            // GEN_GET_ISP_RAW_FILE.
            GEN_GET_ISP_YUV_FILE_CMD_VER_V0,            // GEN_GET_ISP_YUV_FILE.
            GEN_GET_ISPOST_YUV_FILE_CMD_VER_V0,         // GEN_GET_ISPOST_YUV_FILE.
            GEN_SET_ISP_SETTING_FILE_CMD_VER_V0,        // GEN_SET_ISP_SETTING_FILE.
            GEN_GET_ISP_SETTING_FILE_CMD_VER_V0,        // GEN_GET_ISP_SETTING_FILE.
            GEN_GET_ISP_SETTING_SAVE_FILE_CMD_VER_V0,   // GEN_GET_ISP_SETTING_SAVE_FILE.
            GEN_GET_GRID_DATA_FILE_CMD_VER_V0,          // GEN_GET_GRID_DATA_FILE.
            GEN_SET_GRID_DATA_FILE_CMD_VER_V0,          // GEN_SET_GRID_DATA_FILE.
            ISP_IIF_CMD_VER_V0,                         // ISP_IIF.
            ISP_BLC_CMD_VER_V0,                         // ISP_BLC.
            ISP_RLT_CMD_VER_V0,                         // ISP_RLT.
            ISP_LSH_CMD_VER_V0,                         // ISP_LSH.
            ISP_DNS_CMD_VER_V0,                         // ISP_DNS.
            ISP_DPF_CMD_VER_V0,                         // ISP_DPF.
            ISP_LCA_CMD_VER_V0,                         // ISP_LCA.
            ISP_GMA_CMD_VER_V0,                         // ISP_GMA.
            ISP_WBS_CMD_VER_V0,                         // ISP_WBS.
            ISP_HIS_CMD_VER_V0,                         // ISP_HIS.
            ISP_R2Y_CMD_VER_V0,                         // ISP_R2Y.
            ISP_TNM_CMD_VER_V0,                         // ISP_TNM.
            ISP_SHA_CMD_VER_V0,                         // ISP_SHA.
            ISP_WBC_CCM_CMD_VER_V0,                     // ISP_WBC_CCM.
            ISP_AEC_CMD_VER_V0,                         // ISP_AEC.
            ISP_AWB_CMD_VER_V0,                         // ISP_AWB.
            ISP_CMC_CMD_VER_V0,                         // ISP_CMC.
            ISP_TNMC_CMD_VER_V0,                        // ISP_TNMC.
            ISP_IIC_CMD_VER_V0,                         // ISP_IIC.
            ISP_VER_CMD_VER_V0,                         // ISP_VER.
            ISP_DBG_CMD_VER_V0,                         // ISP_DBG.
            ISPOST_DN_CMD_VER_V0,                       // ISPOST_DN.
            ISPOST_VER_CMD_VER_V0,                      // ISPOST_VER.
            ISPOST_DBG_CMD_VER_V0,                      // ISPOST_DBG.
            ISPOST_GRID_PATH_CMD_VER_V0,                // ISPOST_GRID_PATH.
            H26X_ENC_CMD_VER_V0,                        // H26X_ENC.
            H26X_VER_CMD_VER_V0,                        // H26X_VER.
            H26X_DBG_CMD_VER_V0,                        // H26X_DBG.
            NOT_SUPPORT_CMD,                            // 42
            NOT_SUPPORT_CMD,                            // 43
            NOT_SUPPORT_CMD,                            // 44
            NOT_SUPPORT_CMD,                            // 45
            NOT_SUPPORT_CMD,                            // 46
            NOT_SUPPORT_CMD,                            // 47
            NOT_SUPPORT_CMD,                            // 48
            NOT_SUPPORT_CMD,                            // 49
            NOT_SUPPORT_CMD,                            // 50
            NOT_SUPPORT_CMD,                            // 51
            NOT_SUPPORT_CMD,                            // 52
            NOT_SUPPORT_CMD,                            // 53
            NOT_SUPPORT_CMD,                            // 54
            NOT_SUPPORT_CMD,                            // 55
            NOT_SUPPORT_CMD,                            // 56
            NOT_SUPPORT_CMD,                            // 57
            NOT_SUPPORT_CMD,                            // 58
            NOT_SUPPORT_CMD,                            // 59
            NOT_SUPPORT_CMD,                            // 60
            NOT_SUPPORT_CMD,                            // 61
            NOT_SUPPORT_CMD,                            // 62
            NOT_SUPPORT_CMD,                            // 63
            NOT_SUPPORT_CMD,                            // 64
            NOT_SUPPORT_CMD,                            // 65
            NOT_SUPPORT_CMD,                            // 66
            NOT_SUPPORT_CMD,                            // 67
            NOT_SUPPORT_CMD,                            // 68
            NOT_SUPPORT_CMD,                            // 69
            NOT_SUPPORT_CMD,                            // 70
            NOT_SUPPORT_CMD,                            // 71
            NOT_SUPPORT_CMD,                            // 72
            NOT_SUPPORT_CMD,                            // 73
            NOT_SUPPORT_CMD,                            // 74
            NOT_SUPPORT_CMD,                            // 75
            NOT_SUPPORT_CMD,                            // 76
            NOT_SUPPORT_CMD,                            // 77
            NOT_SUPPORT_CMD,                            // 78
            NOT_SUPPORT_CMD,                            // 79
            NOT_SUPPORT_CMD,                            // 80
            NOT_SUPPORT_CMD,                            // 81
            NOT_SUPPORT_CMD,                            // 82
            NOT_SUPPORT_CMD,                            // 83
            NOT_SUPPORT_CMD,                            // 84
            NOT_SUPPORT_CMD,                            // 85
            NOT_SUPPORT_CMD,                            // 86
            NOT_SUPPORT_CMD,                            // 87
            NOT_SUPPORT_CMD,                            // 88
            NOT_SUPPORT_CMD,                            // 89
            NOT_SUPPORT_CMD,                            // 90
            NOT_SUPPORT_CMD,                            // 91
            NOT_SUPPORT_CMD,                            // 92
            NOT_SUPPORT_CMD,                            // 93
            NOT_SUPPORT_CMD,                            // 94
            NOT_SUPPORT_CMD,                            // 95
            NOT_SUPPORT_CMD,                            // 96
            NOT_SUPPORT_CMD,                            // 97
            NOT_SUPPORT_CMD,                            // 98
            NOT_SUPPORT_CMD,                            // 99
            NOT_SUPPORT_CMD,                            // 100
            NOT_SUPPORT_CMD,                            // 101
            NOT_SUPPORT_CMD,                            // 102
            NOT_SUPPORT_CMD,                            // 103
            NOT_SUPPORT_CMD,                            // 104
            NOT_SUPPORT_CMD,                            // 105
            NOT_SUPPORT_CMD,                            // 106
            NOT_SUPPORT_CMD,                            // 107
            NOT_SUPPORT_CMD,                            // 108
            NOT_SUPPORT_CMD,                            // 109
            NOT_SUPPORT_CMD,                            // 110
            NOT_SUPPORT_CMD,                            // 111
            NOT_SUPPORT_CMD,                            // 112
            NOT_SUPPORT_CMD,                            // 113
            NOT_SUPPORT_CMD,                            // 114
            NOT_SUPPORT_CMD,                            // 115
            NOT_SUPPORT_CMD,                            // 116
            NOT_SUPPORT_CMD,                            // 117
            NOT_SUPPORT_CMD,                            // 118
            NOT_SUPPORT_CMD,                            // 119
            NOT_SUPPORT_CMD,                            // 120
            NOT_SUPPORT_CMD,                            // 121
            NOT_SUPPORT_CMD,                            // 122
            NOT_SUPPORT_CMD,                            // 123
            NOT_SUPPORT_CMD,                            // 124
            NOT_SUPPORT_CMD,                            // 125
            NOT_SUPPORT_CMD,                            // 126
            NOT_SUPPORT_CMD,                            // 127
            NOT_SUPPORT_CMD,                            // 128
            NOT_SUPPORT_CMD,                            // 129
            NOT_SUPPORT_CMD,                            // 130
            NOT_SUPPORT_CMD,                            // 131
            NOT_SUPPORT_CMD,                            // 132
            NOT_SUPPORT_CMD,                            // 133
            NOT_SUPPORT_CMD,                            // 134
            NOT_SUPPORT_CMD,                            // 135
            NOT_SUPPORT_CMD,                            // 136
            NOT_SUPPORT_CMD,                            // 137
            NOT_SUPPORT_CMD,                            // 138
            NOT_SUPPORT_CMD,                            // 139
            NOT_SUPPORT_CMD,                            // 140
            NOT_SUPPORT_CMD,                            // 141
            NOT_SUPPORT_CMD,                            // 142
            NOT_SUPPORT_CMD,                            // 143
            NOT_SUPPORT_CMD,                            // 144
            NOT_SUPPORT_CMD,                            // 145
            NOT_SUPPORT_CMD,                            // 146
            NOT_SUPPORT_CMD,                            // 147
            NOT_SUPPORT_CMD,                            // 148
            NOT_SUPPORT_CMD,                            // 149
            NOT_SUPPORT_CMD,                            // 150
            NOT_SUPPORT_CMD,                            // 151
            NOT_SUPPORT_CMD,                            // 152
            NOT_SUPPORT_CMD,                            // 153
            NOT_SUPPORT_CMD,                            // 154
            NOT_SUPPORT_CMD,                            // 155
            NOT_SUPPORT_CMD,                            // 156
            NOT_SUPPORT_CMD,                            // 157
            NOT_SUPPORT_CMD,                            // 158
            NOT_SUPPORT_CMD,                            // 159
            NOT_SUPPORT_CMD,                            // 160
            NOT_SUPPORT_CMD,                            // 161
            NOT_SUPPORT_CMD,                            // 162
            NOT_SUPPORT_CMD,                            // 163
            NOT_SUPPORT_CMD,                            // 164
            NOT_SUPPORT_CMD,                            // 165
            NOT_SUPPORT_CMD,                            // 166
            NOT_SUPPORT_CMD,                            // 167
            NOT_SUPPORT_CMD,                            // 168
            NOT_SUPPORT_CMD,                            // 169
            NOT_SUPPORT_CMD,                            // 170
            NOT_SUPPORT_CMD,                            // 171
            NOT_SUPPORT_CMD,                            // 172
            NOT_SUPPORT_CMD,                            // 173
            NOT_SUPPORT_CMD,                            // 174
            NOT_SUPPORT_CMD,                            // 175
            NOT_SUPPORT_CMD,                            // 176
            NOT_SUPPORT_CMD,                            // 177
            NOT_SUPPORT_CMD,                            // 178
            NOT_SUPPORT_CMD,                            // 179
            NOT_SUPPORT_CMD,                            // 180
            NOT_SUPPORT_CMD,                            // 181
            NOT_SUPPORT_CMD,                            // 182
            NOT_SUPPORT_CMD,                            // 183
            NOT_SUPPORT_CMD,                            // 184
            NOT_SUPPORT_CMD,                            // 185
            NOT_SUPPORT_CMD,                            // 186
            NOT_SUPPORT_CMD,                            // 187
            NOT_SUPPORT_CMD,                            // 188
            NOT_SUPPORT_CMD,                            // 189
            NOT_SUPPORT_CMD,                            // 190
            NOT_SUPPORT_CMD,                            // 191
            NOT_SUPPORT_CMD,                            // 192
            NOT_SUPPORT_CMD,                            // 193
            NOT_SUPPORT_CMD,                            // 194
            NOT_SUPPORT_CMD,                            // 195
            NOT_SUPPORT_CMD,                            // 196
            NOT_SUPPORT_CMD,                            // 197
            NOT_SUPPORT_CMD,                            // 198
            NOT_SUPPORT_CMD,                            // 199
            NOT_SUPPORT_CMD,                            // 200
            NOT_SUPPORT_CMD,                            // 201
            NOT_SUPPORT_CMD,                            // 202
            NOT_SUPPORT_CMD,                            // 203
            NOT_SUPPORT_CMD,                            // 204
            NOT_SUPPORT_CMD,                            // 205
            NOT_SUPPORT_CMD,                            // 206
            NOT_SUPPORT_CMD,                            // 207
            NOT_SUPPORT_CMD,                            // 208
            NOT_SUPPORT_CMD,                            // 209
            NOT_SUPPORT_CMD,                            // 210
            NOT_SUPPORT_CMD,                            // 211
            NOT_SUPPORT_CMD,                            // 212
            NOT_SUPPORT_CMD,                            // 213
            NOT_SUPPORT_CMD,                            // 214
            NOT_SUPPORT_CMD,                            // 215
            NOT_SUPPORT_CMD,                            // 216
            NOT_SUPPORT_CMD,                            // 217
            NOT_SUPPORT_CMD,                            // 218
            NOT_SUPPORT_CMD,                            // 219
            NOT_SUPPORT_CMD,                            // 220
            NOT_SUPPORT_CMD,                            // 221
            NOT_SUPPORT_CMD,                            // 222
            NOT_SUPPORT_CMD,                            // 223
            NOT_SUPPORT_CMD,                            // 224
            NOT_SUPPORT_CMD,                            // 225
            NOT_SUPPORT_CMD,                            // 226
            NOT_SUPPORT_CMD,                            // 227
            NOT_SUPPORT_CMD,                            // 228
            NOT_SUPPORT_CMD,                            // 229
            NOT_SUPPORT_CMD,                            // 230
            NOT_SUPPORT_CMD,                            // 231
            NOT_SUPPORT_CMD,                            // 232
            NOT_SUPPORT_CMD,                            // 233
            NOT_SUPPORT_CMD,                            // 234
            NOT_SUPPORT_CMD,                            // 235
            NOT_SUPPORT_CMD,                            // 236
            NOT_SUPPORT_CMD,                            // 237
            NOT_SUPPORT_CMD,                            // 238
            NOT_SUPPORT_CMD,                            // 239
            NOT_SUPPORT_CMD,                            // 240
            NOT_SUPPORT_CMD,                            // 241
            NOT_SUPPORT_CMD,                            // 242
            NOT_SUPPORT_CMD,                            // 243
            NOT_SUPPORT_CMD,                            // 244
            NOT_SUPPORT_CMD,                            // 245
            NOT_SUPPORT_CMD,                            // 246
            NOT_SUPPORT_CMD,                            // 247
            NOT_SUPPORT_CMD,                            // 248
            NOT_SUPPORT_CMD,                            // 249
            NOT_SUPPORT_CMD,                            // 250
            NOT_SUPPORT_CMD,                            // 251
            NOT_SUPPORT_CMD,                            // 252
            NOT_SUPPORT_CMD,                            // 253
            NOT_SUPPORT_CMD,                            // 254
            NOT_SUPPORT_CMD,                            // 255
        },
    },
#elif 0
    { //version 1
        2,
        {
            GEN_GET_ITTD_VER_CMD_VER_V0,                // GEN_GET_ITTD_VERSION.
            GEN_GET_QSDK_VER_CMD_VER_V0,                // GEN_GET_QSDK_VERSION.
            GEN_SET_DEBUG_INFO_ENABLE_CMD_VER_V0,       // GEN_SET_DEBUG_INFO_ENABLE.
            GEN_SET_JSON_FILE_CMD_VER_V0,               // GEN_SET_JSON_FILE.
            GEN_GET_JSON_FILE_CMD_VER_V0,               // GEN_GET_JSON_FILE.
            GEN_GET_ISP_FLX_FILE_CMD_VER_V0,            // GEN_GET_ISP_FLX_FILE.
            GEN_GET_ISP_RAW_FILE_CMD_VER_V0,            // GEN_GET_ISP_RAW_FILE.
            GEN_GET_ISP_YUV_FILE_CMD_VER_V0,            // GEN_GET_ISP_YUV_FILE.
            GEN_GET_ISPOST_YUV_FILE_CMD_VER_V0,         // GEN_GET_ISPOST_YUV_FILE.
            GEN_SET_ISP_SETTING_FILE_CMD_VER_V0,        // GEN_SET_ISP_SETTING_FILE.
            GEN_GET_ISP_SETTING_FILE_CMD_VER_V0,        // GEN_GET_ISP_SETTING_FILE.
            GEN_GET_ISP_SETTING_SAVE_FILE_CMD_VER_V0,   // GEN_GET_ISP_SETTING_SAVE_FILE.
            GEN_GET_GRID_DATA_FILE_CMD_VER_V0,          // GEN_GET_GRID_DATA_FILE.
            GEN_SET_GRID_DATA_FILE_CMD_VER_V0,          // GEN_SET_GRID_DATA_FILE.
            ISP_IIF_CMD_VER_V0,                         // ISP_IIF.
            ISP_BLC_CMD_VER_V0,                         // ISP_BLC.
            ISP_RLT_CMD_VER_V0,                         // ISP_RLT.
            ISP_LSH_CMD_VER_V0,                         // ISP_LSH.
            ISP_DNS_CMD_VER_V0,                         // ISP_DNS.
            ISP_DPF_CMD_VER_V0,                         // ISP_DPF.
            ISP_LCA_CMD_VER_V0,                         // ISP_LCA.
            ISP_GMA_CMD_VER_V0,                         // ISP_GMA.
            ISP_WBS_CMD_VER_V0,                         // ISP_WBS.
            ISP_HIS_CMD_VER_V0,                         // ISP_HIS.
            ISP_R2Y_CMD_VER_V0,                         // ISP_R2Y.
            ISP_TNM_CMD_VER_V0,                         // ISP_TNM.
            ISP_SHA_CMD_VER_V0,                         // ISP_SHA.
            ISP_WBC_CCM_CMD_VER_V0,                     // ISP_WBC_CCM.
            ISP_AEC_CMD_VER_V0,                         // ISP_AEC.
            ISP_AWB_CMD_VER_V0,                         // ISP_AWB.
            ISP_CMC_CMD_VER_V0,                         // ISP_CMC.
            ISP_TNMC_CMD_VER_V0,                        // ISP_TNMC.
            ISP_IIC_CMD_VER_V0,                         // ISP_IIC.
            ISP_VER_CMD_VER_V0,                         // ISP_VER.
            ISP_DBG_CMD_VER_V0,                         // ISP_DBG.
            ISPOST_DN_CMD_VER_V0,                       // ISPOST_DN.
            ISPOST_VER_CMD_VER_V0,                      // ISPOST_VER.
            ISPOST_DBG_CMD_VER_V0,                      // ISPOST_DBG.
            ISPOST_GRID_PATH_CMD_VER_V0,                // ISPOST_GRID_PATH.
            H26X_ENC_CMD_VER_V0,                        // H26X_ENC.
            H26X_VER_CMD_VER_V0,                        // H26X_VER.
            H26X_DBG_CMD_VER_V0,                        // H26X_DBG.
            ISP_OUT_CMD_VER_V0,                         // ISP_OUT.
            NOT_SUPPORT_CMD,                            // 43
            NOT_SUPPORT_CMD,                            // 44
            NOT_SUPPORT_CMD,                            // 45
            NOT_SUPPORT_CMD,                            // 46
            NOT_SUPPORT_CMD,                            // 47
            NOT_SUPPORT_CMD,                            // 48
            NOT_SUPPORT_CMD,                            // 49
            NOT_SUPPORT_CMD,                            // 50
            NOT_SUPPORT_CMD,                            // 51
            NOT_SUPPORT_CMD,                            // 52
            NOT_SUPPORT_CMD,                            // 53
            NOT_SUPPORT_CMD,                            // 54
            NOT_SUPPORT_CMD,                            // 55
            NOT_SUPPORT_CMD,                            // 56
            NOT_SUPPORT_CMD,                            // 57
            NOT_SUPPORT_CMD,                            // 58
            NOT_SUPPORT_CMD,                            // 59
            NOT_SUPPORT_CMD,                            // 60
            NOT_SUPPORT_CMD,                            // 61
            NOT_SUPPORT_CMD,                            // 62
            NOT_SUPPORT_CMD,                            // 63
            NOT_SUPPORT_CMD,                            // 64
            NOT_SUPPORT_CMD,                            // 65
            NOT_SUPPORT_CMD,                            // 66
            NOT_SUPPORT_CMD,                            // 67
            NOT_SUPPORT_CMD,                            // 68
            NOT_SUPPORT_CMD,                            // 69
            NOT_SUPPORT_CMD,                            // 70
            NOT_SUPPORT_CMD,                            // 71
            NOT_SUPPORT_CMD,                            // 72
            NOT_SUPPORT_CMD,                            // 73
            NOT_SUPPORT_CMD,                            // 74
            NOT_SUPPORT_CMD,                            // 75
            NOT_SUPPORT_CMD,                            // 76
            NOT_SUPPORT_CMD,                            // 77
            NOT_SUPPORT_CMD,                            // 78
            NOT_SUPPORT_CMD,                            // 79
            NOT_SUPPORT_CMD,                            // 80
            NOT_SUPPORT_CMD,                            // 81
            NOT_SUPPORT_CMD,                            // 82
            NOT_SUPPORT_CMD,                            // 83
            NOT_SUPPORT_CMD,                            // 84
            NOT_SUPPORT_CMD,                            // 85
            NOT_SUPPORT_CMD,                            // 86
            NOT_SUPPORT_CMD,                            // 87
            NOT_SUPPORT_CMD,                            // 88
            NOT_SUPPORT_CMD,                            // 89
            NOT_SUPPORT_CMD,                            // 90
            NOT_SUPPORT_CMD,                            // 91
            NOT_SUPPORT_CMD,                            // 92
            NOT_SUPPORT_CMD,                            // 93
            NOT_SUPPORT_CMD,                            // 94
            NOT_SUPPORT_CMD,                            // 95
            NOT_SUPPORT_CMD,                            // 96
            NOT_SUPPORT_CMD,                            // 97
            NOT_SUPPORT_CMD,                            // 98
            NOT_SUPPORT_CMD,                            // 99
            NOT_SUPPORT_CMD,                            // 100
            NOT_SUPPORT_CMD,                            // 101
            NOT_SUPPORT_CMD,                            // 102
            NOT_SUPPORT_CMD,                            // 103
            NOT_SUPPORT_CMD,                            // 104
            NOT_SUPPORT_CMD,                            // 105
            NOT_SUPPORT_CMD,                            // 106
            NOT_SUPPORT_CMD,                            // 107
            NOT_SUPPORT_CMD,                            // 108
            NOT_SUPPORT_CMD,                            // 109
            NOT_SUPPORT_CMD,                            // 110
            NOT_SUPPORT_CMD,                            // 111
            NOT_SUPPORT_CMD,                            // 112
            NOT_SUPPORT_CMD,                            // 113
            NOT_SUPPORT_CMD,                            // 114
            NOT_SUPPORT_CMD,                            // 115
            NOT_SUPPORT_CMD,                            // 116
            NOT_SUPPORT_CMD,                            // 117
            NOT_SUPPORT_CMD,                            // 118
            NOT_SUPPORT_CMD,                            // 119
            NOT_SUPPORT_CMD,                            // 120
            NOT_SUPPORT_CMD,                            // 121
            NOT_SUPPORT_CMD,                            // 122
            NOT_SUPPORT_CMD,                            // 123
            NOT_SUPPORT_CMD,                            // 124
            NOT_SUPPORT_CMD,                            // 125
            NOT_SUPPORT_CMD,                            // 126
            NOT_SUPPORT_CMD,                            // 127
            NOT_SUPPORT_CMD,                            // 128
            NOT_SUPPORT_CMD,                            // 129
            NOT_SUPPORT_CMD,                            // 130
            NOT_SUPPORT_CMD,                            // 131
            NOT_SUPPORT_CMD,                            // 132
            NOT_SUPPORT_CMD,                            // 133
            NOT_SUPPORT_CMD,                            // 134
            NOT_SUPPORT_CMD,                            // 135
            NOT_SUPPORT_CMD,                            // 136
            NOT_SUPPORT_CMD,                            // 137
            NOT_SUPPORT_CMD,                            // 138
            NOT_SUPPORT_CMD,                            // 139
            NOT_SUPPORT_CMD,                            // 140
            NOT_SUPPORT_CMD,                            // 141
            NOT_SUPPORT_CMD,                            // 142
            NOT_SUPPORT_CMD,                            // 143
            NOT_SUPPORT_CMD,                            // 144
            NOT_SUPPORT_CMD,                            // 145
            NOT_SUPPORT_CMD,                            // 146
            NOT_SUPPORT_CMD,                            // 147
            NOT_SUPPORT_CMD,                            // 148
            NOT_SUPPORT_CMD,                            // 149
            NOT_SUPPORT_CMD,                            // 150
            NOT_SUPPORT_CMD,                            // 151
            NOT_SUPPORT_CMD,                            // 152
            NOT_SUPPORT_CMD,                            // 153
            NOT_SUPPORT_CMD,                            // 154
            NOT_SUPPORT_CMD,                            // 155
            NOT_SUPPORT_CMD,                            // 156
            NOT_SUPPORT_CMD,                            // 157
            NOT_SUPPORT_CMD,                            // 158
            NOT_SUPPORT_CMD,                            // 159
            NOT_SUPPORT_CMD,                            // 160
            NOT_SUPPORT_CMD,                            // 161
            NOT_SUPPORT_CMD,                            // 162
            NOT_SUPPORT_CMD,                            // 163
            NOT_SUPPORT_CMD,                            // 164
            NOT_SUPPORT_CMD,                            // 165
            NOT_SUPPORT_CMD,                            // 166
            NOT_SUPPORT_CMD,                            // 167
            NOT_SUPPORT_CMD,                            // 168
            NOT_SUPPORT_CMD,                            // 169
            NOT_SUPPORT_CMD,                            // 170
            NOT_SUPPORT_CMD,                            // 171
            NOT_SUPPORT_CMD,                            // 172
            NOT_SUPPORT_CMD,                            // 173
            NOT_SUPPORT_CMD,                            // 174
            NOT_SUPPORT_CMD,                            // 175
            NOT_SUPPORT_CMD,                            // 176
            NOT_SUPPORT_CMD,                            // 177
            NOT_SUPPORT_CMD,                            // 178
            NOT_SUPPORT_CMD,                            // 179
            NOT_SUPPORT_CMD,                            // 180
            NOT_SUPPORT_CMD,                            // 181
            NOT_SUPPORT_CMD,                            // 182
            NOT_SUPPORT_CMD,                            // 183
            NOT_SUPPORT_CMD,                            // 184
            NOT_SUPPORT_CMD,                            // 185
            NOT_SUPPORT_CMD,                            // 186
            NOT_SUPPORT_CMD,                            // 187
            NOT_SUPPORT_CMD,                            // 188
            NOT_SUPPORT_CMD,                            // 189
            NOT_SUPPORT_CMD,                            // 190
            NOT_SUPPORT_CMD,                            // 191
            NOT_SUPPORT_CMD,                            // 192
            NOT_SUPPORT_CMD,                            // 193
            NOT_SUPPORT_CMD,                            // 194
            NOT_SUPPORT_CMD,                            // 195
            NOT_SUPPORT_CMD,                            // 196
            NOT_SUPPORT_CMD,                            // 197
            NOT_SUPPORT_CMD,                            // 198
            NOT_SUPPORT_CMD,                            // 199
            NOT_SUPPORT_CMD,                            // 200
            NOT_SUPPORT_CMD,                            // 201
            NOT_SUPPORT_CMD,                            // 202
            NOT_SUPPORT_CMD,                            // 203
            NOT_SUPPORT_CMD,                            // 204
            NOT_SUPPORT_CMD,                            // 205
            NOT_SUPPORT_CMD,                            // 206
            NOT_SUPPORT_CMD,                            // 207
            NOT_SUPPORT_CMD,                            // 208
            NOT_SUPPORT_CMD,                            // 209
            NOT_SUPPORT_CMD,                            // 210
            NOT_SUPPORT_CMD,                            // 211
            NOT_SUPPORT_CMD,                            // 212
            NOT_SUPPORT_CMD,                            // 213
            NOT_SUPPORT_CMD,                            // 214
            NOT_SUPPORT_CMD,                            // 215
            NOT_SUPPORT_CMD,                            // 216
            NOT_SUPPORT_CMD,                            // 217
            NOT_SUPPORT_CMD,                            // 218
            NOT_SUPPORT_CMD,                            // 219
            NOT_SUPPORT_CMD,                            // 220
            NOT_SUPPORT_CMD,                            // 221
            NOT_SUPPORT_CMD,                            // 222
            NOT_SUPPORT_CMD,                            // 223
            NOT_SUPPORT_CMD,                            // 224
            NOT_SUPPORT_CMD,                            // 225
            NOT_SUPPORT_CMD,                            // 226
            NOT_SUPPORT_CMD,                            // 227
            NOT_SUPPORT_CMD,                            // 228
            NOT_SUPPORT_CMD,                            // 229
            NOT_SUPPORT_CMD,                            // 230
            NOT_SUPPORT_CMD,                            // 231
            NOT_SUPPORT_CMD,                            // 232
            NOT_SUPPORT_CMD,                            // 233
            NOT_SUPPORT_CMD,                            // 234
            NOT_SUPPORT_CMD,                            // 235
            NOT_SUPPORT_CMD,                            // 236
            NOT_SUPPORT_CMD,                            // 237
            NOT_SUPPORT_CMD,                            // 238
            NOT_SUPPORT_CMD,                            // 239
            NOT_SUPPORT_CMD,                            // 240
            NOT_SUPPORT_CMD,                            // 241
            NOT_SUPPORT_CMD,                            // 242
            NOT_SUPPORT_CMD,                            // 243
            NOT_SUPPORT_CMD,                            // 244
            NOT_SUPPORT_CMD,                            // 245
            NOT_SUPPORT_CMD,                            // 246
            NOT_SUPPORT_CMD,                            // 247
            NOT_SUPPORT_CMD,                            // 248
            NOT_SUPPORT_CMD,                            // 249
            NOT_SUPPORT_CMD,                            // 250
            NOT_SUPPORT_CMD,                            // 251
            NOT_SUPPORT_CMD,                            // 252
            NOT_SUPPORT_CMD,                            // 253
            NOT_SUPPORT_CMD,                            // 254
            NOT_SUPPORT_CMD,                            // 255
        },
    },
#elif 0
    { //version 2
        3,
        {
            GEN_GET_ITTD_VER_CMD_VER_V0,                // GEN_GET_ITTD_VERSION.
            GEN_GET_QSDK_VER_CMD_VER_V0,                // GEN_GET_QSDK_VERSION.
            GEN_SET_DEBUG_INFO_ENABLE_CMD_VER_V0,       // GEN_SET_DEBUG_INFO_ENABLE.
            GEN_SET_JSON_FILE_CMD_VER_V0,               // GEN_SET_JSON_FILE.
            GEN_GET_JSON_FILE_CMD_VER_V0,               // GEN_GET_JSON_FILE.
            GEN_GET_ISP_FLX_FILE_CMD_VER_V0,            // GEN_GET_ISP_FLX_FILE.
            GEN_GET_ISP_RAW_FILE_CMD_VER_V0,            // GEN_GET_ISP_RAW_FILE.
            GEN_GET_ISP_YUV_FILE_CMD_VER_V0,            // GEN_GET_ISP_YUV_FILE.
            GEN_GET_ISPOST_YUV_FILE_CMD_VER_V0,         // GEN_GET_ISPOST_YUV_FILE.
            GEN_SET_ISP_SETTING_FILE_CMD_VER_V0,        // GEN_SET_ISP_SETTING_FILE.
            GEN_GET_ISP_SETTING_FILE_CMD_VER_V0,        // GEN_GET_ISP_SETTING_FILE.
            GEN_GET_ISP_SETTING_SAVE_FILE_CMD_VER_V0,   // GEN_GET_ISP_SETTING_SAVE_FILE.
            GEN_GET_GRID_DATA_FILE_CMD_VER_V0,          // GEN_GET_GRID_DATA_FILE.
            GEN_SET_GRID_DATA_FILE_CMD_VER_V0,          // GEN_SET_GRID_DATA_FILE.
            ISP_IIF_CMD_VER_V0,                         // ISP_IIF.
            ISP_BLC_CMD_VER_V0,                         // ISP_BLC.
            ISP_RLT_CMD_VER_V0,                         // ISP_RLT.
            ISP_LSH_CMD_VER_V0,                         // ISP_LSH.
            ISP_DNS_CMD_VER_V0,                         // ISP_DNS.
            ISP_DPF_CMD_VER_V0,                         // ISP_DPF.
            ISP_LCA_CMD_VER_V0,                         // ISP_LCA.
            ISP_GMA_CMD_VER_V0,                         // ISP_GMA.
            ISP_WBS_CMD_VER_V0,                         // ISP_WBS.
            ISP_HIS_CMD_VER_V0,                         // ISP_HIS.
            ISP_R2Y_CMD_VER_V0,                         // ISP_R2Y.
            ISP_TNM_CMD_VER_V0,                         // ISP_TNM.
            ISP_SHA_CMD_VER_V0,                         // ISP_SHA.
            ISP_WBC_CCM_CMD_VER_V0,                     // ISP_WBC_CCM.
            ISP_AEC_CMD_VER_V0,                         // ISP_AEC.
            ISP_AWB_CMD_VER_V0,                         // ISP_AWB.
            ISP_CMC_CMD_VER_V0,                         // ISP_CMC.
            ISP_TNMC_CMD_VER_V0,                        // ISP_TNMC.
            ISP_IIC_CMD_VER_V0,                         // ISP_IIC.
            ISP_VER_CMD_VER_V0,                         // ISP_VER.
            ISP_DBG_CMD_VER_V0,                         // ISP_DBG.
            ISPOST_DN_CMD_VER_V0,                       // ISPOST_DN.
            ISPOST_VER_CMD_VER_V0,                      // ISPOST_VER.
            ISPOST_DBG_CMD_VER_V0,                      // ISPOST_DBG.
            ISPOST_GRID_PATH_CMD_VER_V0,                // ISPOST_GRID_PATH.
            H26X_ENC_CMD_VER_V0,                        // H26X_ENC.
            H26X_VER_CMD_VER_V0,                        // H26X_VER.
            H26X_DBG_CMD_VER_V0,                        // H26X_DBG.
            ISP_OUT_CMD_VER_V0,                         // ISP_OUT.
            GEN_GET_ISP_LSH_FILE_CMD_VER_V0,            // GEN_GET_ISP_LSH_FILE
            GEN_SET_ISP_LSH_FILE_CMD_VER_V0,            // GEN_SET_ISP_LSH_FILE
            GEN_SET_VIDEOBOX_CTRL_CMD_VER_V0,           // GEN_SET_VIDEOBOX_CTRL
            GEN_GET_FILE_CMD_VER_V0,                    // GEN_GET_FILE
            GEN_SET_FILE_CMD_VER_V0,                    // GEN_SET_FILE
            NOT_SUPPORT_CMD,                            // 48
            NOT_SUPPORT_CMD,                            // 49
            NOT_SUPPORT_CMD,                            // 50
            NOT_SUPPORT_CMD,                            // 51
            NOT_SUPPORT_CMD,                            // 52
            NOT_SUPPORT_CMD,                            // 53
            NOT_SUPPORT_CMD,                            // 54
            NOT_SUPPORT_CMD,                            // 55
            NOT_SUPPORT_CMD,                            // 56
            NOT_SUPPORT_CMD,                            // 57
            NOT_SUPPORT_CMD,                            // 58
            NOT_SUPPORT_CMD,                            // 59
            NOT_SUPPORT_CMD,                            // 60
            NOT_SUPPORT_CMD,                            // 61
            NOT_SUPPORT_CMD,                            // 62
            NOT_SUPPORT_CMD,                            // 63
            NOT_SUPPORT_CMD,                            // 64
            NOT_SUPPORT_CMD,                            // 65
            NOT_SUPPORT_CMD,                            // 66
            NOT_SUPPORT_CMD,                            // 67
            NOT_SUPPORT_CMD,                            // 68
            NOT_SUPPORT_CMD,                            // 69
            NOT_SUPPORT_CMD,                            // 70
            NOT_SUPPORT_CMD,                            // 71
            NOT_SUPPORT_CMD,                            // 72
            NOT_SUPPORT_CMD,                            // 73
            NOT_SUPPORT_CMD,                            // 74
            NOT_SUPPORT_CMD,                            // 75
            NOT_SUPPORT_CMD,                            // 76
            NOT_SUPPORT_CMD,                            // 77
            NOT_SUPPORT_CMD,                            // 78
            NOT_SUPPORT_CMD,                            // 79
            NOT_SUPPORT_CMD,                            // 80
            NOT_SUPPORT_CMD,                            // 81
            NOT_SUPPORT_CMD,                            // 82
            NOT_SUPPORT_CMD,                            // 83
            NOT_SUPPORT_CMD,                            // 84
            NOT_SUPPORT_CMD,                            // 85
            NOT_SUPPORT_CMD,                            // 86
            NOT_SUPPORT_CMD,                            // 87
            NOT_SUPPORT_CMD,                            // 88
            NOT_SUPPORT_CMD,                            // 89
            NOT_SUPPORT_CMD,                            // 90
            NOT_SUPPORT_CMD,                            // 91
            NOT_SUPPORT_CMD,                            // 92
            NOT_SUPPORT_CMD,                            // 93
            NOT_SUPPORT_CMD,                            // 94
            NOT_SUPPORT_CMD,                            // 95
            NOT_SUPPORT_CMD,                            // 96
            NOT_SUPPORT_CMD,                            // 97
            NOT_SUPPORT_CMD,                            // 98
            NOT_SUPPORT_CMD,                            // 99
            NOT_SUPPORT_CMD,                            // 100
            NOT_SUPPORT_CMD,                            // 101
            NOT_SUPPORT_CMD,                            // 102
            NOT_SUPPORT_CMD,                            // 103
            NOT_SUPPORT_CMD,                            // 104
            NOT_SUPPORT_CMD,                            // 105
            NOT_SUPPORT_CMD,                            // 106
            NOT_SUPPORT_CMD,                            // 107
            NOT_SUPPORT_CMD,                            // 108
            NOT_SUPPORT_CMD,                            // 109
            NOT_SUPPORT_CMD,                            // 110
            NOT_SUPPORT_CMD,                            // 111
            NOT_SUPPORT_CMD,                            // 112
            NOT_SUPPORT_CMD,                            // 113
            NOT_SUPPORT_CMD,                            // 114
            NOT_SUPPORT_CMD,                            // 115
            NOT_SUPPORT_CMD,                            // 116
            NOT_SUPPORT_CMD,                            // 117
            NOT_SUPPORT_CMD,                            // 118
            NOT_SUPPORT_CMD,                            // 119
            NOT_SUPPORT_CMD,                            // 120
            NOT_SUPPORT_CMD,                            // 121
            NOT_SUPPORT_CMD,                            // 122
            NOT_SUPPORT_CMD,                            // 123
            NOT_SUPPORT_CMD,                            // 124
            NOT_SUPPORT_CMD,                            // 125
            NOT_SUPPORT_CMD,                            // 126
            NOT_SUPPORT_CMD,                            // 127
            NOT_SUPPORT_CMD,                            // 128
            NOT_SUPPORT_CMD,                            // 129
            NOT_SUPPORT_CMD,                            // 130
            NOT_SUPPORT_CMD,                            // 131
            NOT_SUPPORT_CMD,                            // 132
            NOT_SUPPORT_CMD,                            // 133
            NOT_SUPPORT_CMD,                            // 134
            NOT_SUPPORT_CMD,                            // 135
            NOT_SUPPORT_CMD,                            // 136
            NOT_SUPPORT_CMD,                            // 137
            NOT_SUPPORT_CMD,                            // 138
            NOT_SUPPORT_CMD,                            // 139
            NOT_SUPPORT_CMD,                            // 140
            NOT_SUPPORT_CMD,                            // 141
            NOT_SUPPORT_CMD,                            // 142
            NOT_SUPPORT_CMD,                            // 143
            NOT_SUPPORT_CMD,                            // 144
            NOT_SUPPORT_CMD,                            // 145
            NOT_SUPPORT_CMD,                            // 146
            NOT_SUPPORT_CMD,                            // 147
            NOT_SUPPORT_CMD,                            // 148
            NOT_SUPPORT_CMD,                            // 149
            NOT_SUPPORT_CMD,                            // 150
            NOT_SUPPORT_CMD,                            // 151
            NOT_SUPPORT_CMD,                            // 152
            NOT_SUPPORT_CMD,                            // 153
            NOT_SUPPORT_CMD,                            // 154
            NOT_SUPPORT_CMD,                            // 155
            NOT_SUPPORT_CMD,                            // 156
            NOT_SUPPORT_CMD,                            // 157
            NOT_SUPPORT_CMD,                            // 158
            NOT_SUPPORT_CMD,                            // 159
            NOT_SUPPORT_CMD,                            // 160
            NOT_SUPPORT_CMD,                            // 161
            NOT_SUPPORT_CMD,                            // 162
            NOT_SUPPORT_CMD,                            // 163
            NOT_SUPPORT_CMD,                            // 164
            NOT_SUPPORT_CMD,                            // 165
            NOT_SUPPORT_CMD,                            // 166
            NOT_SUPPORT_CMD,                            // 167
            NOT_SUPPORT_CMD,                            // 168
            NOT_SUPPORT_CMD,                            // 169
            NOT_SUPPORT_CMD,                            // 170
            NOT_SUPPORT_CMD,                            // 171
            NOT_SUPPORT_CMD,                            // 172
            NOT_SUPPORT_CMD,                            // 173
            NOT_SUPPORT_CMD,                            // 174
            NOT_SUPPORT_CMD,                            // 175
            NOT_SUPPORT_CMD,                            // 176
            NOT_SUPPORT_CMD,                            // 177
            NOT_SUPPORT_CMD,                            // 178
            NOT_SUPPORT_CMD,                            // 179
            NOT_SUPPORT_CMD,                            // 180
            NOT_SUPPORT_CMD,                            // 181
            NOT_SUPPORT_CMD,                            // 182
            NOT_SUPPORT_CMD,                            // 183
            NOT_SUPPORT_CMD,                            // 184
            NOT_SUPPORT_CMD,                            // 185
            NOT_SUPPORT_CMD,                            // 186
            NOT_SUPPORT_CMD,                            // 187
            NOT_SUPPORT_CMD,                            // 188
            NOT_SUPPORT_CMD,                            // 189
            NOT_SUPPORT_CMD,                            // 190
            NOT_SUPPORT_CMD,                            // 191
            NOT_SUPPORT_CMD,                            // 192
            NOT_SUPPORT_CMD,                            // 193
            NOT_SUPPORT_CMD,                            // 194
            NOT_SUPPORT_CMD,                            // 195
            NOT_SUPPORT_CMD,                            // 196
            NOT_SUPPORT_CMD,                            // 197
            NOT_SUPPORT_CMD,                            // 198
            NOT_SUPPORT_CMD,                            // 199
            NOT_SUPPORT_CMD,                            // 200
            NOT_SUPPORT_CMD,                            // 201
            NOT_SUPPORT_CMD,                            // 202
            NOT_SUPPORT_CMD,                            // 203
            NOT_SUPPORT_CMD,                            // 204
            NOT_SUPPORT_CMD,                            // 205
            NOT_SUPPORT_CMD,                            // 206
            NOT_SUPPORT_CMD,                            // 207
            NOT_SUPPORT_CMD,                            // 208
            NOT_SUPPORT_CMD,                            // 209
            NOT_SUPPORT_CMD,                            // 210
            NOT_SUPPORT_CMD,                            // 211
            NOT_SUPPORT_CMD,                            // 212
            NOT_SUPPORT_CMD,                            // 213
            NOT_SUPPORT_CMD,                            // 214
            NOT_SUPPORT_CMD,                            // 215
            NOT_SUPPORT_CMD,                            // 216
            NOT_SUPPORT_CMD,                            // 217
            NOT_SUPPORT_CMD,                            // 218
            NOT_SUPPORT_CMD,                            // 219
            NOT_SUPPORT_CMD,                            // 220
            NOT_SUPPORT_CMD,                            // 221
            NOT_SUPPORT_CMD,                            // 222
            NOT_SUPPORT_CMD,                            // 223
            NOT_SUPPORT_CMD,                            // 224
            NOT_SUPPORT_CMD,                            // 225
            NOT_SUPPORT_CMD,                            // 226
            NOT_SUPPORT_CMD,                            // 227
            NOT_SUPPORT_CMD,                            // 228
            NOT_SUPPORT_CMD,                            // 229
            NOT_SUPPORT_CMD,                            // 230
            NOT_SUPPORT_CMD,                            // 231
            NOT_SUPPORT_CMD,                            // 232
            NOT_SUPPORT_CMD,                            // 233
            NOT_SUPPORT_CMD,                            // 234
            NOT_SUPPORT_CMD,                            // 235
            NOT_SUPPORT_CMD,                            // 236
            NOT_SUPPORT_CMD,                            // 237
            NOT_SUPPORT_CMD,                            // 238
            NOT_SUPPORT_CMD,                            // 239
            NOT_SUPPORT_CMD,                            // 240
            NOT_SUPPORT_CMD,                            // 241
            NOT_SUPPORT_CMD,                            // 242
            NOT_SUPPORT_CMD,                            // 243
            NOT_SUPPORT_CMD,                            // 244
            NOT_SUPPORT_CMD,                            // 245
            NOT_SUPPORT_CMD,                            // 246
            NOT_SUPPORT_CMD,                            // 247
            NOT_SUPPORT_CMD,                            // 248
            NOT_SUPPORT_CMD,                            // 249
            NOT_SUPPORT_CMD,                            // 250
            NOT_SUPPORT_CMD,                            // 251
            NOT_SUPPORT_CMD,                            // 252
            NOT_SUPPORT_CMD,                            // 253
            NOT_SUPPORT_CMD,                            // 254
            NOT_SUPPORT_CMD,                            // 255
        },
    },
#else
    { //version 3
        4,
        {
            GEN_GET_ITTD_VER_CMD_VER_V0,                // GEN_GET_ITTD_VERSION.
            GEN_GET_QSDK_VER_CMD_VER_V0,                // GEN_GET_QSDK_VERSION.
            GEN_SET_DEBUG_INFO_ENABLE_CMD_VER_V0,       // GEN_SET_DEBUG_INFO_ENABLE.
            GEN_SET_JSON_FILE_CMD_VER_V0,               // GEN_SET_JSON_FILE.
            GEN_GET_JSON_FILE_CMD_VER_V0,               // GEN_GET_JSON_FILE.
            GEN_GET_ISP_FLX_FILE_CMD_VER_V0,            // GEN_GET_ISP_FLX_FILE.
            GEN_GET_ISP_RAW_FILE_CMD_VER_V0,            // GEN_GET_ISP_RAW_FILE.
            GEN_GET_ISP_YUV_FILE_CMD_VER_V0,            // GEN_GET_ISP_YUV_FILE.
            GEN_GET_ISPOST_YUV_FILE_CMD_VER_V0,         // GEN_GET_ISPOST_YUV_FILE.
            GEN_SET_ISP_SETTING_FILE_CMD_VER_V0,        // GEN_SET_ISP_SETTING_FILE.
            GEN_GET_ISP_SETTING_FILE_CMD_VER_V0,        // GEN_GET_ISP_SETTING_FILE.
            GEN_GET_ISP_SETTING_SAVE_FILE_CMD_VER_V0,   // GEN_GET_ISP_SETTING_SAVE_FILE.
            GEN_GET_GRID_DATA_FILE_CMD_VER_V0,          // GEN_GET_GRID_DATA_FILE.
            GEN_SET_GRID_DATA_FILE_CMD_VER_V0,          // GEN_SET_GRID_DATA_FILE.
            ISP_IIF_CMD_VER_V0,                         // ISP_IIF.
            ISP_BLC_CMD_VER_V0,                         // ISP_BLC.
            ISP_RLT_CMD_VER_V0,                         // ISP_RLT.
            ISP_LSH_CMD_VER_V0,                         // ISP_LSH.
            ISP_DNS_CMD_VER_V0,                         // ISP_DNS.
            ISP_DPF_CMD_VER_V0,                         // ISP_DPF.
            ISP_LCA_CMD_VER_V0,                         // ISP_LCA.
            ISP_GMA_CMD_VER_V0,                         // ISP_GMA.
            ISP_WBS_CMD_VER_V0,                         // ISP_WBS.
            ISP_HIS_CMD_VER_V0,                         // ISP_HIS.
            ISP_R2Y_CMD_VER_V0,                         // ISP_R2Y.
            ISP_TNM_CMD_VER_V0,                         // ISP_TNM.
            ISP_SHA_CMD_VER_V0,                         // ISP_SHA.
            ISP_WBC_CCM_CMD_VER_V0,                     // ISP_WBC_CCM.
            ISP_AEC_CMD_VER_V0,                         // ISP_AEC.
            ISP_AWB_CMD_VER_V0,                         // ISP_AWB.
            ISP_CMC_CMD_VER_V0,                         // ISP_CMC.
            ISP_TNMC_CMD_VER_V0,                        // ISP_TNMC.
            ISP_IIC_CMD_VER_V0,                         // ISP_IIC.
            ISP_VER_CMD_VER_V0,                         // ISP_VER.
            ISP_DBG_CMD_VER_V0,                         // ISP_DBG.
            ISPOST_DN_CMD_VER_V0,                       // ISPOST_DN.
            ISPOST_VER_CMD_VER_V0,                      // ISPOST_VER.
            ISPOST_DBG_CMD_VER_V0,                      // ISPOST_DBG.
            ISPOST_GRID_PATH_CMD_VER_V0,                // ISPOST_GRID_PATH.
            H26X_ENC_CMD_VER_V0,                        // H26X_ENC.
            H26X_VER_CMD_VER_V0,                        // H26X_VER.
            H26X_DBG_CMD_VER_V0,                        // H26X_DBG.
            ISP_OUT_CMD_VER_V0,                         // ISP_OUT.
            GEN_GET_ISP_LSH_FILE_CMD_VER_V0,            // GEN_GET_ISP_LSH_FILE
            GEN_SET_ISP_LSH_FILE_CMD_VER_V0,            // GEN_SET_ISP_LSH_FILE
            GEN_SET_VIDEOBOX_CTRL_CMD_VER_V0,           // GEN_SET_VIDEOBOX_CTRL
            GEN_GET_FILE_CMD_VER_V0,                    // GEN_GET_FILE
            GEN_SET_FILE_CMD_VER_V0,                    // GEN_SET_FILE
            GEN_SET_REMOUNT_DEV_PARTITION_CMD_VER_V0,   // GEN_SET_REMOUNT_DEV_PARTITION
            NOT_SUPPORT_CMD,                            // 49
            NOT_SUPPORT_CMD,                            // 50
            NOT_SUPPORT_CMD,                            // 51
            NOT_SUPPORT_CMD,                            // 52
            NOT_SUPPORT_CMD,                            // 53
            NOT_SUPPORT_CMD,                            // 54
            NOT_SUPPORT_CMD,                            // 55
            NOT_SUPPORT_CMD,                            // 56
            NOT_SUPPORT_CMD,                            // 57
            NOT_SUPPORT_CMD,                            // 58
            NOT_SUPPORT_CMD,                            // 59
            NOT_SUPPORT_CMD,                            // 60
            NOT_SUPPORT_CMD,                            // 61
            NOT_SUPPORT_CMD,                            // 62
            NOT_SUPPORT_CMD,                            // 63
            NOT_SUPPORT_CMD,                            // 64
            NOT_SUPPORT_CMD,                            // 65
            NOT_SUPPORT_CMD,                            // 66
            NOT_SUPPORT_CMD,                            // 67
            NOT_SUPPORT_CMD,                            // 68
            NOT_SUPPORT_CMD,                            // 69
            NOT_SUPPORT_CMD,                            // 70
            NOT_SUPPORT_CMD,                            // 71
            NOT_SUPPORT_CMD,                            // 72
            NOT_SUPPORT_CMD,                            // 73
            NOT_SUPPORT_CMD,                            // 74
            NOT_SUPPORT_CMD,                            // 75
            NOT_SUPPORT_CMD,                            // 76
            NOT_SUPPORT_CMD,                            // 77
            NOT_SUPPORT_CMD,                            // 78
            NOT_SUPPORT_CMD,                            // 79
            NOT_SUPPORT_CMD,                            // 80
            NOT_SUPPORT_CMD,                            // 81
            NOT_SUPPORT_CMD,                            // 82
            NOT_SUPPORT_CMD,                            // 83
            NOT_SUPPORT_CMD,                            // 84
            NOT_SUPPORT_CMD,                            // 85
            NOT_SUPPORT_CMD,                            // 86
            NOT_SUPPORT_CMD,                            // 87
            NOT_SUPPORT_CMD,                            // 88
            NOT_SUPPORT_CMD,                            // 89
            NOT_SUPPORT_CMD,                            // 90
            NOT_SUPPORT_CMD,                            // 91
            NOT_SUPPORT_CMD,                            // 92
            NOT_SUPPORT_CMD,                            // 93
            NOT_SUPPORT_CMD,                            // 94
            NOT_SUPPORT_CMD,                            // 95
            NOT_SUPPORT_CMD,                            // 96
            NOT_SUPPORT_CMD,                            // 97
            NOT_SUPPORT_CMD,                            // 98
            NOT_SUPPORT_CMD,                            // 99
            NOT_SUPPORT_CMD,                            // 100
            NOT_SUPPORT_CMD,                            // 101
            NOT_SUPPORT_CMD,                            // 102
            NOT_SUPPORT_CMD,                            // 103
            NOT_SUPPORT_CMD,                            // 104
            NOT_SUPPORT_CMD,                            // 105
            NOT_SUPPORT_CMD,                            // 106
            NOT_SUPPORT_CMD,                            // 107
            NOT_SUPPORT_CMD,                            // 108
            NOT_SUPPORT_CMD,                            // 109
            NOT_SUPPORT_CMD,                            // 110
            NOT_SUPPORT_CMD,                            // 111
            NOT_SUPPORT_CMD,                            // 112
            NOT_SUPPORT_CMD,                            // 113
            NOT_SUPPORT_CMD,                            // 114
            NOT_SUPPORT_CMD,                            // 115
            NOT_SUPPORT_CMD,                            // 116
            NOT_SUPPORT_CMD,                            // 117
            NOT_SUPPORT_CMD,                            // 118
            NOT_SUPPORT_CMD,                            // 119
            NOT_SUPPORT_CMD,                            // 120
            NOT_SUPPORT_CMD,                            // 121
            NOT_SUPPORT_CMD,                            // 122
            NOT_SUPPORT_CMD,                            // 123
            NOT_SUPPORT_CMD,                            // 124
            NOT_SUPPORT_CMD,                            // 125
            NOT_SUPPORT_CMD,                            // 126
            NOT_SUPPORT_CMD,                            // 127
            NOT_SUPPORT_CMD,                            // 128
            NOT_SUPPORT_CMD,                            // 129
            NOT_SUPPORT_CMD,                            // 130
            NOT_SUPPORT_CMD,                            // 131
            NOT_SUPPORT_CMD,                            // 132
            NOT_SUPPORT_CMD,                            // 133
            NOT_SUPPORT_CMD,                            // 134
            NOT_SUPPORT_CMD,                            // 135
            NOT_SUPPORT_CMD,                            // 136
            NOT_SUPPORT_CMD,                            // 137
            NOT_SUPPORT_CMD,                            // 138
            NOT_SUPPORT_CMD,                            // 139
            NOT_SUPPORT_CMD,                            // 140
            NOT_SUPPORT_CMD,                            // 141
            NOT_SUPPORT_CMD,                            // 142
            NOT_SUPPORT_CMD,                            // 143
            NOT_SUPPORT_CMD,                            // 144
            NOT_SUPPORT_CMD,                            // 145
            NOT_SUPPORT_CMD,                            // 146
            NOT_SUPPORT_CMD,                            // 147
            NOT_SUPPORT_CMD,                            // 148
            NOT_SUPPORT_CMD,                            // 149
            NOT_SUPPORT_CMD,                            // 150
            NOT_SUPPORT_CMD,                            // 151
            NOT_SUPPORT_CMD,                            // 152
            NOT_SUPPORT_CMD,                            // 153
            NOT_SUPPORT_CMD,                            // 154
            NOT_SUPPORT_CMD,                            // 155
            NOT_SUPPORT_CMD,                            // 156
            NOT_SUPPORT_CMD,                            // 157
            NOT_SUPPORT_CMD,                            // 158
            NOT_SUPPORT_CMD,                            // 159
            NOT_SUPPORT_CMD,                            // 160
            NOT_SUPPORT_CMD,                            // 161
            NOT_SUPPORT_CMD,                            // 162
            NOT_SUPPORT_CMD,                            // 163
            NOT_SUPPORT_CMD,                            // 164
            NOT_SUPPORT_CMD,                            // 165
            NOT_SUPPORT_CMD,                            // 166
            NOT_SUPPORT_CMD,                            // 167
            NOT_SUPPORT_CMD,                            // 168
            NOT_SUPPORT_CMD,                            // 169
            NOT_SUPPORT_CMD,                            // 170
            NOT_SUPPORT_CMD,                            // 171
            NOT_SUPPORT_CMD,                            // 172
            NOT_SUPPORT_CMD,                            // 173
            NOT_SUPPORT_CMD,                            // 174
            NOT_SUPPORT_CMD,                            // 175
            NOT_SUPPORT_CMD,                            // 176
            NOT_SUPPORT_CMD,                            // 177
            NOT_SUPPORT_CMD,                            // 178
            NOT_SUPPORT_CMD,                            // 179
            NOT_SUPPORT_CMD,                            // 180
            NOT_SUPPORT_CMD,                            // 181
            NOT_SUPPORT_CMD,                            // 182
            NOT_SUPPORT_CMD,                            // 183
            NOT_SUPPORT_CMD,                            // 184
            NOT_SUPPORT_CMD,                            // 185
            NOT_SUPPORT_CMD,                            // 186
            NOT_SUPPORT_CMD,                            // 187
            NOT_SUPPORT_CMD,                            // 188
            NOT_SUPPORT_CMD,                            // 189
            NOT_SUPPORT_CMD,                            // 190
            NOT_SUPPORT_CMD,                            // 191
            NOT_SUPPORT_CMD,                            // 192
            NOT_SUPPORT_CMD,                            // 193
            NOT_SUPPORT_CMD,                            // 194
            NOT_SUPPORT_CMD,                            // 195
            NOT_SUPPORT_CMD,                            // 196
            NOT_SUPPORT_CMD,                            // 197
            NOT_SUPPORT_CMD,                            // 198
            NOT_SUPPORT_CMD,                            // 199
            NOT_SUPPORT_CMD,                            // 200
            NOT_SUPPORT_CMD,                            // 201
            NOT_SUPPORT_CMD,                            // 202
            NOT_SUPPORT_CMD,                            // 203
            NOT_SUPPORT_CMD,                            // 204
            NOT_SUPPORT_CMD,                            // 205
            NOT_SUPPORT_CMD,                            // 206
            NOT_SUPPORT_CMD,                            // 207
            NOT_SUPPORT_CMD,                            // 208
            NOT_SUPPORT_CMD,                            // 209
            NOT_SUPPORT_CMD,                            // 210
            NOT_SUPPORT_CMD,                            // 211
            NOT_SUPPORT_CMD,                            // 212
            NOT_SUPPORT_CMD,                            // 213
            NOT_SUPPORT_CMD,                            // 214
            NOT_SUPPORT_CMD,                            // 215
            NOT_SUPPORT_CMD,                            // 216
            NOT_SUPPORT_CMD,                            // 217
            NOT_SUPPORT_CMD,                            // 218
            NOT_SUPPORT_CMD,                            // 219
            NOT_SUPPORT_CMD,                            // 220
            NOT_SUPPORT_CMD,                            // 221
            NOT_SUPPORT_CMD,                            // 222
            NOT_SUPPORT_CMD,                            // 223
            NOT_SUPPORT_CMD,                            // 224
            NOT_SUPPORT_CMD,                            // 225
            NOT_SUPPORT_CMD,                            // 226
            NOT_SUPPORT_CMD,                            // 227
            NOT_SUPPORT_CMD,                            // 228
            NOT_SUPPORT_CMD,                            // 229
            NOT_SUPPORT_CMD,                            // 230
            NOT_SUPPORT_CMD,                            // 231
            NOT_SUPPORT_CMD,                            // 232
            NOT_SUPPORT_CMD,                            // 233
            NOT_SUPPORT_CMD,                            // 234
            NOT_SUPPORT_CMD,                            // 235
            NOT_SUPPORT_CMD,                            // 236
            NOT_SUPPORT_CMD,                            // 237
            NOT_SUPPORT_CMD,                            // 238
            NOT_SUPPORT_CMD,                            // 239
            NOT_SUPPORT_CMD,                            // 240
            NOT_SUPPORT_CMD,                            // 241
            NOT_SUPPORT_CMD,                            // 242
            NOT_SUPPORT_CMD,                            // 243
            NOT_SUPPORT_CMD,                            // 244
            NOT_SUPPORT_CMD,                            // 245
            NOT_SUPPORT_CMD,                            // 246
            NOT_SUPPORT_CMD,                            // 247
            NOT_SUPPORT_CMD,                            // 248
            NOT_SUPPORT_CMD,                            // 249
            NOT_SUPPORT_CMD,                            // 250
            NOT_SUPPORT_CMD,                            // 251
            NOT_SUPPORT_CMD,                            // 252
            NOT_SUPPORT_CMD,                            // 253
            NOT_SUPPORT_CMD,                            // 254
            NOT_SUPPORT_CMD,                            // 255
        },
    },
#endif
};


std::vector<TCPServiceThread*> m_cServiceThreads;

//#define ITTD_DEBUG		1

//******************************************************************************/
//* TCPServer Class.                                                           */
//******************************************************************************/
TCPServer::TCPServer(int iPortNumber, int iBacklog, int iMaxThreads)
{
	m_iMaxThreads = iMaxThreads;
    m_pcListeningSocket = new TCPServerSocket(iPortNumber, iBacklog, false);
    //printf("TCPServer Listen(%d)\n", m_pcListeningSocket->GetSockDesc());
}

TCPServer::~TCPServer()
{
    //printf("~TCPServer() call\n");
    //m_condWakeThread.LockMutEx();
    m_bServerTerminate = true;
    //m_condWakeThread.Signal();
    //m_condWakeThread.UnlockMutEx();

    if (m_pcListeningSocket)
    	delete m_pcListeningSocket;

    //printf("TCPServer WaitThread()\n");
    WaitThread();
    //printf("TCPServer WaitThread() OK\n");
}

void TCPServer::Start()
{
    m_bServerTerminate = false;
    m_iThreadCount = 0;

    LaunchThread();
}

void TCPServer::Stop()
{
	std::vector<TCPServiceThread*>::iterator pThraed;
	TCPServiceThread* pcServiceThread;

	for (pThraed = m_cServiceThreads.begin(); pThraed != m_cServiceThreads.end(); pThraed++)
	{
		(*pThraed)->Stop();
	}

	while (!m_cServiceThreads.empty())
	{
		pcServiceThread = (TCPServiceThread*)m_cServiceThreads.back();
		m_cServiceThreads.pop_back();
		if (pcServiceThread)
			delete pcServiceThread;
	}

    m_bServerTerminate = true;
}

void* TCPServer::Initialise()
{

    return NULL;
}

void* TCPServer::Execute()
{
	int iRetVal;
	SocketSet mySet;
	struct timeval timeout;
	TCPSocket* pcClientSocket;
	TCPServiceThread* pcServiceThread;

    //m_condWakeThread.LockMutEx();
    while (!m_bServerTerminate)
    {
		do
		{
			timeout.tv_sec = 0;
			timeout.tv_usec = 10;
			mySet.Clear();
			mySet += (SocketBase *)m_pcListeningSocket;
			iRetVal = select((int) mySet + 1, mySet, NULL, NULL, &timeout);
			//if (iRetVal == -1)
			//	throw errClientError;
		} while((iRetVal==0) && (!m_bServerTerminate));

		if (mySet.IsMember((SocketBase*)m_pcListeningSocket))
		{
			pcClientSocket = AcceptClient();
			if (pcClientSocket)
			{
				m_iThreadCount++;
				pcServiceThread = new TCPServiceThread(pcClientSocket, m_iThreadCount, this);
				if (pcServiceThread)
				{
					m_cServiceThreads.push_back(pcServiceThread);
					//printf("Thread created - Active Threads(%d), size: %d\n", m_iThreadCount, m_cServiceThreads.size());
					DumpClient();
				}
			}
		}
    }

    //m_condWakeThread.UnlockMutEx();
	//printf("TCPServer Execute end - ThreadCount: %d\n", m_iThreadCount);

    return NULL;
}

TCPSocket* TCPServer::AcceptClient()
{
    m_mutexAccept.Lock();

	try
	{
		TCPSocket   *pcClientSocket;

		pcClientSocket = m_pcListeningSocket->AcceptClient();
		m_mutexAccept.Unlock();
		return pcClientSocket;
	}
	catch (SocketException)
	{
	}

    m_mutexAccept.Unlock();
    return NULL;
    //throw errServerDown;
}

void TCPServer::DeleteClient(TCPServiceThread* pcServiceThread)
{
	std::vector<TCPServiceThread*>::iterator pThraed;

	for (pThraed = m_cServiceThreads.begin(); pThraed != m_cServiceThreads.end(); pThraed++)
	{
		if ((*pThraed)->GetThreadID() == pcServiceThread->GetThreadID())
		{
			//printf("Delete ThraedID: %d\n", (*pThraed)->GetThreadID());
			(*pThraed)->Stop();
			m_cServiceThreads.erase(pThraed);
			break;
		}
	}

	DumpClient();
}

void TCPServer::DumpClient()
{
	std::vector<TCPServiceThread*>::iterator pThraed;
	TCPSocket* pcSocket;

	printf("Client count: %d\n", m_cServiceThreads.size());
	printf("------+-------------------------------------------------\n");

	for (pThraed = m_cServiceThreads.begin(); pThraed != m_cServiceThreads.end(); pThraed++)
	{
		pcSocket = (*pThraed)->GetSocket();
		printf(" %04d | %s:%d\n",
				(*pThraed)->GetThreadID(),
				(pcSocket->RemoteIPAddress()).GetAddressString(),
				pcSocket->RemotePortNumber());
	}
	printf("------+-------------------------------------------------\n");
}

//******************************************************************************/
//* TCPServiceThread Class.                                                    */
//******************************************************************************/
TCPServiceThread::TCPServiceThread(TCPSocket* pcClientSocket, int iThreadID, TCPServer* pcServer) : ThreadBase()
{
	m_pcClientSocket = pcClientSocket;
	m_iThreadID = iThreadID;
	m_bIttdDebugInfoEnable = false;
	m_bH26XDebugInfoEnable = false;
	Start();
}

TCPServiceThread::~TCPServiceThread()
{
    //printf("~TCPServiceThread() call\n");

	m_bServiceTerminate = true;
    //printf("TCPServiceThread WaitThread()\n");
    WaitThread();
    //printf("TCPServiceThread WaitThread() OK\n");
}

void* TCPServiceThread::Initialise()
{
	return NULL;
}

void TCPServiceThread::Start()
{
	m_bServiceTerminate = false;
	//printf("Service Start - ThraedID: %d\n", m_iThreadID);
    LaunchThread();
}

void TCPServiceThread::Stop()
{
	m_bServiceTerminate = true;
	//printf("Service Stop - ThraedID: %d\n", m_iThreadID);
}

void* TCPServiceThread::Execute()
{
	int iRetVal;
	SocketSet mySet;
	struct timeval timeout;

    while (!m_bServiceTerminate)
    {
        try
        {
    		do
    		{
    			timeout.tv_sec = 0;
    			timeout.tv_usec = 10;
    			mySet.Clear();
    			mySet += (SocketBase *)m_pcClientSocket;
    			iRetVal = select((int) mySet + 1, mySet, NULL, NULL, &timeout);
    			//if (iRetVal == -1)
    			//	throw errClientError;
    		} while((iRetVal==0) && (!m_bServiceTerminate));

    		if (mySet.IsMember((SocketBase*)m_pcClientSocket))
    		{
    			//printf("Service R/W - ThraedID: %d\n", m_iThreadID);
    			ServiceClient();
    		}
        }
        catch (KillThreadExceptions &excep)
        {
            switch (excep)
            {
                case errFatalError:
                case errServerDown:
                case errTooManyThreads:
                case errClientError:
        			printf("ERROR: Execute Exception : %s\n", (const char *) excep);
                	break;
            }
        }
    }

	//printf("TCPServiceThread - Deactivate ThreadID: %d\n", m_iThreadID);
    return NULL;
}


void TCPServiceThread::CleanUp()
{

}

void TCPServiceThread::ServiceClient()
{
    int iBytesTransferred = 0;
    unsigned int uiSize;

    if (!m_pcClientSocket)
        return;

    try
    {
        iBytesTransferred = m_pcClientSocket->RecvData(&m_CmdData.tcpcmd, sizeof(TCPCommand));
        if (iBytesTransferred != sizeof(TCPCommand))
            throw errFatalError;

        if (MODID_GEN_BASE <= m_CmdData.tcpcmd.mod &&
            MODID_GEN_BASE + MODID_RANGE > m_CmdData.tcpcmd.mod &&
            CMD_TYPE_FILE == m_CmdData.tcpcmd.cmd.ui3Type &&
            CMD_DIR_SET == m_CmdData.tcpcmd.cmd.ui1Dir)
        {
            char szFilename[CMD_FILE_BUF_SIZE];

            if (0 < m_CmdData.tcpcmd.datasize)
            {
                int iRet = -1;

                iRet = ReceiveFile(&m_CmdData, szFilename);
                if (-1 == iRet)
                    throw errFatalError;
                strcpy((char *)m_CmdData.buffer, szFilename);
                ParseGeneralCommand(&m_CmdData, CMD_FILE_BUF_SIZE);
            }
        }
        else
        {
            if (m_CmdData.tcpcmd.datasize > 0)
            {
                iBytesTransferred = m_pcClientSocket->RecvData(m_CmdData.buffer, m_CmdData.tcpcmd.datasize);
                if (iBytesTransferred != (int)m_CmdData.tcpcmd.datasize)
                    throw errFatalError;
            }

            uiSize = m_CmdData.tcpcmd.datasize + sizeof(TCPCommand);

            ParseCommand(&m_CmdData, uiSize);

            if (this->m_bIttdDebugInfoEnable) {
                printf("[%s:%d] Data: %d bytes\n",
                       (m_pcClientSocket->RemoteIPAddress()).GetAddressString(),
                       m_pcClientSocket->RemotePortNumber(),
                       iBytesTransferred);
            }
        }
    }
    catch (SocketException &excep)
    {
        if (excep == SocketException::errNotConnected)
        {
            //printf("Service DeleteClient - ThraedID: %d\n", m_iThreadID);
            m_pcServer->DeleteClient(this);
        }
        else
        {
            printf("ERROR: Socket Exception : %s\n", (const char *) excep);
            throw errFatalError;
        }
	}
}

int TCPServiceThread::SendCommand(unsigned int nMod, unsigned int nDir, unsigned int nType, unsigned int uiDataSize, UTLONGLONG utVal)
{
	TCPCommand tcpCmd;
	tcpCmd.mod = (unsigned short)nMod;
	tcpCmd.cmd.ui1Dir = nDir;
	tcpCmd.cmd.ui3Type = nType;
	tcpCmd.cmd.ui4GroupIdx = 0;
	tcpCmd.datasize = uiDataSize;
	tcpCmd.val = utVal;

	return SendDataCommand(&tcpCmd, (void*)NULL, 0);
}

int TCPServiceThread::SendDataCommand(TCPCommand* pCmd, void* pData, unsigned int uiDataSize)
{
    int iBytesTransferred = 0;

    if (!m_pcClientSocket)
    	return -1;

	try
	{
		if (pCmd != NULL)
		{
			iBytesTransferred = m_pcClientSocket->SendData(pCmd, sizeof(TCPCommand));
			if (iBytesTransferred != sizeof(TCPCommand))
				return -1;
		}
		if (pData != NULL && uiDataSize > 0)
		{
			iBytesTransferred = m_pcClientSocket->SendData(pData, uiDataSize);
			if ((unsigned int)iBytesTransferred != uiDataSize)
				return -1;
		}
	}
	catch (SocketException &excep)
	{
		if (excep == SocketException::errNotConnected)
		{
			//printf("Service DeleteClient - ThraedID: %d\n", m_iThreadID);
			m_pcServer->DeleteClient(this);
		}
		else
		{
			printf("ERROR: Socket Exception : %s\n", (const char *) excep);
			throw errFatalError;
		}

		return -1;
	}
	return iBytesTransferred;
}

int TCPServiceThread::ReceiveFile(TCPCmdData* pTcp, char* pszFilename)
{
    FILE* pfdWrite = NULL;
    bool bTerminate = false;
    unsigned long ulLeftBytes = 0;
    unsigned long ulReceivedBytes = 0;
    char szReceivedFilename[CMD_FILE_BUF_SIZE];
    char szFilePath[CMD_FILE_BUF_SIZE];
    char szFilename[CMD_FILE_BUF_SIZE];
    int iRet = -1;
    int iIdx = 0;
    char * pszTmp;

    if (!m_pcClientSocket)
        return -1;

    ulLeftBytes = pTcp->tcpcmd.val.ulValue;

    // Receive Filename.
    ulReceivedBytes = m_pcClientSocket->RecvData(szReceivedFilename, CMD_FILE_BUF_SIZE);
    if (CMD_FILE_BUF_SIZE != ulReceivedBytes) {
        return -1;
    }
    //printf("Received file name = %s\n", szReceivedFilename);

    // Extract file path and filename.
    szFilePath[0] = '\0';
    szFilename[0] = '\0';
    pszTmp = strrchr(szReceivedFilename, '/');
    if (NULL != pszTmp) {
        iIdx = &pszTmp[0] - &szReceivedFilename[0];
        iIdx++;
        strncpy(szFilePath, szReceivedFilename, (unsigned int)iIdx);
        szFilePath[iIdx] = '\0';
        strcpy(szFilename, &szReceivedFilename[iIdx]);
    } else {
        strcpy(szFilename, szReceivedFilename);
    }
    //printf("File path = %s\n", szFilePath);
    //printf("File name = %s\n", szFilename);

    // Open a file for write the data.
    sprintf(pszFilename, "%s", szReceivedFilename);
    if ('\0' == szFilePath[0]) {
        sprintf(szReceivedFilename, "/tmp/%s", szFilename);
    }
    //printf("Try to open %s ...\n", szReceivedFilename);
    if (NULL == (pfdWrite = fopen(szReceivedFilename, "wb"))) {
        printf("Cannot open %s to store received data.\n", szReceivedFilename);
        return -1;
    }

    // Receive File data.
    printf("Receiving:");
    do {
        ulReceivedBytes = (ulLeftBytes < 4096) ? ulLeftBytes : 4096;
        ulReceivedBytes = m_pcClientSocket->RecvData(pTcp->buffer, ulReceivedBytes);
        if (0 == ulReceivedBytes) {
            bTerminate = true;
            printf("timeout\n");
        } else {
            fwrite(pTcp->buffer, ulReceivedBytes, 1, pfdWrite);
            ulLeftBytes -= ulReceivedBytes;
            printf(".");
        }
    } while (ulLeftBytes && !bTerminate);
    if (!bTerminate) {
        printf("\nReceived complete.\n");
    }

    // Close this file.
    fclose(pfdWrite);
    pfdWrite = NULL;

    iRet = 0;

    return iRet;
}

int TCPServiceThread::SendFile(unsigned int nMod, char* pszFilename, void* pData, unsigned int uiDataSize)
{
	FILE* pfdRaed;
	unsigned long ulFileSize;

    if (!m_pcClientSocket)
    	return -1;

	if ((pfdRaed = fopen(pszFilename, "rb")) == NULL)
		return -1;

	long lPos;
	lPos = ftell(pfdRaed);
	fseek(pfdRaed, 0, SEEK_END);
	ulFileSize = ftell(pfdRaed);
	fseek(pfdRaed , lPos, SEEK_SET);
	lPos = ftell(pfdRaed);

	UTLONGLONG utVal;
	utVal.ulValue = ulFileSize;
	SendCommand(nMod, CMD_DIR_GET, CMD_TYPE_FILE, CMD_FILE_BUF_SIZE, utVal);

	char szFilenameBuf[CMD_FILE_BUF_SIZE];
	strncpy(szFilenameBuf, pszFilename, CMD_FILE_BUF_SIZE);
	SendDataCommand((TCPCommand*)NULL, (void*)szFilenameBuf, CMD_FILE_BUF_SIZE);

	unsigned char* pBuf = (unsigned char*)pData;
	unsigned long ulTotalBytes, ulReadBytes, ulOffset;
	int iSendBytes;
	ulTotalBytes = ulFileSize;

	do
	{
		ulReadBytes = (ulTotalBytes > uiDataSize) ? uiDataSize : ulTotalBytes;
		ulReadBytes = fread(pBuf, 1, ulReadBytes, pfdRaed);
		ulOffset = 0;

		do
		{
			iSendBytes = m_pcClientSocket->SendData(pBuf + ulOffset, ulReadBytes);
			if (iSendBytes <= 0)
			{
				printf("Send file [%s] error!!! err: %d\n", pszFilename, iSendBytes);
				fclose(pfdRaed);
				return -1;
			}

			ulOffset += iSendBytes;
			ulReadBytes -= iSendBytes;
			ulTotalBytes -= iSendBytes;
		} while (ulReadBytes > 0);
	} while (ulTotalBytes > 0);

	fclose(pfdRaed);
	printf("Send file [%s] success %lu bytes.\n", pszFilename, ulFileSize);

	return 0;
}

void TCPServiceThread::DumpCommandData(TCPCmdData* pTcp, unsigned int uiSize)
{

    if (m_bIttdDebugInfoEnable) {
        unsigned char* pChar = (unsigned char*)pTcp;

        printf("-------------------------------\n");
        for (unsigned int i=0; i<uiSize; i++)
        {
            printf("%02X ", *(pChar+i));
        }
        printf("\n");
        printf("-------------------------------\n");
    }
}

void TCPServiceThread::ParseCommand(TCPCmdData* pTcp, unsigned int uiSize)
{

    if (m_bIttdDebugInfoEnable) {
        printf("ittd_mod: %u, ", pTcp->tcpcmd.mod);
        printf("ver: %lu, ", pTcp->tcpcmd.ver);
        printf("cmd.ui1Dir: %lu, ", pTcp->tcpcmd.cmd.ui1Dir);
        printf("cmd.ui3Type: %lu, ", pTcp->tcpcmd.cmd.ui3Type);
        printf("cmd.ui4GroupIdx: %lu, ", pTcp->tcpcmd.cmd.ui4GroupIdx);
        printf("cmd.ui16ParamOrder: %lu, ", pTcp->tcpcmd.cmd.ui16ParamOrder);
        printf("datasize: %u, ", pTcp->tcpcmd.datasize);
        printf("ittd_Size: %u\n", uiSize);
    }

	if (pTcp->tcpcmd.mod >= MODID_GEN_BASE && pTcp->tcpcmd.mod < MODID_GEN_BASE + MODID_RANGE)
	{
		ParseGeneralCommand(pTcp, uiSize);
	}
	else if (pTcp->tcpcmd.mod >= MODID_ISP_BASE && pTcp->tcpcmd.mod < MODID_ISP_BASE + MODID_RANGE)
	{
		ParseIspCommand(pTcp, uiSize);
	}
	else if (pTcp->tcpcmd.mod >= MODID_ISPOST_BASE && pTcp->tcpcmd.mod < MODID_ISPOST_BASE + MODID_RANGE)
	{
		ParseIspostCommand(pTcp, uiSize);
	}
	else if (pTcp->tcpcmd.mod >= MODID_H264_BASE && pTcp->tcpcmd.mod < MODID_H264_BASE + MODID_RANGE)
	{
		ParseH26XCommand(pTcp, uiSize);
	}

	//DumpCommandData(pTcp, uiSize);
}

void TCPServiceThread::ParseGeneralCommand(TCPCmdData* pTcp, unsigned int uiSize)
{

    if (m_bIttdDebugInfoEnable) {
        printf("ittd_GEN\n");
    }

	if (pTcp->tcpcmd.cmd.ui1Dir == CMD_DIR_SET)
	{
        SetGeneralCommand(pTcp, uiSize);
	}
	else if (pTcp->tcpcmd.cmd.ui1Dir == CMD_DIR_GET)
	{
		GetGeneralCommand(pTcp, uiSize);
	}
}

void TCPServiceThread::ParseIspCommand(TCPCmdData* pTcp, unsigned int uiSize)
{

    if (m_bIttdDebugInfoEnable) {
        printf("ittd_ISP\n");
    }

	if (pTcp->tcpcmd.cmd.ui1Dir == CMD_DIR_SET)
	{
		camera_set_iq("isp", pTcp, uiSize);
	}
	else if (pTcp->tcpcmd.cmd.ui1Dir == CMD_DIR_GET)
	{
		camera_get_iq("isp", pTcp, uiSize);
		SendDataCommand((TCPCommand*)pTcp, (void*)pTcp->buffer, pTcp->tcpcmd.datasize);
	}
}

void TCPServiceThread::ParseIspostCommand(TCPCmdData* pTcp, unsigned int uiSize)
{

    if (m_bIttdDebugInfoEnable) {
        printf("ittd_ISPOST\n");
    }

    if (pTcp->tcpcmd.cmd.ui1Dir == CMD_DIR_SET)
    {
        SetIspostCommand(pTcp, uiSize);
    }
    else if (pTcp->tcpcmd.cmd.ui1Dir == CMD_DIR_GET)
    {
        GetIspostCommand(pTcp, uiSize);
    }
}

int TCPServiceThread::GetIspostCommand(TCPCmdData* pTcp, unsigned int uiSize)
{
    ISPOST_MDLE_DN * pModDN = (ISPOST_MDLE_DN *)pTcp->buffer;
    int iRet = -1;

    switch (pTcp->tcpcmd.mod - MODID_ISPOST_BASE) {
        case ISPOST_DN:
            {
                unsigned int uiCmdSize;
                cam_3d_dns_attr_t st3ddns_attr;

                uiCmdSize = sizeof(TCPCommand) + sizeof(ISPOST_MDLE_DN);
                if (uiCmdSize != uiSize) {
                    printf("ittd_Ispost: received data size %d < %d!!!\n", uiCmdSize, uiSize);
                    return iRet;
                }

                memset((void*)&st3ddns_attr, 0, sizeof(st3ddns_attr));

                //camera_get_ispost("ispost", pTcp, uiSize);
                iRet = camera_get_ispost((const char *)pModDN->szIspostInstance, pTcp, uiSize);
                if (0 == iRet) {
                    iRet = camera_get_3d_dns_attr("isp", &st3ddns_attr);
                    pModDN->iDnManualMode = (int)st3ddns_attr.cmn.mode;
                    pModDN->stManualDnParam.iThresholdY = st3ddns_attr.y_threshold;
                    pModDN->stManualDnParam.iThresholdU = st3ddns_attr.u_threshold;
                    pModDN->stManualDnParam.iThresholdV = st3ddns_attr.v_threshold;
                    pModDN->stManualDnParam.iWeight = st3ddns_attr.weight;
                }
                SendDataCommand((TCPCommand*)pTcp, (void*)pTcp->buffer, pTcp->tcpcmd.datasize);
            }
            break;

        default:
            iRet = camera_get_ispost((const char *)pModDN->szIspostInstance, pTcp, uiSize);
            SendDataCommand((TCPCommand*)pTcp, (void*)pTcp->buffer, pTcp->tcpcmd.datasize);
            break;
    }

    return iRet;
}

int TCPServiceThread::SetIspostCommand(TCPCmdData* pTcp, unsigned int uiSize)
{
    ISPOST_MDLE_DN * pModDN = (ISPOST_MDLE_DN *)pTcp->buffer;
    int iRet = -1;

    switch (pTcp->tcpcmd.mod - MODID_ISPOST_BASE) {
        case ISPOST_DN:
            {
                unsigned int uiCmdSize;
                cam_3d_dns_attr_t st3ddns_attr;

                uiCmdSize = sizeof(TCPCommand) + sizeof(ISPOST_MDLE_DN);
                if (uiCmdSize < uiSize) {
                    printf("ittd_Ispost: received data size %d < %d!!!\n", uiCmdSize, uiSize);
                    return iRet;
                }

                memset((void*)&st3ddns_attr, 0, sizeof(st3ddns_attr));

                if ((pModDN->stManualDnParam.iThresholdY != 0) && (pModDN->stManualDnParam.iThresholdU != 0)
                    && (pModDN->stManualDnParam.iThresholdV != 0) && (pModDN->stManualDnParam.iWeight != 0)) {
                    if (((pModDN->stManualDnParam.iThresholdY % 2) != 0)
                        || ((pModDN->stManualDnParam.iThresholdU % 2) != 0)
                        || ((pModDN->stManualDnParam.iThresholdV % 2) != 0)) {
                        printf("WARNING ! The YUV of threshold can't be odd value. \n");
                        pModDN->stManualDnParam.iThresholdY = (pModDN->stManualDnParam.iThresholdY + 2) & 0xFFFFFFFE;
                        pModDN->stManualDnParam.iThresholdU = (pModDN->stManualDnParam.iThresholdU + 2) & 0xFFFFFFFE;
                        pModDN->stManualDnParam.iThresholdV = (pModDN->stManualDnParam.iThresholdV + 2) & 0xFFFFFFFE;
                    }
                }
                st3ddns_attr.cmn.mode = (cam_common_mode_e)pModDN->iDnManualMode;
                st3ddns_attr.y_threshold = pModDN->stDnParam[0].iThresholdY;
                st3ddns_attr.u_threshold = pModDN->stDnParam[0].iThresholdU;
                st3ddns_attr.v_threshold = pModDN->stDnParam[0].iThresholdV;
                st3ddns_attr.weight = pModDN->stDnParam[0].iWeight;
                //printf("[0]yuvw(%d,%d,%d,%d)\n", st3ddns_attr.y_threshold, st3ddns_attr.u_threshold, st3ddns_attr.v_threshold, st3ddns_attr.weight);

                //camera_set_ispost("ispost", pTcp, uiSize);
                iRet = camera_set_ispost((const char *)pModDN->szIspostInstance, pTcp, uiSize);
                if (0 == iRet) {
                    iRet = camera_set_3d_dns_attr("isp", &st3ddns_attr);
                }
            }
            break;

        default:
            iRet = camera_set_ispost((const char *)pModDN->szIspostInstance, pTcp, uiSize);
            break;
    }

    return iRet;
}

void TCPServiceThread::ParseH26XCommand(TCPCmdData* pTcp, unsigned int uiSize)
{

    if (m_bIttdDebugInfoEnable) {
        printf("ittd_H26x\n");
    }

    if (pTcp->tcpcmd.cmd.ui1Dir == CMD_DIR_SET)
    {
        SetH26XCommand(pTcp, uiSize);
    }
    else if (pTcp->tcpcmd.cmd.ui1Dir == CMD_DIR_GET)
    {
        GetH26XCommand(pTcp, uiSize);
    }
}

int TCPServiceThread::GetH26XCommand(TCPCmdData* pTcp, unsigned int uiSize)
{
    const char szRateCtrlMode[][6] = {
        "CBR",
        "VBR",
        "FixQp"
    };
    int iRet = -1;
    H26X_MDLE_ENC * pModENC = (H26X_MDLE_ENC *)pTcp->buffer;

    // Since we added the version control, therefore we will not check the structure size.
    //unsigned int uiCmdSize;
    //uiCmdSize = sizeof(TCPCommand) + sizeof(H26X_MDLE_ENC);
    //if (uiCmdSize != uiSize) {
    //    printf("ittd_H26x: received data size %d != %d!!!\n", uiCmdSize, uiSize);
    //    return iRet;
    //}

    switch (pTcp->tcpcmd.mod - MODID_H264_BASE) {
        case H26X_ENC:
            {
                if ((H26X_ENC_CMD_VER_MAX - 1) < pTcp->tcpcmd.ver) {
                    printf("Out of support version. Ver = %lu\n", pTcp->tcpcmd.ver);
                    return iRet;
                }
                switch (pTcp->tcpcmd.ver) {
                    case H26X_ENC_CMD_VER_V0:
                    default:
                        //strcpy(pModENC->szEncoderChannel, "h1264-stream");
                        iRet = video_get_ratectrl((const char *)pModENC->szEncoderChannel, (struct v_rate_ctrl_info *)&pModENC->stVideoRateCtrl);
                        if (0 > iRet) {
                            printf("ittd_H26x: get encoder rate control failed!!!\n");
                            return iRet;
                        }
                        pModENC->stVideoRateCtrl.ullBps = video_get_rtbps((const char *)pModENC->szEncoderChannel);
                        if (H26X_ENC_CMD_VER_V0 == pTcp->tcpcmd.ver) {
                            pTcp->tcpcmd.datasize = sizeof(H26X_MDLE_ENC);
                        }

                        if (m_bH26XDebugInfoEnable) {
                            int iRateCtrlMode;

                            iRateCtrlMode = pModENC->stVideoRateCtrl.eRateCtrlMode;
                            printf("ittd_H26x get ENC.datasize: %u\n", pTcp->tcpcmd.datasize);
                            printf("ittd_H26x get ENC.szEncoderChannel: %s\n", pModENC->szEncoderChannel);
                            printf("ittd_H26x get ENC.szName: %s\n", pModENC->stVideoRateCtrl.szName);
                            printf("ittd_H26x get ENC.eRateCtrlMode: %s\n", szRateCtrlMode[pModENC->stVideoRateCtrl.eRateCtrlMode]);
                            printf("ittd_H26x get ENC.iGopLength: %d\n", pModENC->stVideoRateCtrl.iGopLength);
                            printf("ittd_H26x get ENC.iPictureSkip: %d\n", pModENC->stVideoRateCtrl.iPictureSkip);
                            printf("ittd_H26x get ENC.iIdrInterval: %d\n", pModENC->stVideoRateCtrl.iIdrInterval);
                            printf("ittd_H26x get ENC.iHrd: %d\n", pModENC->stVideoRateCtrl.iHrd);
                            printf("ittd_H26x get ENC.iHrdCpbSize: %d\n", pModENC->stVideoRateCtrl.iHrdCpbSize);
                            printf("ittd_H26x get ENC.iRefreshInterval: %d\n", pModENC->stVideoRateCtrl.iRefreshInterval);
                            printf("ittd_H26x get ENC.ullBps: %llu\n", pModENC->stVideoRateCtrl.ullBps);
                            printf("ittd_H26x set ENC.iMacroBlockRateCtrl: %d\n", pModENC->stVideoRateCtrl.iMacroBlockRateCtrl);
                            printf("ittd_H26x set ENC.iMacroBlockQpAdjustment: %d\n", pModENC->stVideoRateCtrl.iMacroBlockQpAdjustment);
                            switch (iRateCtrlMode) {
                                case ENC_V_RATE_CTRL_MODE_CBR_MODE:
                                    printf("ittd_H26x get ENC.stCBR.iQpMax: %d\n", pModENC->stVideoRateCtrl.stCBR.iQpMax);
                                    printf("ittd_H26x get ENC.stCBR.iQpMin: %d\n", pModENC->stVideoRateCtrl.stCBR.iQpMin);
                                    printf("ittd_H26x get ENC.stCBR.iBitRate: %d (%5.3lfM)bs\n", pModENC->stVideoRateCtrl.stCBR.iBitRate, ((double)pModENC->stVideoRateCtrl.stCBR.iBitRate / (1024 * 1024)));
                                    printf("ittd_H26x get ENC.stCBR.iQpDelta: %d\n", pModENC->stVideoRateCtrl.stCBR.iQpDelta);
                                    printf("ittd_H26x set ENC.stCBR.iQpHdr: %d\n", pModENC->stVideoRateCtrl.stCBR.iQpHdr);
                                    break;

                                case ENC_V_RATE_CTRL_MODE_VBR_MODE:
                                    printf("ittd_H26x get ENC.stVBR.iQpMax: %d\n", pModENC->stVideoRateCtrl.stVBR.iQpMax);
                                    printf("ittd_H26x get ENC.stVBR.iQpMin: %d\n", pModENC->stVideoRateCtrl.stVBR.iQpMin);
                                    printf("ittd_H26x get ENC.stVBR.iMaxBitRate: %d (%5.3lfM)bs\n", pModENC->stVideoRateCtrl.stVBR.iMaxBitRate, ((double)pModENC->stVideoRateCtrl.stVBR.iMaxBitRate / (1024 * 1024)));
                                    printf("ittd_H26x get ENC.stVBR.iThreshold: %d\n", pModENC->stVideoRateCtrl.stVBR.iThreshold);
                                    printf("ittd_H26x get ENC.stVBR.iQpDelta: %d\n", pModENC->stVideoRateCtrl.stVBR.iQpDelta);
                                    break;

                                case ENC_V_RATE_CTRL_MODE_FIXQP_MODE:
                                    printf("ittd_H26x get ENC.stFixQp.iQpFix: %d\n", pModENC->stVideoRateCtrl.stFixQp.iQpFix);
                                    break;
                            }
                        }
                        SendDataCommand((TCPCommand*)pTcp, (void*)pTcp->buffer, pTcp->tcpcmd.datasize);
                        break;
                }
            }
            break;

        case H26X_VER:
            if ((H26X_VER_CMD_VER_MAX - 1) < pTcp->tcpcmd.ver) {
                printf("Out of support version. Ver = %lu\n", pTcp->tcpcmd.ver);
                return iRet;
            }
            switch (pTcp->tcpcmd.ver) {
                case H26X_VER_CMD_VER_V0:
                default:
                    //H26X_MDLE_VER * pModEncVer = (H26X_MDLE_VER *)pTcp->buffer;
                    //if (H26X_VER_CMD_VER_V0 == pTcp->tcpcmd.ver) {
                    //    pTcp->tcpcmd.datasize = sizeof(H26X_MDLE_VER);
                    //}
                    //SendDataCommand((TCPCommand*)pTcp, (void*)pTcp->buffer, pTcp->tcpcmd.datasize);
                    //iRet = 0;
                    printf("H26X does not support this command.\n");
                    break;
            }
            break;

        case H26X_DBG:
            if ((H26X_DBG_CMD_VER_MAX - 1) < pTcp->tcpcmd.ver) {
                printf("Out of support version. Ver = %lu\n", pTcp->tcpcmd.ver);
                return iRet;
            }
            switch (pTcp->tcpcmd.ver) {
                case H26X_DBG_CMD_VER_V0:
                default:
                    H26X_MDLE_DBG *pModEncDbg = (H26X_MDLE_DBG *)pTcp->buffer;
                    pModEncDbg->bDebugEnable = m_bH26XDebugInfoEnable;
                    if (H26X_DBG_CMD_VER_V0 == pTcp->tcpcmd.ver) {
                        pTcp->tcpcmd.datasize = sizeof(H26X_MDLE_DBG);
                    }

                    SendDataCommand((TCPCommand*)pTcp, (void*)pTcp->buffer, pTcp->tcpcmd.datasize);
                    iRet = 0;
                    break;
            }
            break;

        default:
            printf("H26X does not support this command.\n");
            break;
    }

    return iRet;
}

int TCPServiceThread::SetH26XCommand(TCPCmdData* pTcp, unsigned int uiSize)
{
    const char szRateCtrlMode[][6] = {
        "CBR",
        "VBR",
        "FixQp"
    };
    int iRet = -1;
    H26X_MDLE_ENC * pModENC = (H26X_MDLE_ENC *)pTcp->buffer;

    // Since we added the version control, therefore we will not check the structure size.
    //unsigned int uiCmdSize;
    //uiCmdSize = sizeof(TCPCommand) + sizeof(H26X_MDLE_ENC);
    //if (uiCmdSize != uiSize) {
    //    printf("ittd_H26x: received data size %d != %d!!!\n", uiCmdSize, uiSize);
    //    return iRet;
    //}

    switch (pTcp->tcpcmd.mod - MODID_H264_BASE) {
        case H26X_ENC:
            {
                if ((H26X_ENC_CMD_VER_MAX - 1) < pTcp->tcpcmd.ver) {
                    printf("Out of support version. Ver = %lu\n", pTcp->tcpcmd.ver);
                    return iRet;
                }
                switch (pTcp->tcpcmd.ver) {
                    case H26X_ENC_CMD_VER_V0:
                    default:
                        if (m_bH26XDebugInfoEnable) {
                            int iRateCtrlMode;

                            //strcpy(pModENC->szEncoderChannel, "h1264-stream");
                            iRateCtrlMode = pModENC->stVideoRateCtrl.eRateCtrlMode;
                            printf("ittd_H26x set ENC.datasize: %u\n", pTcp->tcpcmd.datasize);
                            printf("ittd_H26x set ENC.szEncoderChannel: %s\n", pModENC->szEncoderChannel);
                            printf("ittd_H26x set ENC.szName: %s\n", pModENC->stVideoRateCtrl.szName);
                            printf("ittd_H26x set ENC.eRateCtrlMode: %s\n", szRateCtrlMode[pModENC->stVideoRateCtrl.eRateCtrlMode]);
                            printf("ittd_H26x set ENC.iGopLength: %d\n", pModENC->stVideoRateCtrl.iGopLength);
                            printf("ittd_H26x set ENC.iPictureSkip: %d\n", pModENC->stVideoRateCtrl.iPictureSkip);
                            printf("ittd_H26x set ENC.iIdrInterval: %d\n", pModENC->stVideoRateCtrl.iIdrInterval);
                            printf("ittd_H26x set ENC.iHrd: %d\n", pModENC->stVideoRateCtrl.iHrd);
                            printf("ittd_H26x set ENC.iHrdCpbSize: %d\n", pModENC->stVideoRateCtrl.iHrdCpbSize);
                            printf("ittd_H26x set ENC.iRefreshInterval: %d\n", pModENC->stVideoRateCtrl.iRefreshInterval);
                            printf("ittd_H26x set ENC.iMacroBlockRateCtrl: %d\n", pModENC->stVideoRateCtrl.iMacroBlockRateCtrl);
                            printf("ittd_H26x set ENC.iMacroBlockQpAdjustment: %d\n", pModENC->stVideoRateCtrl.iMacroBlockQpAdjustment);
                            switch (iRateCtrlMode) {
                                case ENC_V_RATE_CTRL_MODE_CBR_MODE:
                                    printf("ittd_H26x set ENC.stCBR.iQpMax: %d\n", pModENC->stVideoRateCtrl.stCBR.iQpMax);
                                    printf("ittd_H26x set ENC.stCBR.iQpMin: %d\n", pModENC->stVideoRateCtrl.stCBR.iQpMin);
                                    printf("ittd_H26x set ENC.stCBR.iBitRate: %d (%5.3lfM)bs\n", pModENC->stVideoRateCtrl.stCBR.iBitRate, ((double)pModENC->stVideoRateCtrl.stCBR.iBitRate / (1024 * 1024)));
                                    printf("ittd_H26x set ENC.stCBR.iQpDelta: %d\n", pModENC->stVideoRateCtrl.stCBR.iQpDelta);
                                    printf("ittd_H26x set ENC.stCBR.iQpHdr: %d\n", pModENC->stVideoRateCtrl.stCBR.iQpHdr);
                                    break;

                                case ENC_V_RATE_CTRL_MODE_VBR_MODE:
                                    printf("ittd_H26x set ENC.stVBR.iQpMax: %d\n", pModENC->stVideoRateCtrl.stVBR.iQpMax);
                                    printf("ittd_H26x set ENC.stVBR.iQpMin: %d\n", pModENC->stVideoRateCtrl.stVBR.iQpMin);
                                    printf("ittd_H26x set ENC.stVBR.iMaxBitRate: %d (%5.3lfM)bs\n", pModENC->stVideoRateCtrl.stVBR.iMaxBitRate, ((double)pModENC->stVideoRateCtrl.stVBR.iMaxBitRate / (1024 * 1024)));
                                    printf("ittd_H26x set ENC.stVBR.iThreshold: %d\n", pModENC->stVideoRateCtrl.stVBR.iThreshold);
                                    printf("ittd_H26x set ENC.stVBR.iQpDelta: %d\n", pModENC->stVideoRateCtrl.stVBR.iQpDelta);
                                    break;

                                case ENC_V_RATE_CTRL_MODE_FIXQP_MODE:
                                    printf("ittd_H26x set ENC.stFixQp.iQpFix: %d\n", pModENC->stVideoRateCtrl.stFixQp.iQpFix);
                                    break;
                            }
                            printf("ittd_H26x set ENC.ullBps: %llu\n", pModENC->stVideoRateCtrl.ullBps);
                        }

                        iRet = video_set_ratectrl((const char *)pModENC->szEncoderChannel, (struct v_rate_ctrl_info *)&pModENC->stVideoRateCtrl);
                        if (0 > iRet) {
                            printf("ittd_H26x: set encoder rate control failed!!!\n");
                        }
                        break;
                }
            }
            break;

        case H26X_VER:
            //if ((H26X_VER_CMD_VER_MAX - 1) < pTcp->tcpcmd.ver) {
            //    printf("Out of support version. Ver = %lu\n", pTcp->tcpcmd.ver);
            //    return iRet;
            //}
            //switch (pTcp->tcpcmd.ver) {
            //    case H26X_VER_CMD_VER_V0:
            //    default:
            //        //H26X_MDLE_VER * pModEncVer = (H26X_MDLE_VER *)pTcp->buffer;
            //        //if (H26X_VER_CMD_VER_V0 == pTcp->tcpcmd.ver) {
            //        //    pTcp->tcpcmd.datasize = sizeof(H26X_MDLE_VER);
            //        //}
            //        //iRet = 0;
            //        LOGE("H26X does not support this command.\n");
            //        break;
            //}
            printf("H26X does not support this command.\n");
            break;

        case H26X_DBG:
            if ((H26X_DBG_CMD_VER_MAX - 1) < pTcp->tcpcmd.ver) {
                printf("Out of support version. Ver = %lu\n", pTcp->tcpcmd.ver);
                return iRet;
            }
            switch (pTcp->tcpcmd.ver) {
                case H26X_DBG_CMD_VER_V0:
                default:
                    H26X_MDLE_DBG * pModEncDbg = (H26X_MDLE_DBG *)pTcp->buffer;
                    m_bH26XDebugInfoEnable = pModEncDbg->bDebugEnable;
                    iRet = 0;
                    break;
            }
            break;

        default:
            printf("H26X does not support this command.\n");
            break;
    }

    return iRet;
}

int TCPServiceThread::GetGeneralCommand(TCPCmdData* pTcp, unsigned int uiSize)
{
    int iRet = -1;
	cam_save_data_t cam_sav;
	char szFullFilename[256];

    //if (m_bIttdDebugInfoEnable) {
    //    printf("ittd_gen_get_mod: %u\n", pTcp->tcpcmd.mod);
    //    printf("ittd_gen_get_cmd.ui3Type: %lu\n", pTcp->tcpcmd.cmd.ui3Type);
    //}

    switch (pTcp->tcpcmd.mod - MODID_GEN_BASE)
    {
        case GEN_GET_ITTD_VERSION:
        {
            if ((GEN_GET_ITTD_VER_CMD_VER_MAX - 1) < pTcp->tcpcmd.ver) {
                printf("Out of support version. Ver = %lu\n", pTcp->tcpcmd.ver);
                return iRet;
            }
            switch (pTcp->tcpcmd.ver) {
                case GEN_GET_ITTD_VER_CMD_VER_V0:
                default:
                    pTcp->tcpcmd.val.ulValue = ITTD_VERSION;
                    SendDataCommand((TCPCommand*)pTcp, (void*)NULL, 0);
                    break;
            }
            break;
        }

        case GEN_GET_QSDK_VERSION:
        {
            if ((GEN_GET_QSDK_VER_CMD_VER_MAX - 1) < pTcp->tcpcmd.ver) {
                printf("Out of support version. Ver = %lu\n", pTcp->tcpcmd.ver);
                return iRet;
            }
            switch (pTcp->tcpcmd.ver) {
                case GEN_GET_QSDK_VER_CMD_VER_V0:
                default:
                    pTcp->tcpcmd.val.ulValue = QSDK_VERSION;
                    pTcp->tcpcmd.datasize = sizeof(QsdkVersion);
                    SendDataCommand((TCPCommand*)pTcp, (void*)&g_QsdkVersion[0], sizeof(QsdkVersion));
                    break;
            }
            break;
        }

        case GEN_GET_JSON_FILE:
        {
            char szJsonPath[256];

            if ((GEN_GET_JSON_FILE_CMD_VER_MAX - 1) < pTcp->tcpcmd.ver) {
                printf("Out of support version. Ver = %lu\n", pTcp->tcpcmd.ver);
                return iRet;
            }
            switch (pTcp->tcpcmd.ver) {
                case GEN_GET_JSON_FILE_CMD_VER_V0:
                default:
                    videobox_get_jsonpath(szJsonPath, 256);
                    if (m_bIttdDebugInfoEnable) {
                        printf("Get Json file: %s\n", szJsonPath);
                    }
                    iRet = SendFile(MODID_GEN_BASE + GEN_GET_JSON_FILE, szJsonPath, pTcp->buffer, CMD_DATA_BUF_SIZE);
                    break;
            }
            break;
        }

        case GEN_GET_ISP_FLX_FILE:
        {
            if ((GEN_GET_ISP_FLX_FILE_CMD_VER_MAX - 1) < pTcp->tcpcmd.ver) {
                printf("Out of support version. Ver = %lu\n", pTcp->tcpcmd.ver);
                return iRet;
            }
            switch (pTcp->tcpcmd.ver) {
                case GEN_GET_ISP_FLX_FILE_CMD_VER_V0:
                default:
                    cam_sav.fmt = CAM_DATA_FMT_BAYER_FLX;
#if 0
                    getcwd(cam_sav.file_path, sizeof(cam_sav.file_path));
                    strcat(cam_sav.file_path, "/");
#else
                    strcpy(cam_sav.file_path, "/tmp/");
#endif //#if 0
                    cam_sav.file_name[0] = 0;
                    camera_save_data("isp", &cam_sav);

                    if (m_bIttdDebugInfoEnable) {
                        printf("Get Isp flx file: %s%s\n", cam_sav.file_path, cam_sav.file_name);
                    }
                    if(cam_sav.file_name == NULL || strlen(cam_sav.file_name) <= 0) {
                        sprintf(cam_sav.file_name, "empty.flx");
                    }
                    snprintf(szFullFilename, sizeof(szFullFilename), "%s%s", cam_sav.file_path, cam_sav.file_name);
                    if(access(szFullFilename, F_OK)) {
                        printf("%s not exist\n", szFullFilename);
                        char newfile[128] = {0};
                        int fw, fh;
                        video_get_resolution("v4l2-out", &fw, &fh);
                        snprintf(newfile, sizeof(newfile), "dd if=/dev/zero of=%s bs=%d count=1", szFullFilename, fw*fh*2+303);
                        system(newfile);
                    }
                    iRet = SendFile(MODID_GEN_BASE + GEN_GET_ISP_FLX_FILE, szFullFilename, pTcp->buffer, CMD_DATA_BUF_SIZE);
                    snprintf(szFullFilename, sizeof(szFullFilename), "rm -rf %s%s", cam_sav.file_path, cam_sav.file_name);
                    system(szFullFilename);
                    break;
            }
            break;
        }

        case GEN_GET_ISP_RAW_FILE:
        {
            if ((GEN_GET_ISP_RAW_FILE_CMD_VER_MAX - 1) < pTcp->tcpcmd.ver) {
                printf("Out of support version. Ver = %lu\n", pTcp->tcpcmd.ver);
                return iRet;
            }
            switch (pTcp->tcpcmd.ver) {
                case GEN_GET_ISP_RAW_FILE_CMD_VER_V0:
                default:
                    cam_sav.fmt = CAM_DATA_FMT_BAYER_RAW;
#if 0
                    getcwd(cam_sav.file_path, sizeof(cam_sav.file_path));
                    strcat(cam_sav.file_path, "/");
#else
                    strcpy(cam_sav.file_path, "/tmp/");
#endif //#if 0
                    cam_sav.file_name[0] = 0;
                    camera_save_data("isp", &cam_sav);
                    if (m_bIttdDebugInfoEnable) {
                        printf("Get Isp raw file: %s%s\n", cam_sav.file_path, cam_sav.file_name);
                    }
                    snprintf(szFullFilename, sizeof(szFullFilename), "%s%s", cam_sav.file_path, cam_sav.file_name);
                    iRet = SendFile(MODID_GEN_BASE + GEN_GET_ISP_RAW_FILE, szFullFilename, pTcp->buffer, CMD_DATA_BUF_SIZE);
                    snprintf(szFullFilename, sizeof(szFullFilename), "rm -rf %s%s", cam_sav.file_path, cam_sav.file_name);
                    system(szFullFilename);
                    break;
            }
            break;
        }

        case GEN_GET_ISP_YUV_FILE:
        {
            if ((GEN_GET_ISP_YUV_FILE_CMD_VER_MAX - 1) < pTcp->tcpcmd.ver) {
                printf("Out of support version. Ver = %lu\n", pTcp->tcpcmd.ver);
                return iRet;
            }
            switch (pTcp->tcpcmd.ver) {
                case GEN_GET_ISP_YUV_FILE_CMD_VER_V0:
                default:
                    cam_sav.fmt = CAM_DATA_FMT_YUV;
#if 0
                    getcwd(cam_sav.file_path, sizeof(cam_sav.file_path));
                    strcat(cam_sav.file_path, "/");
#else
                    strcpy(cam_sav.file_path, "/tmp/");
#endif //#if 0
#ifndef ITTD_V4L2_ONLY
                    cam_sav.file_name[0] = 0;
                    camera_save_data("isp", &cam_sav);
#else
                    char *ispch = "v4l2-out";
                    struct timeval stTv;
                    struct tm* stTm;
                    char preName[100];

                    int ret, w, h;
                    struct fr_buf_info buf = {0};

                    ret = video_get_resolution(ispch, &w, &h);
                    if(ret < 0){
                        printf("get video resolution fail!\n");
                    }

                    gettimeofday(&stTv,NULL);
                    stTm = localtime(&stTv.tv_sec);
                    snprintf(preName,sizeof(preName), "%02d%02d-%02d%02d%02d-%03d",
                            stTm->tm_mon + 1, stTm->tm_mday,
                            stTm->tm_hour, stTm->tm_min, stTm->tm_sec,
                            (int)(stTv.tv_usec / 1000));

                    snprintf(cam_sav.file_name, sizeof(cam_sav.file_name), "%s_%dx%d_%s.yuv",
                            preName, w, h,
                            "NV12");
                    ret = video_get_frame(ispch, &buf);
                    if(ret < 0) {
                        printf("video_get_frame %s failed, ret: %d\n", ispch, ret);
                        break;
                    }
                    snprintf(szFullFilename, sizeof(szFullFilename), "%s%s", cam_sav.file_path, cam_sav.file_name);
                    printf("file name: %s\n", szFullFilename);
                    FILE *fp = fopen(szFullFilename, "w+");
                    fwrite(buf.virt_addr, buf.size, 1, fp);
                    fclose(fp);
                    video_put_frame(ispch, &buf);
#endif

                    if (m_bIttdDebugInfoEnable) {
                        printf("Get Isp yuv file: %s%s\n", cam_sav.file_path, cam_sav.file_name);
                    }
                    snprintf(szFullFilename, sizeof(szFullFilename), "%s%s", cam_sav.file_path, cam_sav.file_name);
                    iRet = SendFile(MODID_GEN_BASE + GEN_GET_ISP_YUV_FILE, szFullFilename, pTcp->buffer, CMD_DATA_BUF_SIZE);
                    snprintf(szFullFilename, sizeof(szFullFilename), "rm -rf %s%s", cam_sav.file_path, cam_sav.file_name);
                    system(szFullFilename);
                    break;
            }
            break;
        }

        case GEN_GET_ISPOST_YUV_FILE:
            if ((GEN_GET_ISPOST_YUV_FILE_CMD_VER_MAX - 1) < pTcp->tcpcmd.ver) {
                printf("Out of support version. Ver = %lu\n", pTcp->tcpcmd.ver);
                return iRet;
            }
            switch (pTcp->tcpcmd.ver) {
                case GEN_GET_ISPOST_YUV_FILE_CMD_VER_V0:
                default:
                    printf("Does not support at this version.\n");
                    break;
            }
            break;

        case GEN_GET_ISP_SETTING_FILE:
        {
            char szSettingPath[256] = "/root/.ispddk/sensor0-isp-config.txt";

            if ((GEN_GET_ISP_SETTING_FILE_CMD_VER_MAX - 1) < pTcp->tcpcmd.ver) {
                printf("Out of support version. Ver = %lu\n", pTcp->tcpcmd.ver);
                return iRet;
            }
            switch (pTcp->tcpcmd.ver) {
                case GEN_GET_ISP_SETTING_FILE_CMD_VER_V0:
                default:

                    if (m_bIttdDebugInfoEnable) {
                        printf("Get sensor setting file: %s\n", szSettingPath);
                    }
                    iRet = SendFile(MODID_GEN_BASE + GEN_GET_ISP_SETTING_FILE, szSettingPath, pTcp->buffer, CMD_DATA_BUF_SIZE);
                    break;
            }
            break;
        }

        case GEN_GET_ISP_SETTING_SAVE_FILE:
        {
            if ((GEN_GET_ISP_SETTING_SAVE_FILE_CMD_VER_MAX - 1) < pTcp->tcpcmd.ver) {
                printf("Out of support version. Ver = %lu\n", pTcp->tcpcmd.ver);
                return iRet;
            }
            switch (pTcp->tcpcmd.ver) {
                case GEN_GET_ISP_SETTING_SAVE_FILE_CMD_VER_V0:
                default:
                    cam_sav.fmt = CAM_DATA_FMT_NONE;
#if 0
                    int iLen;
                    getcwd(cam_sav.file_path, sizeof(cam_sav.file_path));
                    iLen = strlen(cam_sav.file_path);
  #if 0
                    if ((1 == iLen) && ('/' == cam_sav.file_path[iLen - 1])) {
                        cam_sav.file_path[0] = '\0';
                    } else if ((0 != iLen) & ('/' != cam_sav.file_path[iLen - 1])) {
                        strcat(cam_sav.file_path, "/");
                    }
  #else
                    if ((0 != iLen) & ('/' != cam_sav.file_path[iLen - 1])) {
                        strcat(cam_sav.file_path, "/");
                    }
  #endif //#if 0
#else
                    strcpy(cam_sav.file_path, "/tmp/");
#endif //#if 0
                    cam_sav.file_name[0] = 0;
                    camera_save_isp_parameters("isp", &cam_sav);
                    snprintf(szFullFilename, sizeof(szFullFilename), "%s%s", cam_sav.file_path, cam_sav.file_name);
                    if (m_bIttdDebugInfoEnable) {
                        printf("Get sensor setting save file: %s\n", szFullFilename);
                    }
                    iRet = SendFile(MODID_GEN_BASE + GEN_GET_ISP_SETTING_SAVE_FILE, szFullFilename, pTcp->buffer, CMD_DATA_BUF_SIZE);
                    snprintf(szFullFilename, sizeof(szFullFilename), "rm -rf %s%s", cam_sav.file_path, cam_sav.file_name);
                    system(szFullFilename);
                    break;
            }
            break;
        }

        case GEN_GET_GRID_DATA_FILE:
        {
            if ((GEN_GET_GRID_DATA_FILE_CMD_VER_MAX - 1) < pTcp->tcpcmd.ver) {
                printf("Out of support version. Ver = %lu\n", pTcp->tcpcmd.ver);
                return iRet;
            }
            switch (pTcp->tcpcmd.ver) {
                case GEN_GET_GRID_DATA_FILE_CMD_VER_V0:
                default:
                    printf("Does not support at this version.\n");
                    break;
            }
            break;
        }

        case GEN_GET_ISP_LSH_FILE:
        {
            if ((GEN_GET_ISP_LSH_FILE_CMD_VER_MAX - 1) < pTcp->tcpcmd.ver) {
                printf("Out of support version. Ver = %lu\n", pTcp->tcpcmd.ver);
                return iRet;
            }
            switch (pTcp->tcpcmd.ver) {
                case GEN_GET_ISP_LSH_FILE_CMD_VER_V0:
                default:
                    printf("Does not support at this version.\n");
                    break;
            }
            break;
        }

        case GEN_GET_FILE:
        {
            if ((GEN_GET_FILE_CMD_VER_MAX - 1) < pTcp->tcpcmd.ver) {
                printf("Out of support version. Ver = %lu\n", pTcp->tcpcmd.ver);
                return iRet;
            }
            switch (pTcp->tcpcmd.ver) {
                case GEN_GET_FILE_CMD_VER_V0:
                default:
                    printf("Does not support at this version.\n");
                    break;
            }
            break;
        }
    }

    return iRet;
}

int TCPServiceThread::SetGeneralCommand(TCPCmdData* pTcp, unsigned int uiSize)
{
    int iRet = -1;
    char szFullPathFilename[CMD_FILE_BUF_SIZE];
    char szFullPathFilenameTemp[CMD_FILE_BUF_SIZE];
    char szFullPathFilenameBackup[CMD_FILE_BUF_SIZE];
    char szFilename[CMD_FILE_BUF_SIZE];

    //if (m_bIttdDebugInfoEnable) {
    //    printf("ittd_gen_set_mod: %u\n", pTcp->tcpcmd.mod);
    //    printf("ittd_gen_set_cmd.ui3Type: %lu\n", pTcp->tcpcmd.cmd.ui3Type);
    //}

    switch (pTcp->tcpcmd.mod - MODID_GEN_BASE)
    {
        case GEN_SET_DEBUG_INFO_ENABLE:
        {
            //printf("GEN_SET_DEBUG_INFO_ENABLE: %lu\n", pTcp->tcpcmd.val.ulValue);
            if ((GEN_SET_DEBUG_INFO_ENABLE_CMD_VER_MAX - 1) < pTcp->tcpcmd.ver) {
                printf("Out of support version. Ver = %lu\n", pTcp->tcpcmd.ver);
                return iRet;
            }
            switch (pTcp->tcpcmd.ver) {
                case GEN_SET_DEBUG_INFO_ENABLE_CMD_VER_V0:
                default:
                    m_bIttdDebugInfoEnable = pTcp->tcpcmd.val.ulValue;
                    break;
            }
            break;
        }

        case GEN_SET_JSON_FILE:
        {
            char * pszFilename = NULL;

            if ((GEN_SET_JSON_FILE_CMD_VER_MAX - 1) < pTcp->tcpcmd.ver) {
                printf("Out of support version. Ver = %lu\n", pTcp->tcpcmd.ver);
                return iRet;
            }
            switch (pTcp->tcpcmd.ver) {
                case GEN_SET_JSON_FILE_CMD_VER_V0:
                default:
                    strcpy(&szFullPathFilenameTemp[0], (const char *)pTcp->buffer);
                    pszFilename = strrchr(szFullPathFilenameTemp, '/');
                    if (NULL != pszFilename) {
                        strcpy(szFullPathFilename, szFullPathFilenameTemp);
                    } else {
                        sprintf(szFullPathFilename, "/tmp/%s", szFullPathFilenameTemp);
                    }
                    videobox_stop();
                    videobox_start(szFullPathFilename);
                    break;
            }
            break;
        }

        case GEN_SET_ISP_SETTING_FILE:
        {
            int iIdx;
            char szTemp[CMD_FILE_BUF_SIZE * 2];
            char * pszFilename = NULL;

            if ((GEN_SET_ISP_SETTING_FILE_CMD_VER_MAX - 1) < pTcp->tcpcmd.ver) {
                printf("Out of support version. Ver = %lu\n", pTcp->tcpcmd.ver);
                return iRet;
            }
            switch (pTcp->tcpcmd.ver) {
                case GEN_SET_ISP_SETTING_FILE_CMD_VER_V0:
                default:
                    strcpy(szFullPathFilenameTemp, (const char *)pTcp->buffer);
                    pszFilename = strrchr(szFullPathFilenameTemp, '/');
                    if (NULL != pszFilename) {
                        iIdx = &pszFilename[0] - &szFullPathFilenameTemp[0];
                        iIdx++;
                    } else {
                        iIdx = 0;
                    }
                    strcpy(szFilename, &szFullPathFilenameTemp[iIdx]);

                    sprintf(szFullPathFilename, "/root/.ispddk/%s", szFilename);
                    sprintf(szFullPathFilenameBackup, "/root/.ispddk/%s.bak", szFilename);
                    if (-1 != access(szFullPathFilenameBackup, 0)) {
                        sprintf(szTemp, "rm /root/.ispddk/%s.bak", szFilename);
                        system(szTemp);
                    }
                    sprintf(szTemp, "mv /root/.ispddk/%s /root/.ispddk/%s.bak", szFilename, szFilename);
                    system(szTemp);
#if 0
                    if (0 == iIdx) {
                        sprintf(szTemp, "mv /tmp/%s /root/.ispddk", szFullPathFilenameTemp);
                    } else {
                        sprintf(szTemp, "mv %s /root/.ispddk", szFullPathFilenameTemp);
                    }
#else
                    if (0 != iIdx) {
                        sprintf(szTemp, "mv %s /tmp", szFullPathFilenameTemp);
                        system(szTemp);
                    }
                    sprintf(szTemp, "cp /tmp/%s /root/.ispddk", szFullPathFilenameTemp);
#endif
                    system(szTemp);
                    printf("ITTD server re-start videobox now...\n");
#if 0
                    videobox_stop();
                    videobox_start("/root/.videobox/path.json");
#elif 1
                    videobox_repath("/root/.videobox/path.json");
#else
                    videobox_stop();
                    system("videoboxd /root/.videobox/path.json");
#endif
                    break;
            }
            break;
        }

        case GEN_SET_GRID_DATA_FILE:
        {
            char szTemp[CMD_FILE_BUF_SIZE * 2];
            char *pszFilename = NULL;

            if ((GEN_SET_GRID_DATA_FILE_CMD_VER_MAX - 1) < pTcp->tcpcmd.ver) {
                printf("Out of support version. Ver = %lu\n", pTcp->tcpcmd.ver);
                return iRet;
            }
            switch (pTcp->tcpcmd.ver) {
                case GEN_SET_GRID_DATA_FILE_CMD_VER_V0:
                default:
                    strcpy(szFullPathFilenameTemp, (const char *)pTcp->buffer);
                    pszFilename = strrchr(szFullPathFilenameTemp, '/');
                    if (NULL != pszFilename) {
                        // The grid data file has been put to specify path.
                        // So, we don't need to do anything.
                    } else {
                        // Does not transmit the store path.
                        int iRet;
                        TCPCmdData stTcp;
                        PISPOST_MDLE_GRID_FILE_PATH pGrdiFilePath = NULL;
                        stTcp.tcpcmd.cmd.ui3Type = CMD_TYPE_ALL;
                        stTcp.tcpcmd.cmd.ui1Dir = CMD_DIR_GET;
                        stTcp.tcpcmd.mod = MODID_ISPOST_BASE + ISPOST_GRID_PATH;
                        stTcp.tcpcmd.datasize = sizeof(ISPOST_MDLE_GRID_FILE_PATH);
                        pGrdiFilePath = (ISPOST_MDLE_GRID_FILE_PATH*)((unsigned char*)stTcp.buffer);
                        pGrdiFilePath->szGridFilePath[0][0] = '\0';
                        pGrdiFilePath->szGridFilePath[1][0] = '\0';
                        iRet = camera_get_ispost("ispost", &stTcp, (sizeof(TCPCommand) + stTcp.tcpcmd.datasize));
                        if (0 != iRet) {
                            iRet = camera_get_ispost("ispost0", &stTcp, (sizeof(TCPCommand) + stTcp.tcpcmd.datasize));
                        }
                        if (0 != iRet) {
                            printf("Get grid file path failed.\n");
                            // Remove grid data file to avoid space not enough issue.
                            sprintf(szTemp, "rm /tmp/%s", szFullPathFilenameTemp);
                            system(szTemp);
                        } else {
                            //printf("szGridFilePath[0] = %s\n", pGrdiFilePath->szGridFilePath[0]);
                            //printf("szGridFilePath[1] = %s\n", pGrdiFilePath->szGridFilePath[1]);
                            // Move grid data file to json assign path.
                            if ('\0' != pGrdiFilePath->szGridFilePath[1][0]) {
                                sprintf(szTemp, "mv /tmp/%s %s%s",
                                    szFullPathFilenameTemp,
                                    pGrdiFilePath->szGridFilePath[1],
                                    szFullPathFilenameTemp
                                    );
                            } else if ('\0' != pGrdiFilePath->szGridFilePath[0][0]) {
                                sprintf(szTemp, "mv /tmp/%s %s%s",
                                    szFullPathFilenameTemp,
                                    pGrdiFilePath->szGridFilePath[0],
                                    szFullPathFilenameTemp
                                    );
                            } else {
                                printf("Failed, does not find and path to store grid file.\n");
                                // Remove grid data file to avoid space not enough issue.
                                sprintf(szTemp, "rm /tmp/%s", szFullPathFilenameTemp);
                            }
                            printf("%s", szTemp);
                            system(szTemp);
//                            printf("ITTD server re-start videobox now...\n");
//#if 0
//                            videobox_stop();
//                            videobox_start("/root/.videobox/path.json");
//#elif 1
//                            videobox_repath("/root/.videobox/path.json");
//#else
//                            videobox_stop();
//                            system("videoboxd /root/.videobox/path.json");
//#endif
                        }
                    }
                    break;
            }
            break;
        }

        case GEN_SET_ISP_LSH_FILE:
        {
            int iIdx;
            char szTemp[CMD_FILE_BUF_SIZE * 2];
            char * pszFilename = NULL;

            if ((GEN_SET_ISP_LSH_FILE_CMD_VER_MAX - 1) < pTcp->tcpcmd.ver) {
                printf("Out of support version. Ver = %lu\n", pTcp->tcpcmd.ver);
                return iRet;
            }
            switch (pTcp->tcpcmd.ver) {
                case GEN_SET_ISP_LSH_FILE_CMD_VER_V0:
                default:
                    strcpy(szFullPathFilenameTemp, (const char *)pTcp->buffer);
                    pszFilename = strrchr(szFullPathFilenameTemp, '/');
                    if (NULL != pszFilename) {
                        iIdx = &pszFilename[0] - &szFullPathFilenameTemp[0];
                        iIdx++;
                    } else {
                        iIdx = 0;
                    }
                    strcpy(szFilename, &szFullPathFilenameTemp[iIdx]);

                    sprintf(szFullPathFilename, "/root/.ispddk/%s", szFilename);
                    sprintf(szFullPathFilenameBackup, "/root/.ispddk/%s.bak", szFilename);
                    if (-1 != access(szFullPathFilenameBackup, 0)) {
                        sprintf(szTemp, "rm /root/.ispddk/%s.bak", szFilename);
                        system(szTemp);
                    }
                    sprintf(szTemp, "mv /root/.ispddk/%s /root/.ispddk/%s.bak", szFilename, szFilename);
                    system(szTemp);
#if 0
                    if (0 == iIdx) {
                        sprintf(szTemp, "mv /tmp/%s /root/.ispddk", szFullPathFilenameTemp);
                    } else {
                        sprintf(szTemp, "mv %s /root/.ispddk", szFullPathFilenameTemp);
                    }
#else
                    if (0 != iIdx) {
                        sprintf(szTemp, "mv %s /tmp", szFullPathFilenameTemp);
                        system(szTemp);
                    }
                    sprintf(szTemp, "cp /tmp/%s /root/.ispddk", szFullPathFilenameTemp);
#endif
                    system(szTemp);
//                    printf("ITTD server re-start videobox now...\n");
//#if 0
//                    videobox_stop();
//                    videobox_start("/root/.videobox/path.json");
//#elif 1
//                    videobox_repath("/root/.videobox/path.json");
//#else
//                    videobox_stop();
//                    system("videoboxd /root/.videobox/path.json");
//#endif
                    break;
            }
            break;
        }

        case GEN_SET_VIDEOBOX_CTRL:
        {
            if ((GEN_SET_VIDEOBOX_CTRL_CMD_VER_MAX - 1) < pTcp->tcpcmd.ver) {
                printf("Out of support version. Ver = %lu\n", pTcp->tcpcmd.ver);
                return iRet;
            }
            switch (pTcp->tcpcmd.ver) {
                case GEN_SET_VIDEOBOX_CTRL_CMD_VER_V0:
                default:
                    VIDEOBOX_CTRL * pVideoboxCtrl = (VIDEOBOX_CTRL *)pTcp->buffer;
                    memcpy((void *)&m_stVideoboxCtrl, (void *)pVideoboxCtrl, sizeof(VIDEOBOX_CTRL));
                    switch (m_stVideoboxCtrl.iMode) {
                        case VIDEOBOX_CTRL_MODE_START:
                            printf("ITTD server start videobox now...\n");
                            if (0 != strlen(m_stVideoboxCtrl.szJsonFilePath)) {
                                videobox_start(m_stVideoboxCtrl.szJsonFilePath);
                            } else {
                                videobox_start("/root/.videobox/path.json");
                            }
                            break;

                        case VIDEOBOX_CTRL_MODE_STOP:
                            printf("ITTD server stop videobox now...\n");
                            videobox_stop();
                            break;

                        case VIDEOBOX_CTRL_MODE_REBIND:
                            if ((0 != strlen(m_stVideoboxCtrl.szPort)) && (0 != strlen(m_stVideoboxCtrl.szTargetPort))) {
                                printf("ITTD server rebind videobox port %s %s now...\n", m_stVideoboxCtrl.szPort, m_stVideoboxCtrl.szTargetPort);
                                videobox_rebind(m_stVideoboxCtrl.szPort, m_stVideoboxCtrl.szTargetPort);
                            }
                            break;

                        case VIDEOBOX_CTRL_MODE_REPATH:
                            printf("ITTD server repath videobox now...\n");
                            if (0 != strlen(m_stVideoboxCtrl.szJsonFilePath)) {
                                videobox_repath(m_stVideoboxCtrl.szJsonFilePath);
                            } else {
                                videobox_repath("/root/.videobox/path.json");
                            }
                            break;
                    }
                    break;
            }
            break;
        }

        case GEN_SET_FILE:
        {
            int iLen;
            char * pszFilename = NULL;
            char szFilePath[CMD_FILE_BUF_SIZE * 2];
            char szTemp[CMD_FILE_BUF_SIZE * 2];

            if ((GEN_SET_FILE_CMD_VER_MAX - 1) < pTcp->tcpcmd.ver) {
                printf("Out of support version. Ver = %lu\n", pTcp->tcpcmd.ver);
                return iRet;
            }
            switch (pTcp->tcpcmd.ver) {
                case GEN_SET_FILE_CMD_VER_V0:
                default:
                    strcpy(&szFullPathFilenameTemp[0], (const char *)pTcp->buffer);
                    pszFilename = strrchr(szFullPathFilenameTemp, '/');
                    if (NULL != pszFilename) {
                        iLen = &pszFilename[0] - &szFullPathFilenameTemp[0];
                        iLen++;
                        strncpy(szFilePath, szFullPathFilenameTemp, iLen+1);
                        szFilePath[iLen] = '\0';
                        if (0 != strcmp(szFilePath, "/tmp")) {
                            sprintf(szTemp, "mv -f /tmp/%s %s", &pszFilename[1], szFullPathFilenameTemp);
                            system(szTemp);
                        } else {
                            // Keep this file in the /tmp folder.
                        }
                    } else {
                        // Keep this file in the /tmp folder.
                    }
                    break;
            }
            break;
        }

        case GEN_SET_REMOUNT_DEV_PARTITION:
        {
            if ((GEN_SET_VIDEOBOX_CTRL_CMD_VER_MAX - 1) < pTcp->tcpcmd.ver) {
                printf("Out of support version. Ver = %lu\n", pTcp->tcpcmd.ver);
                return iRet;
            }
            switch (pTcp->tcpcmd.ver) {
                case GEN_SET_REMOUNT_DEV_PARTITION_CMD_VER_V0:
                default:
                    RE_MOUNT_DEV_CTRL * pReMountDevCtrl = (RE_MOUNT_DEV_CTRL *)pTcp->buffer;
                    memcpy((void *)&m_stReMountDevCtrl, (void *)pReMountDevCtrl, sizeof(RE_MOUNT_DEV_CTRL));

                    // Flush the device cache buffer.
                    system("sync");

                    switch (m_stReMountDevCtrl.iMethod) {
                        case RE_MOUNT_METHOD_EXECUTE_SCRIPT:
                            printf("Executr script to re-mount dev and/or partition...\n");
                            if(!access(m_stReMountDevCtrl.szScriptDevPartitionName, F_OK))
                                system(m_stReMountDevCtrl.szScriptDevPartitionName);
                            break;

                        case RE_MOUNT_METHOD_DEV_PARTITION_NAME:
                            printf("Does not support at this version.\n");
                            break;
                    }
                    break;
            }
            break;
        }
    }

    return iRet;
}
