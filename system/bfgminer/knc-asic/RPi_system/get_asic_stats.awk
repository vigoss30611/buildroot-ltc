#!/usr/bin/awk -f
BEGIN {
  NUM_ASIC = 6;
  NUM_DIE = 4;
  NUM_DCDC = 8;
  for (asic = 0; asic < NUM_ASIC; ++asic) {
    asic_temp[asic] = "";
    asic_type[asic] = "OFF";
    for (die = 0; die < NUM_DIE; ++die) {
      asic_freq[asic, die] = 0;
    }
    for (dcdc = 0; dcdc < NUM_DCDC; ++dcdc)
      asic_dcdctemp[asic, dcdc] = "";
  }
  cur_asic = 0;
 }

/ASICCACHE/ { asic = $2;
  asic_type[asic] = $3;
  asic_temp[asic] = $4;
  for (die = 0; die < NUM_DIE; ++die) {
    asic_freq[asic, die] = $(5 + die*2);
    asic_volt[asic, die] = $(5 + die*2 + 1);
  }
  for (dcdc = 0; dcdc < NUM_DCDC; ++dcdc) {
    asic_dcdctemp[asic, dcdc] = $(5 + NUM_DIE*2 + dcdc*3);
    asic_dcdcVout[asic, dcdc] = $(5 + NUM_DIE*2 + dcdc*3 + 1);
    asic_dcdcIout[asic, dcdc] = $(5 + NUM_DIE*2 + dcdc*3 + 2);
  }
}

END {
  for (asic = 0; asic < NUM_ASIC; ++asic) {
    num = asic + 1;
    temp = "---";
    if ("" != asic_temp[asic])
      temp = (0.0 + asic_temp[asic]);
    if (temp <= 0)
      temp = "---";
    freq = 0.0;
    for (die = 0; die < NUM_DIE; ++die) {
      freq += asic_freq[asic, die];
    }
    freq /= 4.0;
    if (freq <= 0)
      freq = "---";
    dcdctemp = 0;
    dcdcnum = 0;
    for (dcdc = 0; dcdc < NUM_DCDC; ++dcdc) {
      if ("" == asic_dcdctemp[asic, dcdc])
        continue;
      dcdctemp += asic_dcdctemp[asic, dcdc];
      dcdcnum++;
    }
    if ((0 == dcdcnum) || (dcdctemp <= 0))
      dcdctemp = "---";
    else
      dcdctemp = sprintf("%.1f", dcdctemp / dcdcnum);
    print num, temp, dcdctemp, freq, asic_type[asic];
  }
}
