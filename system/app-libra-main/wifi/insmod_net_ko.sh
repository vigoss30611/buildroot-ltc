#!/bin/sh

modprobe  ansi_cprng.ko
modprobe  mii.ko
modprobe  libphy.ko
modprobe  tun.ko
modprobe  act_gact.ko
modprobe  cls_u32.ko
modprobe  xt_mark.ko
modprobe  nf_defrag_ipv4.ko
modprobe  arp_tables.ko
modprobe  arpt_mangle.ko
modprobe  arptable_filter.ko
modprobe  xfrm4_mode_beet.ko
modprobe  xfrm4_mode_transport.ko
modprobe  xfrm4_mode_tunnel.ko
modprobe  inet_diag.ko
modprobe  tcp_diag.ko
modprobe  tcp_bic.ko
modprobe  tcp_cubic.ko
modprobe  tcp_westwood.ko
modprobe  tcp_htcp.ko
modprobe  xfrm_algo.ko
modprobe  xfrm_user.ko
modprobe  xfrm_ipcomp.ko
modprobe  cfg80211.ko
modprobe  lib80211.ko
modprobe  af_packet.ko
modprobe  af_key.ko
modprobe  rfkill-regulator.ko
modprobe  hostap.ko
modprobe  mac80211.ko

ln -s /mnt1/libexec/ /libexec
