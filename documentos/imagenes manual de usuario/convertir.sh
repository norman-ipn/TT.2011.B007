#!/bin/bash

for i in *.xcf
do
  convert "$i" "$i.ps"
done
