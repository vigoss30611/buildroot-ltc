#!/bin/sh

insmod /mnt1/ko/ansi_cprng.ko 
insmod /mnt1/ko/mii.ko	
insmod /mnt1/ko/libphy.ko	
insmod /mnt1/ko/tun.ko	
insmod /mnt1/ko/act_gact.ko	
insmod /mnt1/ko/cls_u32.ko	
insmod /mnt1/ko/xt_mark.ko	
insmod /mnt1/ko/nf_defrag_ipv4.ko	
insmod /mnt1/ko/arp_tables.ko	
insmod /mnt1/ko/arpt_mangle.ko	
insmod /mnt1/ko/arptable_filter.ko	
insmod /mnt1/ko/xfrm4_mode_beet.ko	
insmod /mnt1/ko/xfrm4_mode_transport.ko	
insmod /mnt1/ko/xfrm4_mode_tunnel.ko	
insmod /mnt1/ko/inet_diag.ko	
insmod /mnt1/ko/tcp_diag.ko	
insmod /mnt1/ko/tcp_bic.ko	
insmod /mnt1/ko/tcp_cubic.ko	
insmod /mnt1/ko/tcp_westwood.ko	
insmod /mnt1/ko/tcp_htcp.ko	
insmod /mnt1/ko/xfrm_algo.ko	
insmod /mnt1/ko/xfrm_user.ko	
insmod /mnt1/ko/xfrm_ipcomp.ko	
insmod /mnt1/ko/cfg80211.ko	
insmod /mnt1/ko/lib80211.ko	
insmod /mnt1/ko/af_packet.ko	
insmod /mnt1/ko/af_key.ko	
insmod /mnt1/ko/rfkill-regulator.ko	
insmod /mnt1/ko/hostap.ko
insmod /mnt1/ko/mac80211.ko

ln -s /mnt1/libexec/ /libexec
