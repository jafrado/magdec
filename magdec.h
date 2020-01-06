#ifndef __MAGDEC__H
#define __MAGDEC__H
/***
 * 
 * @mainpage Magnetic Declination Corrector
 * 
 * The World Magnetic Model is the standard model used by the
 * U.S. Department of Defense, the U.K. Ministry of Defence, the North
 * Atlantic Treaty Organization (NATO) and the International
 * Hydrographic Organization (IHO), for navigation, attitude and
 * heading referencing systems using the geomagnetic field present on
 * the planet Earth. It is also used widely in civilian navigation and
 * heading systems. The model, associated software, and documentation
 * are distributed by NCEI on behalf of NGA. The model is produced at
 * 5-year intervals, with the current model expiring on December 31,
 * 2019.
 * 
 * This module uses a table generated from the online NOAA Magnetic
 * field calculator which uses a grid approach for the data and provides
 * CSV output tabulated such that it may be parsed by a simple Perl
 * script as below.
 * 
 * This module provides a mini magnetic declination corrector suitable
 * for embedded systems which may need to provide accurate magnetometer 
 * readings and not necessarily able to compute using the WMM model. 
 * 
 * To use, go here and input your coords
 * http://www.ngdc.noaa.gov/geomag-web/#igrfgrid
 * 
 *  For Continental North America, Greenland, tip of South America
 * 
 * Lat: Min=11, Max=73
 * Lon: Min=169, Max=23
 * Declination
 * CSV Output
 * WMM 2014-2019
 * 
 * (Pick a start/end date)
 * 
 * Export the CSV file and save it (e.g. igrfgridData.csv)
 * 
 * For the whole planet Earth:
 * 
 
 * Lat: Min = 90S, Max = 90 N
 * Lon: Min = 180W, Max = 180E
 * Elevation - Mean Sea Level
 * Declination
 * CSV Output
 * WMM 2014-2019
 *
 * Export the CSV file and save it (e.g. igrfgridData.csv)
 *
 * 
 * 
 * 
 * Run the below:
 * 
 * ./parse_mag_dec.pl igrfgridData.csv 
 * 
 * Insert the resulting table output below into this code
 * 
 * @author James Dougherty <jfd@cs.stanford.edu>
 * 
 */



/* see earthxx.c */
typedef struct magdec_correction_s{
    int year;
    int lat;
    int lon;
    float magdec;
} magdec_correction_t;

/* WMM 2015 data - 2017 to 2019*/
extern magdec_correction_t corr[]; 
extern int n_magdec_entries;

/* WMM 2020 data - 2019 to 2024 */
extern magdec_correction_t corr_2020[]; 
extern int n_magdec_entries_2020;

/** 
 * @brief Look up the magnetic compass declination angle at a given
 * latitude/longitude for compass correction
 * 
 *  @param lat the latitude for the location to lookup (decimal degrees)
 *  @param lon the longitude for the location to lookup (decimal degrees)
 *  @param year the year for which the declination angle is needed
 *  @return The magnetic declination angle (in degrees)
 */
float lookup_magdev(float lat, float lon, int year);


/** 
 * @brief Calculate the magnetic compass declination angle at a given
 * latitude/longitude for compass correction using WMM20xx
 * 
 *  @param lat the latitude for the location to lookup (decimal degrees)
 *  @param lon the longitude for the location to lookup (decimal degrees)
 *  @param year the year for which the declination angle is needed
 *  @return The magnetic declination angle (in degrees)
 */
float calc_magdev(float lat, float lon, int year);


#endif /* !__MAGDEC__H */
