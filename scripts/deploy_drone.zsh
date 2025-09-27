#!/bin/bash
# scripts/deploy_drone.sh

echo "Deploying C drone firmware..."
cd drone_firmware

# Compile for drone architecture (example for ARM)
make clean
make CC=arm-linux-gnueabihf-gcc

# Deploy to drone via network or SD card
scp drone_networking drone_ip:/opt/drone/

echo "Drone firmware deployed!"