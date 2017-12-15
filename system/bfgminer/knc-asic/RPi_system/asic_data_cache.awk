#!/usr/bin/awk -f
BEGIN {
  NUM_ASIC = 6;
  NUM_DIE = 4;
  NUM_DCDC = 8;
  STALE_COUNT = 4;
  for (asic = 0; asic < NUM_ASIC; ++asic) {
    asic_temp[asic] = "";
    asic_temp_cnt[asic] = 0;
    asic_type[asic] = "";
    asic_type_cnt[asic] = 0;
    for (die = 0; die < NUM_DIE; ++die) {
        asic_freq[asic, die] = "";
        asic_freq_cnt[asic, die] = 0;
        asic_volt[asic, die] = "";
        asic_volt_cnt[asic, die] = 0;
    }
    for (dcdc = 0; dcdc < NUM_DCDC; ++dcdc) {
      asic_dcdctemp[asic, dcdc] = "";
      asic_dcdctemp_cnt[asic, dcdc] = 0;
      asic_dcdcVout[asic, dcdc] = "";
      asic_dcdcVout_cnt[asic, dcdc] = 0;
      asic_dcdcIout[asic, dcdc] = "";
      asic_dcdcIout_cnt[asic, dcdc] = 0;
    }
  }
  cur_asic = 0;
 }

/"asic_1"/ { cur_asic = 0 }
/"asic_2"/ { cur_asic = 1 }
/"asic_3"/ { cur_asic = 2 }
/"asic_4"/ { cur_asic = 3 }
/"asic_5"/ { cur_asic = 4 }
/"asic_6"/ { cur_asic = 5 }

