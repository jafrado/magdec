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
 * readings.
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
 */




/* 
 * Run the below:
 * 
 * ./parse_mag_dec.pl igrfgridData.csv 
 * 
 * Insert the resulting table output below into this code
 * 
 * @author James Dougherty <jfd@cs.stanford.edu>
 * 
 */


#include <stdlib.h>
#include <stdio.h>
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
float lookup_magdev(float lat, float lon, int year)
{
	int i, n;
	int lati, loni;
	magdec_correction_t* c = NULL;
	
	lati = (int)lat;
	loni = (int)lon;
	
	//printf("lookup: %d,%d\n", lati, loni);

	if (year <= 2019) { 
		c = &corr[0];
		n = n_magdec_entries;
	}
	else { 
		c = &corr_2020[0];
		n = n_magdec_entries_2020;
	}
	
	for (i = 0; i < n; i++){ 
		if ((c[i].lat == lati) && (c[i].lon == loni) && 
		    (c[i].year == year)){
		  // printf("entry %d - %d, %d\n", i, corr[i].lat, corr[i].lon);
			break;
		}
	}
	if (i < n)
		return c[i].magdec;
	else { 
		printf("WARNING: %2.2f, %2.2f not in database for %d,"
		       " correction most likely incorrect\n", lat, lon, year);
		return 0;
	}
}


/* Blackrock: 36.52,-120.07 */
/* Fresno: 40.85,-119.12 */
/* Waikiki: 21.285, -157.83 */

typedef struct location_s{
	char* city;
	char* state;
	char* country;
	float lat;
	float lon;
} location_t;

location_t locations[] = {
	{"Miami Beach", "FLA", "USA", 25.7907, -80.1300},
	{"New York", "NY", "USA", 40.7128, -74.0059},
	{"Marietta", "GA", "USA", 33.9526, -84.5499},
	{"Denver", "CO", "USA", 39.7392, -104.9903},
	{"Fresno", "CA", "USA", 40.85,-119.12},
	{"Blackrock", "NV", "USA", 36.52,-120.07},
	{"Waikiki, Honolulu", "HI", "USA", 21.2850, -157.8357},
	{"Mauna Kea", "HI", "USA", 19.820600, -155.468100},
	{"Dochartaigh Castle", "Donegal", "Ireland", 55.315995, -7.371370},
	{"Mageroya", "Nordkapp", "Norway", 71.129743, 25.874366},
	{"South Pole", "SP", "Antartica", -89, 0},
	{"North Pole", "NP", "Artic Ocean", 89, 0},	
};
#define n_locations ((sizeof(locations)/sizeof(locations[0])))

int years[] = {2019, 2020, 2021, 2022, 2023, 2024};
#define n_years ((sizeof(years)/sizeof(years[0])))

int main(int argc, char* argv[])
{
	int i, year;
	double variance[100];

	printf("Showing Magnetic declination changes for "
	       "%ld locations over %ld years\n\n", n_locations, n_years);
	
	for (int i = 0; i < n_locations; i++) {
		for (int j = 0; j < n_years; j++) { 		
			year = years[j];
			printf("%s,%s, %s (%2.3f,%2.3f) in "
			       "%d: %2.2f degrees\n", 
			       locations[i].city, locations[i].state,
			       locations[i].country,
			       locations[i].lat,
			       locations[i].lon, year,
			       lookup_magdev(locations[i].lat,
					     locations[i].lon, year));

		}
		printf("---\n");
	}

}
