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
  print "{"
  for (asic = 0; asic < NUM_ASIC; ++asic) {
    num = asic + 1;
    printf("\"asic_%d\": {\n", num);

    for (dcdc = 0; dcdc < NUM_DCDC; ++dcdc) {
      vout = "";
      if ("" != asic_dcdcVout[asic, dcdc])
        vout = (0.0 + asic_dcdcVout[asic, dcdc]);
      if (vout <= 0)
        vout_s = "";
      else
        vout_s = sprintf("%.4f", vout);
      print "\t\"dcdc" dcdc "_Vout\": \"" vout_s "\","

      iout = "";
      if ("" != asic_dcdcIout[asic, dcdc])
        iout = (0.0 + asic_dcdcIout[asic, dcdc]);
      if (iout <= 0)
        iout_s = "";
      else
        iout_s = sprintf("%.4f", iout);
      print "\t\"dcdc" dcdc "_Iout\": \"" iout_s "\","

      temp = "";
      if ("" != asic_dcdctemp[asic, dcdc])
        temp = (0.0 + asic_dcdctemp[asic, dcdc]);
      if (temp <= 0)
        temp_s = "";
      else
        temp_s = sprintf("%.1f", temp);
      print "\t\"dcdc" dcdc "_Temp\": \"" temp_s "\","
    }

    for (die = 0; die < NUM_DIE; ++die) {
      freq = "";
      if ("" != asic_freq[asic, die])
        freq = (0 + asic_freq[asic, die]);
      if (freq <= 0)
        freq = "";
      print "\t\"die" die "_Freq\": \"" freq "\",";

      volt = "";
      if ("" != asic_volt[asic, die])
        volt = (0.0 + asic_volt[asic, die]);
      if (volt <= -1000)
        volt_s = "";
      else
        volt_s = sprintf("%.4f", volt);
      print "\t\"die" die "_Voffset\": \"" volt_s "\",";
    }

    temp = "";
    if ("" != asic_temp[asic])
      temp = (0.0 + asic_temp[asic]);
    if (temp <= 0)
      temp_s = "";
    else
      temp_s = sprintf("%.1f", temp);
    print "\t\"temperature\": \"" temp_s "\"";

    if (asic < (NUM_ASIC - 1))
      print "},"
    else
      print "}"
  }
  print "}"
}
