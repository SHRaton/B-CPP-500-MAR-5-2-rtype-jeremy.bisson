#!/bin/bash

cd ../build

for i in {1..20}
do
    ./client 127.0.0.1 8081
done
