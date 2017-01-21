#!/usr/bin/perl

#
# Parse the grid data and build structure for look-up
#
# Copyright (C) 2016 James Dougherty <jfd@cs.stanford.edu>
#
# http://www.ngdc.noaa.gov/geomag-web/#igrfgrid
#
# For Continental North America, Greenland, tip of South America
# Lat: Min=11, Max=73
# Lon: Min=169, Max=23
# Declination
# CSV Output
# WMM 2014-2019


print "typedef struct magdec_correction_s{\n";
print "    int year;\n";
print "    int lat;\n";
print "    int lon;\n";
print "    float magdec;\n";
print "} magdec_correction_t;\n";

print "magdec_correction_t corr[] = { \n";
while(my $row = <>){
    chomp $row;
#    print $row, "\n";
    ($date, $lat, $lon, $h, $dec, $dec_sv, $dec_unc) = split(',', $row);
    printf("\t{%d, %d, %d, %2.2f},\n", $date, $lat, $lon, $dec);
#    print "date:", $date, " (%\n";
}
print"};\n";
