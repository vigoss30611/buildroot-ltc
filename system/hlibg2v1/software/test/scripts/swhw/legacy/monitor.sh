 #!/bin/bash

################################################################################
#
# Exits application after given timeout
#
# Requires:
# - bash, v3.2.17 tested
# - netkit rexec, v0.17 tested
# (- board_reboot.sh etc. on server)
#
#
# Returncodes
# return 0 : Board did not hang within given monitor timeout
# return 1 : Incorrect parameters 
# return 2 : Board was reset, ping timeout reached
# return 3 : Error resetting the board
################################################################################


################################################################################
#
# Global Parameters
#
# SERVER_IP - PC controlling the relay board
# SINGLE_PING_TIMEOUT -w option for single ping
# BOARD_POWER_UP_TIMEOUT : wait for this amount of time until declaring reboot attempt a failure
# BOARD_POWER_UP_RETRIES : try max n times to reboot the board, 0 is infinite
################################################################################

SERVER_IP="192.168.30.104"
SINGLE_PING_TIMEOUT="1"
BOARD_POWER_UP_TIMEOUT="90"
BOARD_POWER_UP_RETRIES="0"

echo num: "$#"

if [ $# -ne 4 ]; then
	echo "Usage: monitor.sh <ip> <board ping timeout, sec> <board is up pingtime, sec> <monitor timeout, sec>"
	echo ""
	echo "board ping timeout   : If board does not respond to several ping attempts in a given time, reset."
	echo "board is up pingtime : When board responds after reset to ping for a given time, board is considered ready"
	echo "monitor timeout      : Exit program in any case after this time"
	echo ""
	exit 1
fi

# Target IP
IP=$1
# Target ping timeout
pTime=$2
# Target ready time
pReady=$3

# monitor program total timeout
mTime=$4

echo "   IP: $IP   "
echo "pTime: $pTime"
echo "mTime: $mTime"

ret=0
start_time=`date +%s`
timeout_time=$(($start_time + $mTime))

echo "  start_time : $start_time"
echo "timeout_time : $timeout_time"

done=0
maybe_dead=0


################################################################################
#
# Makes sure board is up and running after reboot
#
# Parameters 
# 1 : Target IP
#
#
################################################################################

function prepare_board {
	target=$1
	give_up=0
	retries=0
	# Loop until board responds to a ping for a set amount of time
	# first boot responds to a ping in a bootload process. time: xxx sec
	# next board responds to a ping but is not ready for rexec. time xxx sec

	while [ $give_up -eq 0 ] ; do
		maybe_alive=0
		
		board_reboot $target

		curr_time=`date +%s`
		reboot_time=$(($curr_time + $BOARD_POWER_UP_TIMEOUT))

		echo "[$curr_time] : Rebooting. Retry: $retries/$BOARD_POWER_UP_RETRIES"
		echo "[$curr_time] : Timeout at $reboot_time"

		# Wait until board first responds to a ping
		# then start counting until board responds to ping (board ready time) for a given time
		# If this does not happen in given time (board reboot timeout) perform another board_reboot

		# Wait for a board reboot timeout
		
		while [ $curr_time -le $reboot_time ]; do
			#echo "Still time to try, $curr_time < $reboot_time"
			
			sleep 1
					
			output=`ping -c 1 $IP -w $SINGLE_PING_TIMEOUT`
			retval=$?
	
			#echo $retval
	
			# If board responds
			if [ $retval -ne 1 ] ; then
				if [ $maybe_alive -eq 0 ]; then
					maybe_alive=1
					reincarnation_time=$(($curr_time + $pReady))
					echo "[$curr_time] : Got PING! $pReady	s for reincarnation"
				else
					if [ $curr_time -le $reincarnation_time ]; then
						time_todo=$(($reincarnation_time - $curr_time))
						echo "[$curr_time] : Got PING. $time_todo s and we are up"
						# reset watchdog, in ping timeout is much bigger
						# than actual reboot timeout

						reboot_time=$(($curr_time + $BOARD_POWER_UP_TIMEOUT))

					else
						echo "[$curr_time] : The board has reincrnated."
						echo "Board needed $retries reboot retries."
						return 0
					fi
				fi
			else
			   retry_time=$(($reboot_time - $curr_time))
				echo "[$curr_time] : No PING. Resetting after $retry_time s"
				maybe_alive=0	
			fi

			curr_time=`date +%s`
		done

		if [ $BOARD_POWER_UP_RETRIES -eq 0 ]; then
			# Wait for infinity
			echo "[$curr_time] retry $retries/oo"
		else
			if [ $retries -lt $BOARD_POWER_UP_RETRIES ]; then
				# still time to try
				echo "try to reach $BOARD_POWER_UP_RETRIES retries"
			else
				# declare dead
				echo "give up, $BOARD_POWER_UP_RETRIES retries reached"
				give_up=1
			fi		
		fi
		retries=$(($retries + 1))
	done

	# Board did not come up after retries
	return 1
}

################################################################################
#
# Power-cycles board, if control server is available
#
# Parameters 
# 1 : Target IP
# Global: SERVER_IP
#
# If server cannot be found, return code is set
#
# Returncodes
#
# 0 All ok
# 1 server not found
#
################################################################################

function board_reboot {
	#echo "rexec -l rexec -p rexec "/usr/sbin/board_reboot.sh $1""
	#rexec -l rexec -p rexec "/usr/sbin/board_reboot.sh $1"

	output=`ping -c 1 $SERVER_IP -w 1`
	retval=$?

	if [ $retval -ne 0 ]; then
		echo "Board control server $SERVER_IP does not respond"
		return 1
	fi

	# MaKar edit
    #echo "rexec -l rexec -p rexec $SERVER_IP "/usr/sbin/board_reboot.shx 192.168.30.72""
	#result=`rexec -l rexec -p rexec $SERVER_IP "/usr/sbin/board_reboot.sh 192.168.30.72"`
    echo "rexec -l rexec -p rexec $SERVER_IP "/usr/sbin/board_reboot.shx $IP""
    result=`rexec -l rexec -p rexec $SERVER_IP "/usr/sbin/board_reboot.sh $IP"`
    # end MaKar edit
	retcode=$?
	echo ""
	echo "rexec returned:"
	echo "res: $result"
	echo "ret: $retcode"
	
	# 0 : rexec ok
	# 
	
	if [ $result -ne 0 ]; then
		echo "Error running board_reboot.sh"
		echo "$result"
	fi
	
	return 0 
}


################################################################################
#
# Main
#
################################################################################

while [ $done -ne 1 ]; do
    sleep 1
	
	# wait 5 sec for each ping packet response
	
	output=`ping -c 1 $IP -w $SINGLE_PING_TIMEOUT`
	retval=$?
	
	#echo "${retval}: ${output}"
    #echo "${retval}"

	curr_time=`date +%s`
	
	# If board did not respond, start counting
	if [ $retval -ne 0 ] ; then
		if [ $maybe_dead -eq 0 ]; then
			echo "[$curr_time] : Board not responding, start counting for knockout"
			maybe_dead=1
			maybe_dead_time=`date +%s`
			knockout_time=$(($maybe_dead_time + $pTime))
		else
			time_left=$(($knockout_time - $curr_time))
			echo "[$curr_time] : Maybe dead. $time_left for Knockout"
			maybe_dead=1
			if [ $knockout_time -le $curr_time ]; then
				echo "[$curr_time] : KO. prepare board"
				
				prepare_board $IP
				retval=$?

				#echo retval: $retval
				echo "exit 2"
				exit 2
			fi
		fi
	else
		#echo "[$curr_time] : Board is alive"

		if [ $maybe_dead -eq 1 ]; then
			echo "[$curr_time] : False hope. reset counting"
			maybe_dead=0
		fi
	fi

	mTime_left=$(($timeout_time - $curr_time))	
	#echo "[$curr_time] : monitor.sh timeout after $mTime_left s"
	
	# Board did not hang within given monitor timeout
	if [ $timeout_time -le $curr_time ]; then
		done=1
		ret=0
        # MaKar edit
        echo "[$curr_time] : Boot forced!"
        prepare_board $IP
        # end MaKar edit
	fi
done

echo "exit"
exit $ret

################################################################################
#
#
#
#
################################################################################
