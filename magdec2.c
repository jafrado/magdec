/*
 * magdec2.c - Simple WMM-2025 magnetic declination calculator
 *
 * Computes magnetic declination using spherical harmonic expansion
 * of the World Magnetic Model 2025 (epoch 2025.0, valid 2025-2030).
 *
 * Coefficients linked from wmmcof2025.c.
 *
 * Usage: magdec2 <lat> <lon> [decimal_year]
 *   lat, lon in decimal degrees
 *   decimal_year defaults to current date
 *
 * Algorithm reference:
 * 
 *   WMM Technical Report, NOAA/NCEI
 *   Spherical harmonic expansion with Schmidt quasi-normalized
 *   associated Legendre functions, degree/order 12.
 *
 * Author: James F Dougherty <jfd@cs.stanford.edu>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


#define NMAX     12
#define NCOEFF   91        /* (NMAX+1)*(NMAX+2)/2 */

/* WGS-84 ellipsoid */
#define WGS84_A     6378.137          /* semi-major axis  (km) */
#define WGS84_B     6356.7523142      /* semi-minor axis  (km) */
#define WGS84_ESQ   (1.0 - (WGS84_B * WGS84_B) / (WGS84_A * WGS84_A))

/* Earth's magnetic reference sphere radius */
#define EARTH_R     6371.2            /* km */

/* WMM-2025 epoch */
#define WMM_EPOCH   2025.0

#define DEG2RAD(d)  ((d) * M_PI / 180.0)
#define RAD2DEG(r)  ((r) * 180.0 / M_PI)

/* Triangular index: n*(n+1)/2 + m */
#define IDX(n, m)   ((n) * ((n) + 1) / 2 + (m))

extern const double wmm_gnm[NCOEFF];
extern const double wmm_hnm[NCOEFF];
extern const double wmm_gtnm[NCOEFF];
extern const double wmm_htnm[NCOEFF];

#ifndef ARDUINO

/**
 * @brief current_decimal year  -  get current year from system 
 *
 * @return decimal year
 */
