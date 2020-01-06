/***
 * Print out declination over the globe.
 * 
 * Based on WMM 2020 GeomagnetismLibrary
 * 
 *
 * The WMM source code is in the public domain and not licensed or
 * under copyright.  The information and software may be used
 * freely by the public. As required by 17 U.S.C. 403, third
 * parties producing copyrighted works consisting predominantly
 * of the material produced by U.S. government agencies must
 * provide notice with such work(s) identifying the
 * U.S. Government material incorporated and stating that such
 * material is not subject to copyright protection.
 * 
 * 
 * Copyright (C) 2020 Real Flight Systems, Inc.
 * Copyright (C) 2020 James Dougherty <jfd@realflightsystems.com>
 */

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>


#include "GeomagnetismHeader.h"
#include "EGM9615.h"

int main(int argc, char* argv[])
{
	MAGtype_MagneticModel * MagneticModels[1], *TimedMagneticModel;
	MAGtype_Ellipsoid Ellip;
	MAGtype_CoordSpherical CoordSpherical;
	MAGtype_CoordGeodetic CoordGeodetic;
	MAGtype_Date UserDate;
	MAGtype_GeoMagneticElements GeoMagneticElements, Errors;
	MAGtype_Geoid Geoid;
	
	char* filename = "WMM.COF";
	
	char VersionDate[12];
	int NumTerms, nMax = 0;
	int epochs = 1;
	int lat, lon, year;
	
	/* Memory allocation */
	
	strncpy(VersionDate, VERSIONDATE_LARGE + 39, 11);
	VersionDate[11] = '\0';
	
	if(!MAG_robustReadMagModels(filename, &MagneticModels, epochs)) {
		printf("error: [WMM.COF] not found.\n ");
		return -1;
	}
	
	if(nMax < MagneticModels[0]->nMax)
		nMax = MagneticModels[0]->nMax;
	
	NumTerms = ((nMax + 1) * (nMax + 2) / 2);
	
	/* For storing the time modified WMM Model parameters */
	TimedMagneticModel = MAG_AllocateModelMemory(NumTerms); 
	if(MagneticModels[0] == NULL || TimedMagneticModel == NULL) {
		MAG_Error(2);
	}
	/* Set default values and constants */    
	MAG_SetDefaults(&Ellip, &Geoid);
	/* Check for Geographic Poles */
	
	/* Set EGM96 Geoid parameters */
	Geoid.GeoidHeightBuffer = GeoidHeightBuffer;
	Geoid.Geoid_Initialized = 1;

	for (year = 2020; year <= 2025; year++) {
	  
		for (lat = 90; lat >= -90; lat--) {
		  
			for (lon = -180; lon <= 180; lon++) {

				/* Input Year */
				UserDate.DecimalYear = year;
				UserDate.Year = year;    
				UserDate.Month = 12;
				UserDate.Day = 1;
			  
				CoordGeodetic.phi = lat; /* lat */
				CoordGeodetic.lambda = lon; /* lon */
		
				/* Ground Level */
				CoordGeodetic.HeightAboveGeoid = CoordGeodetic.HeightAboveEllipsoid = 0;
                                /* Not above geoid */
				Geoid.UseGeoid = 0;

#if 0		
				/* User entered height above MSL, convert it to the height above WGS-84 ellipsoid */
				Geoid.UseGeoid = 1;    
				CoordGeodetic.HeightAboveGeoid = 0.001; /* Units in Km - ie .001 Km = 1 meter */
				MAG_ConvertGeoidToEllipsoidHeight(&CoordGeodetic, &Geoid);
#endif				
		
		
				/*Convert from geodetic to Spherical Equations: 17-18, WMM Technical report*/		
				MAG_GeodeticToSpherical(Ellip, CoordGeodetic, &CoordSpherical);
		
				/* Time adjust the coefficients, Equation 19, WMM Technical report */
				MAG_TimelyModifyMagneticModel(UserDate, MagneticModels[0], TimedMagneticModel);
		
				/* Computes the geoMagnetic field elements and their time change*/
				MAG_Geomag(Ellip, CoordSpherical, CoordGeodetic,
					   TimedMagneticModel, &GeoMagneticElements); 

				MAG_CalculateGridVariation(CoordGeodetic, &GeoMagneticElements);

				MAG_WMMErrorCalc(GeoMagneticElements.H, &Errors);

#if 0		
				/* Print the results */		    
				MAG_PrintUserDataWithUncertainty(GeoMagneticElements,
								 Errors,
								 CoordGeodetic,
								 UserDate, TimedMagneticModel, &Geoid);
#endif				
				printf("%d,%d,%d,%2.2f,%2.2f,%2.2f,%2.2f\n",
				      year, lat, lon, CoordGeodetic.HeightAboveGeoid,
				      GeoMagneticElements.Decl,
				      60 * Errors.Decl,
				      60 * GeoMagneticElements.Decldot);
				      

				
			}
		}
	}
	
	MAG_FreeMagneticModelMemory(TimedMagneticModel);
	MAG_FreeMagneticModelMemory(MagneticModels[0]);
	
	return 0;
}

