#!/bin/bash

export LANG=C
avrogencpp -p backend/AvroCpp/api -n rmf_raw_avro2 -i src/backend/avro2/Frame.json -o src/backend/avro2/raw_frame.h
sed -i.old "s#namespace avro#namespace rmf_avro#g" src/backend/avro2/raw_frame.h
sed -i.old "s#avro::#rmf_avro::#g" src/backend/avro2/raw_frame.h