static double current_decimal_year(void)
{
    time_t now = time(NULL);
    struct tm *t = gmtime(&now);
    int year  = t->tm_year + 1900;
    int yday  = t->tm_yday;          /* 0-based day of year */
    int leap  = (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
    return year + (double)yday / (leap ? 366.0 : 365.0);
}
#endif

/**
 * @brief wmm_declination  -  Calculate magnetic declination at a surface point
 *
 * @param  lat_deg    geodetic latitude  (decimal degrees, +N / -S)
 * @param  lon_deg    geodetic longitude (decimal degrees, +E / -W)
 * @param  dyear      decimal year  (e.g. 2025.5)
 *
 * @return declination in degrees (+E / -W)
 */
double wmm_declination(double lat_deg, double lon_deg, double dyear)
{
    int n, m, i;
    double dt = dyear - WMM_EPOCH;

    /* 1. Time-adjusted Gauss coefficients */
    double g[NCOEFF], h[NCOEFF];
    for (i = 0; i < NCOEFF; i++) {
        g[i] = wmm_gnm[i] + wmm_gtnm[i] * dt;
        h[i] = wmm_hnm[i] + wmm_htnm[i] * dt;
    }

    /* 2. Clamp latitude to avoid polar singularity in By */
    if (lat_deg >  89.9999) lat_deg =  89.9999;
    if (lat_deg < -89.9999) lat_deg = -89.9999;

    double lat_rad = DEG2RAD(lat_deg);
    double lon_rad = DEG2RAD(lon_deg);
    double sin_lat = sin(lat_rad);
    double cos_lat = cos(lat_rad);

    /* 3. Geodetic to geocentric spherical (height = 0, sea level) */
    double rc = WGS84_A / sqrt(1.0 - WGS84_ESQ * sin_lat * sin_lat);
    double xp = rc * cos_lat;
    double zp = rc * (1.0 - WGS84_ESQ) * sin_lat;
    double r  = sqrt(xp * xp + zp * zp);

    double gc_lat_rad = asin(zp / r);          /* geocentric latitude (rad) */
    double gc_lat_deg = RAD2DEG(gc_lat_rad);

    double sin_gc = sin(gc_lat_rad);            /* = cos(colatitude) */
    double cos_gc = cos(gc_lat_rad);            /* = sin(colatitude) */

    /* 4. Relative radius powers: rr[n] = (EARTH_R / r)^(n+2) */
    double rr[NMAX + 1];
    double ratio = EARTH_R / r;
    rr[0] = ratio * ratio;
    for (n = 1; n <= NMAX; n++)
        rr[n] = rr[n - 1] * ratio;

    /* 5. Longitude trig: cos(m*lon), sin(m*lon) via angle-addition */
    double cos_ml[NMAX + 1], sin_ml[NMAX + 1];
    cos_ml[0] = 1.0;  sin_ml[0] = 0.0;
    cos_ml[1] = cos(lon_rad);
    sin_ml[1] = sin(lon_rad);
    for (m = 2; m <= NMAX; m++) {
        cos_ml[m] = cos_ml[m - 1] * cos_ml[1] - sin_ml[m - 1] * sin_ml[1];
        sin_ml[m] = sin_ml[m - 1] * cos_ml[1] + cos_ml[m - 1] * sin_ml[1];
    }

    /* 6. Gauss-normalized associated Legendre functions & colatitude derivatives
     *    Argument: x = sin(geocentric lat) = cos(colatitude)
     *              z = cos(geocentric lat) = sin(colatitude)
     *    dPcup stores dP/d(colatitude) in Gauss normalisation.
     */
    double x = sin_gc;
    double z = cos_gc;

    double Pcup[NCOEFF], dPcup[NCOEFF];
    memset(Pcup,  0, sizeof(Pcup));
    memset(dPcup, 0, sizeof(dPcup));

    Pcup[0]  = 1.0;
    dPcup[0] = 0.0;

    for (n = 1; n <= NMAX; n++) {
        for (m = 0; m <= n; m++) {
            int idx = IDX(n, m);
            if (n == m) {
                /* Sectoral (diagonal) recursion */
                int i1 = IDX(n - 1, m - 1);
                Pcup[idx]  = z * Pcup[i1];
                dPcup[idx] = z * dPcup[i1] + x * Pcup[i1];
            } else if (n == 1 && m == 0) {
                /* P(1,0) = x */
                int i1 = IDX(0, 0);
                Pcup[idx]  = x * Pcup[i1];
                dPcup[idx] = x * dPcup[i1] - z * Pcup[i1];
            } else {
                /* n > 1, m < n */
                int i2 = IDX(n - 1, m);
                if (m > n - 2) {
                    /* Sub-diagonal (m = n-1): P(n-2,m) does not exist */
                    Pcup[idx]  = x * Pcup[i2];
                    dPcup[idx] = x * dPcup[i2] - z * Pcup[i2];
                } else {
                    int i1 = IDX(n - 2, m);
                    double k = (double)((n - 1) * (n - 1) - m * m) /
                               (double)((2 * n - 1) * (2 * n - 3));
                    Pcup[idx]  = x * Pcup[i2] - k * Pcup[i1];
                    dPcup[idx] = x * dPcup[i2] - z * Pcup[i2] - k * dPcup[i1];
                }
            }
        }
    }

    /* 7. Schmidt quasi-normalisation ratios and application.
     *    Also flip derivative sign: d/d(colatitude) -> d/d(latitude).
     */
    double sqn[NCOEFF];
    sqn[0] = 1.0;
    for (n = 1; n <= NMAX; n++) {
        /* m = 0 */
        sqn[IDX(n, 0)] = sqn[IDX(n - 1, 0)] * (double)(2 * n - 1) / (double)n;
        /* m = 1..n */
        for (m = 1; m <= n; m++)
            sqn[IDX(n, m)] = sqn[IDX(n, m - 1)]
                * sqrt((double)((n - m + 1) * (m == 1 ? 2 : 1))
                       / (double)(n + m));
    }
    for (n = 1; n <= NMAX; n++) {
        for (m = 0; m <= n; m++) {
            int idx = IDX(n, m);
            Pcup[idx]  *=  sqn[idx];
            dPcup[idx]  = -dPcup[idx] * sqn[idx];   /* sign flip */
        }
    }

    /* 8. Spherical harmonic summation (spherical coordinate frame)
     *    Bx = north,  By = east,  Bz = down
     *    (Equations 10-12, WMM Technical Report)
     */
    double Bx = 0.0, By = 0.0, Bz = 0.0;
    for (n = 1; n <= NMAX; n++) {
        for (m = 0; m <= n; m++) {
            int idx = IDX(n, m);
            double gcos_hsin = g[idx] * cos_ml[m] + h[idx] * sin_ml[m];
            double gsin_hcos = g[idx] * sin_ml[m] - h[idx] * cos_ml[m];

            Bz -= rr[n] * (double)(n + 1) * gcos_hsin * Pcup[idx];
            By += rr[n] * (double)m       * gsin_hcos * Pcup[idx];
            Bx -= rr[n]                   * gcos_hsin * dPcup[idx];
        }
    }

    /* Divide By by cos(geocentric latitude) */
    if (fabs(cos_gc) > 1.0e-10)
        By /= cos_gc;

    /* 9. Rotate from geocentric spherical to geodetic (Eq. 16) */
    double psi = DEG2RAD(gc_lat_deg - lat_deg);
    double Bx_geo = Bx * cos(psi) - Bz * sin(psi);
    double By_geo = By;

    /* 10. Magnetic declination */
    return RAD2DEG(atan2(By_geo, Bx_geo));
}

#ifndef ARDUINO

int main(int argc, char *argv[])
{
    if (argc < 3) {
        fprintf(stderr,
            "magdec2 - WMM-2025 magnetic declination calculator\n"
            "Usage: %s <lat> <lon> [decimal_year]\n"
            "  lat, lon  : decimal degrees (+N/-S, +E/-W)\n"
            "  year      : decimal year (default: current date)\n"
            "  Epoch 2025.0, valid 2025.0 - 2030.0\n",
            argv[0]);
        return 1;
    }

    double lat   = atof(argv[1]);
    double lon   = atof(argv[2]);
    double dyear = (argc >= 4) ? atof(argv[3]) : current_decimal_year();

    double decl = wmm_declination(lat, lon, dyear);
    char   dir  = decl >= 0.0 ? 'E' : 'W';

    printf("%+.2f degrees (%c)  at (%+.4f, %+.4f) in %.2f\n",
           decl, dir, lat, lon, dyear);

    return 0;
}
#endif
