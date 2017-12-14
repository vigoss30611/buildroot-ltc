*******************************************************************************
*******************************************************************************
*********** This is a step by step guide to H1 encoder testing!  ************
*******************************************************************************
*******************************************************************************

1. Checking out, configuring and compiling SW.

1.0 Access to lab testing file server

    - You should have access to hantro lab network
    - This is where all the testing takes place
    - ssh hlabws3
    - cd to your own working directory export/work/user/
    - check lab monitor at http://172.28.107.67

1.1 Checking SW out from version control.

    - H1 project git repository is at:
        /auto/hantro-projects/gforce/GIT/h1_encoder

    - First you have to clone the working tree from the git main repository:

    > git clone -n /auto/hantro-projects/gforce/GIT/h1_encoder

    - You can see all the tags in the main tree with command

    > cd h1_encoder
    > git tag

    - Then you need to check out the correct tag and make a branch of it

    > git checkout -b branch_x_x ench1_x_x

    - Normally you get Software versions from tag mails you get
        from SW guys (SaPi). Tags can also be found from
        PWA at H1 project site

1.2 Configuring and compiling SW

    - Go to dir with lab scripts, this is where they are run in:

    > cd software/linux_reference/test/common

    - Make sure that the permissions are correct:

    > chmod a+wx .

    - Open commonconfig.sh and edit the following lines:

        ->export swtag="ench1_x_x"        # This is the tag with which you checked out the software from version control

        ->export hwtag="ench1_x_x"        # This is the HW version that you are testing, you usually get it from tag mails, otherwise ask Ari Hautala.

        ->export systag="ench1_x_x"       # This is the system model which corresponds to the software and hardware tags.

        ->export testdeviceip=75          # Set IP for test device, check lab monitor

        ->COMPILER_SETTINGS=""            # Board specific value, this can be seen from board status monitor

        ->DWL_IMPLEMENTATION="POLLING"     #Then you have to decide do you use IRQ or POLLING mode. IRQ's do not
                                           #work with all the boards yet, but normally they are recommended.
                                           #AXI versatile boards do not support IRQs yet so with
                                           #those you have to use polling mode.

        ->INTERNAL_TEST="y"         #With this flag you can either enable or disable internal
                                    #test cases from tests. Should be n with customer
                                    #releases, otherwise y

        ->MAX_WIDTH=""              #You can see from the hardware tag, which resolution
        ->MAX_HEIGHT=""             #this configuration supports.

        ->csv_path=""              #The directory where test reports are generated.

    - Run script ./set.sh

    - This script cleans the test directory, changes all the necessary software
        parameters automatically, compiles the software and test benches,
        and copies all the test scripts and binaries
        to the test directory.



2 Laboratory testing.

    - First you have to log in to test board from HLABWS.

    > telnet 172.28.107.xx
    > username -> root

    - Verify that the tag corresponds to the
        one in RTL, this can be checked from the hw base address when logged on to
        the board:
           > dm2 0xC0000000 (Versatile board),
                 0xC4000000 (Axi Versatile board) or
                 0x84000000 (Emulation Board)

#### NEW .py scripts

    - On HLABWS run:
    > ./testcaselist.sh                     # Creates task/run_xxxx.sh script for every possible case
    > python create_tasklist.py task/list \ # Creates task/list with case numbers
             all size-order \               # All valid cases in size-order
             width<1981 rotation=0 \        # Take only cases within limits
             videoStab=0 inputFormat<5      # Possibility to limit any run.sh parameter
    > ./create_reference.sh task/list       # Creates sys_ench1_xx_yy/case_zz references

    - When references are being created, you can start to run the test cases on FPGA:
    > ./run_task.sh task/list

    - Checking on HLABWS:
    > ./check_task.sh task/list


#### OLD .sh scripts

    - Go to the /export/work/user/h1_encoder/software/linux_reference/test/common and run script

    > ./runall.sh

    - This script starts by loading the memalloc and driver modules to the board.

        - If something goes wrong probable reason is wrong kernel,
            wrong base address or wrong memory start address.
            Return value -1 normally means wrong kernel
        - You can check the board's kernel from http://192.168.30.104/monitor/
            so that it is the same you defined for the sw.

    - Then it runs a small test set "smoketest" to ensure that the test
        environment is set up correctly.
    - Each case run creates data into case_xxx directory

    - Open a new terminal, cd to your export working directory h1_encoder/software/linux_reference/test/common

    - Run the script

    > ./checkall.sh

        to check the results. This must be running at the same time as runall.sh
        because the testing and checking are done parallel.

    - This script runs the system model in sys_x_x, creates the reference data
        in sys_x_x/case_xxx and compares the data in case_xxx to sys_x_x/case_xxx

    - If everything goes ok, just wait for the tests to finish,
        this usually lasts until the next day.

    - If something goes wrong, check the file commonconfig.sh and start again
        from running set.sh.

    - Check test report files: integrationreport_format_tag_time.csv
        which are copied to /auto/hantro-projects/gforce/integration/test_reports

    - If some tests fail you can rerun them by running clean.sh to remove
      failed cases. Then run the runall and checkall scripts again.

    - Optionally you can also run the movie test lasting hours:
    > ./movie_decode.sh moviefile.h264 on PC to decode a movie and then
    > ./movie_encode.sh on board to encode it

    - Update tag comment in Project Web Access > H1 > Tags:
        summary of tag testing, number of failed cases and reasons