/"die0_Freq"/ { gsub(/"/, "", $2); gsub(/,/, "", $2); asic_freq[cur_asic, 0] = $2 }
/"die1_Freq"/ { gsub(/"/, "", $2); gsub(/,/, "", $2); asic_freq[cur_asic, 1] = $2 }
/"die2_Freq"/ { gsub(/"/, "", $2); gsub(/,/, "", $2); asic_freq[cur_asic, 2] = $2 }
/"die3_Freq"/ { gsub(/"/, "", $2); gsub(/,/, "", $2); asic_freq[cur_asic, 3] = $2 }

/"die0_Voffset"/ { gsub(/"/, "", $2); gsub(/,/, "", $2); asic_volt[cur_asic, 0] = $2 }
/"die1_Voffset"/ { gsub(/"/, "", $2); gsub(/,/, "", $2); asic_volt[cur_asic, 1] = $2 }
/"die2_Voffset"/ { gsub(/"/, "", $2); gsub(/,/, "", $2); asic_volt[cur_asic, 2] = $2 }
/"die3_Voffset"/ { gsub(/"/, "", $2); gsub(/,/, "", $2); asic_volt[cur_asic, 3] = $2 }

/"dcdc0_Temp"/ { gsub(/"/, "", $2); gsub(/,/, "", $2); asic_dcdctemp[cur_asic, 0] = $2 }
/"dcdc1_Temp"/ { gsub(/"/, "", $2); gsub(/,/, "", $2); asic_dcdctemp[cur_asic, 1] = $2 }
/"dcdc2_Temp"/ { gsub(/"/, "", $2); gsub(/,/, "", $2); asic_dcdctemp[cur_asic, 2] = $2 }
/"dcdc3_Temp"/ { gsub(/"/, "", $2); gsub(/,/, "", $2); asic_dcdctemp[cur_asic, 3] = $2 }
/"dcdc4_Temp"/ { gsub(/"/, "", $2); gsub(/,/, "", $2); asic_dcdctemp[cur_asic, 4] = $2 }
/"dcdc5_Temp"/ { gsub(/"/, "", $2); gsub(/,/, "", $2); asic_dcdctemp[cur_asic, 5] = $2 }
/"dcdc6_Temp"/ { gsub(/"/, "", $2); gsub(/,/, "", $2); asic_dcdctemp[cur_asic, 6] = $2 }
/"dcdc7_Temp"/ { gsub(/"/, "", $2); gsub(/,/, "", $2); asic_dcdctemp[cur_asic, 7] = $2 }

/"dcdc0_Vout"/ { gsub(/"/, "", $2); gsub(/,/, "", $2); asic_dcdcVout[cur_asic, 0] = $2 }
/"dcdc1_Vout"/ { gsub(/"/, "", $2); gsub(/,/, "", $2); asic_dcdcVout[cur_asic, 1] = $2 }
/"dcdc2_Vout"/ { gsub(/"/, "", $2); gsub(/,/, "", $2); asic_dcdcVout[cur_asic, 2] = $2 }
/"dcdc3_Vout"/ { gsub(/"/, "", $2); gsub(/,/, "", $2); asic_dcdcVout[cur_asic, 3] = $2 }
/"dcdc4_Vout"/ { gsub(/"/, "", $2); gsub(/,/, "", $2); asic_dcdcVout[cur_asic, 4] = $2 }
/"dcdc5_Vout"/ { gsub(/"/, "", $2); gsub(/,/, "", $2); asic_dcdcVout[cur_asic, 5] = $2 }
/"dcdc6_Vout"/ { gsub(/"/, "", $2); gsub(/,/, "", $2); asic_dcdcVout[cur_asic, 6] = $2 }
/"dcdc7_Vout"/ { gsub(/"/, "", $2); gsub(/,/, "", $2); asic_dcdcVout[cur_asic, 7] = $2 }

/"dcdc0_Iout"/ { gsub(/"/, "", $2); gsub(/,/, "", $2); asic_dcdcIout[cur_asic, 0] = $2 }
/"dcdc1_Iout"/ { gsub(/"/, "", $2); gsub(/,/, "", $2); asic_dcdcIout[cur_asic, 1] = $2 }
/"dcdc2_Iout"/ { gsub(/"/, "", $2); gsub(/,/, "", $2); asic_dcdcIout[cur_asic, 2] = $2 }
/"dcdc3_Iout"/ { gsub(/"/, "", $2); gsub(/,/, "", $2); asic_dcdcIout[cur_asic, 3] = $2 }
/"dcdc4_Iout"/ { gsub(/"/, "", $2); gsub(/,/, "", $2); asic_dcdcIout[cur_asic, 4] = $2 }
/"dcdc5_Iout"/ { gsub(/"/, "", $2); gsub(/,/, "", $2); asic_dcdcIout[cur_asic, 5] = $2 }
/"dcdc6_Iout"/ { gsub(/"/, "", $2); gsub(/,/, "", $2); asic_dcdcIout[cur_asic, 6] = $2 }
/"dcdc7_Iout"/ { gsub(/"/, "", $2); gsub(/,/, "", $2); asic_dcdcIout[cur_asic, 7] = $2 }

/BOARD0=/ { gsub(/BOARD0=/, "", $1); asic_type[0] = $1; }
/BOARD1=/ { gsub(/BOARD1=/, "", $1); asic_type[1] = $1; }
/BOARD2=/ { gsub(/BOARD2=/, "", $1); asic_type[2] = $1; }
/BOARD3=/ { gsub(/BOARD3=/, "", $1); asic_type[3] = $1; }
/BOARD4=/ { gsub(/BOARD4=/, "", $1); asic_type[4] = $1; }
/BOARD5=/ { gsub(/BOARD5=/, "", $1); asic_type[5] = $1; }

/"temperature"/ { gsub(/"/, "", $2); gsub(/,/, "", $2); asic_temp[cur_asic] = $2 }

/ASICCACHE/ { asic = $2;
  asic_type_cache[asic] = $3;
  asic_temp_cache[asic] = $4;
  for (die = 0; die < NUM_DIE; ++die) {
    asic_freq_cache[asic, die] = $(5 + die*2);
    asic_volt_cache[asic, die] = $(5 + die*2 + 1);
  }
  for (dcdc = 0; dcdc < NUM_DCDC; ++dcdc) {
    asic_dcdctemp_cache[asic, dcdc] = $(5 + NUM_DIE*2 + dcdc*3);
    asic_dcdcVout_cache[asic, dcdc] = $(5 + NUM_DIE*2 + dcdc*3 + 1);
    asic_dcdcIout_cache[asic, dcdc] = $(5 + NUM_DIE*2 + dcdc*3 + 2);
  }
}

/STALECOUNTERS/ { asic = $2;
  asic_type_cnt[asic] = $3;
  asic_temp_cnt[asic] = $4;
  for (die = 0; die < NUM_DIE; ++die) {
    asic_freq_cnt[asic, die] = $(5 + die*2);
    asic_volt_cnt[asic, die] = $(5 + die*2 + 1);
  }
  for (dcdc = 0; dcdc < NUM_DCDC; ++dcdc) {
    asic_dcdctemp_cnt[asic, dcdc] = $(5 + NUM_DIE*2 + dcdc*3);
    asic_dcdcVout_cnt[asic, dcdc] = $(5 + NUM_DIE*2 + dcdc*3 + 1);
    asic_dcdcIout_cnt[asic, dcdc] = $(5 + NUM_DIE*2 + dcdc*3 + 2);
  }
}

END {
  for (asic = 0; asic < NUM_ASIC; ++asic) {
    if ("" != asic_type[asic]) {
      type = asic_type[asic];
      asic_type_cnt[asic] = 0;
    } else {
      if (++asic_type_cnt[asic] > STALE_COUNT) {
        asic_type_cnt[asic] = STALE_COUNT + 1;
        type = "OFF";
      } else {
        type = asic_type_cache[asic];
      }
    }
    if ("" != asic_temp[asic]) {
      temp = (0.0 + asic_temp[asic]);
      asic_temp_cnt[asic] = 0;
    } else {
      if (++asic_temp_cnt[asic] > STALE_COUNT) {
        asic_temp_cnt[asic] = STALE_COUNT + 1;
        temp = 0.0;
      } else {
        temp = asic_temp_cache[asic];
      }
    }
    for (die = 0; die < NUM_DIE; ++die) {
      if ("" != asic_freq[asic, die]) {
        freq[die] = (0.0 + asic_freq[asic, die]);
        asic_freq_cnt[asic, die] = 0;
      } else {
        if (++asic_freq_cnt[asic, die] > STALE_COUNT) {
          asic_freq_cnt[asic, die] = STALE_COUNT + 1;
          freq[die] = 0.0;
        } else {
          freq[die] = asic_freq_cache[asic, die];
        }
      }
      if ("" != asic_volt[asic, die]) {
        volt[die] = (0.0 + asic_volt[asic, die]);
        asic_volt_cnt[asic, die] = 0;
      } else {
        if (++asic_volt_cnt[asic, die] > STALE_COUNT) {
          asic_volt_cnt[asic, die] = STALE_COUNT + 1;
          volt[die] = 0.0;
        } else {
          volt[die] = asic_volt_cache[asic, die];
        }
      }
    }
    for (dcdc = 0; dcdc < NUM_DCDC; ++dcdc) {
      if ("" != asic_dcdctemp[asic, dcdc]) {
        dcdctemp[dcdc] = (0.0 + asic_dcdctemp[asic, dcdc]);
	asic_dcdctemp_cnt[asic, dcdc] = 0;
      } else {
        if (++asic_dcdctemp_cnt[asic, dcdc] > STALE_COUNT) {
          asic_dcdctemp_cnt[asic, dcdc] = STALE_COUNT + 1;
          dcdctemp[dcdc] = 0.0;
        } else {
          dcdctemp[dcdc] = asic_dcdctemp_cache[asic, dcdc];
        }
      }
      if ("" != asic_dcdcVout[asic, dcdc]) {
        dcdcVout[dcdc] = (0.0 + asic_dcdcVout[asic, dcdc]);
	asic_dcdcVout_cnt[asic, dcdc] = 0;
      } else {
        if (++asic_dcdcVout_cnt[asic, dcdc] > STALE_COUNT) {
          asic_dcdcVout_cnt[asic, dcdc] = STALE_COUNT + 1;
          dcdcVout[dcdc] = 0.0;
        } else {
          dcdcVout[dcdc] = asic_dcdcVout_cache[asic, dcdc];
        }
      }
      if ("" != asic_dcdcIout[asic, dcdc]) {
        dcdcIout[dcdc] = (0.0 + asic_dcdcIout[asic, dcdc]);
	asic_dcdcIout_cnt[asic, dcdc] = 0;
      } else {
        if (++asic_dcdcIout_cnt[asic, dcdc] > STALE_COUNT) {
          asic_dcdcIout_cnt[asic, dcdc] = STALE_COUNT + 1;
          dcdcIout[dcdc] = 0.0;
        } else {
          dcdcIout[dcdc] = asic_dcdcIout_cache[asic, dcdc];
        }
      }
    }
    printf("ASICCACHE %d %s %.1f", asic, type, temp);
    for (die = 0; die < NUM_DIE; ++die) {
      printf(" %d %.4f", freq[die], volt[die]);
    }
    for (dcdc = 0; dcdc < NUM_DCDC; ++dcdc) {
      printf(" %.1f %.4f %.4f", dcdctemp[dcdc], dcdcVout[dcdc], dcdcIout[dcdc]);
    }
    printf("\nSTALECOUNTERS %d %d %d", asic, asic_type_cnt[asic], asic_temp_cnt[asic]);
    for (die = 0; die < NUM_DIE; ++die) {
      printf(" %d %d", asic_freq_cnt[asic, die], asic_volt_cnt[asic, die]);
    }
    for (dcdc = 0; dcdc < NUM_DCDC; ++dcdc) {
      printf(" %d %d %d", asic_dcdctemp_cnt[asic, dcdc], asic_dcdcVout_cnt[asic, dcdc], asic_dcdcIout_cnt[asic, dcdc]);
    }
    printf("\n");
  }
}
