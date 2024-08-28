#!/bin/bash
#Script to run perf test on authprovvalidator uservice
#Run with the location of injector tool as first argument

INJECTOR_DIR=$1

#Step 1: compile with debug info and prepare uservice needs
mason.py -build -target debug -no-sonar-report

CURRENT_DIR=$(pwd)
export VALIDATORPORT=9002 OAISCHEMAFILE=$CURRENT_DIR/../../schema/authprovvalidator.yaml LOGCONTROLPATH=$CURRENT_DIR/authlog.json

#Step 2: perf configurations
sudo sysctl kernel.perf_event_paranoid=-1
sudo sysctl kernel.kptr_restrict=0

#Step 3: run perf along with uservice
cd ../../.debug/bin
perf record -g -s ./Authenticationprovisioningvalidator.usvc.elf &

#Step 4: Inject traffic
cd $INJECTOR_DIR
auth=$(./injector.usvc.elf -u 'http://127.0.0.1:9002/validation/v1/validate/validate' -m POST -f 2000 -d 3600 -j $CURRENT_DIR/authprovdata.json  -M 10)


#step 4: store injector results
cd $CURRENT_DIR
echo "$auth" > authresults.txt

#kill perf process
kill -INT $(ps -u | grep linux-tools | awk '{print $2}')
kill -INT $(ps -u | grep ./Authenticationprovisioningvalidator.usvc.elf | awk '{print $2}')
