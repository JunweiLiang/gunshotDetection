# ========================================================================
#  JANUS-SR   Janus Speech Recognition Toolkit
#             ------------------------------------------------------------
#             Object: Wavelet Definitions and some helper routines
#             ------------------------------------------------------------
#
#  Author  :  Michael Wand
#  Module  :  wavelet.tcl
#  Date    :  $Id: wavelets.tcl 2747 2007-02-15 10:33:15Z stueker $
#
#  Remarks :
#
# ========================================================================
#
# $Log$
# Revision 1.1  2007/02/15 10:33:15  stueker
# Predfined wavelet filters and helper functions from Michael Wand
#
#
# ========================================================================


# This procedure takes a low-pass filter H and turns it into a highpass filter G
# by the law G(z) = z^{-1} H(-z^{-1}). Note that H and G contain names of filter objects:
# $H must exist, $G must NOT exist.
proc getHighpassFilter { G H } {
        set oldOffset [ lindex [$H] 0 ];
        set oldList [ lindex [$H] 1 ];
        set oldListLast [ expr [ llength $oldList ] - 1 + $oldOffset ];
        set newOffset [ expr 1 - $oldListLast ];

        set curEl $oldListLast;
        set newList {}
        if { [ expr $newOffset % 2 ] == 0 } {
                set curFactor 1;
        } else {
                set curFactor -1;
        }

        while { $curEl >= $oldOffset } {
		set newElement [ expr $curFactor * [ lindex $oldList [ expr $curEl - $oldOffset ] ] ];
                lappend newList $newElement;
                incr curEl -1;
		set curFactor [ expr $curFactor * -1 ];
        }

        Filter $G [ list $newOffset $newList ];
}

# This procedure upsamples a filter by the factor 2. Note that H and G contain names of filter objects:
# $H must exist, $G must NOT exist.
proc upsampleFilter { G H } {
	set oldList [ lindex [$H] 1 ];
	set oldOffset [ lindex [$H] 0 ];
	set newOffset [ expr $oldOffset * 2 ];
	set newList { };
	for { set curIndex 0 } { $curIndex < [ llength $oldList ] } { incr curIndex } {
		lappend newList [ lindex $oldList $curIndex ];
		lappend newList 0;
	}

        Filter $G [ list $newOffset $newList ];
}

# --------------------------------------------------------------------
# Filters for the DTCWT by N. Kingsbury
# flt6*: 6-tap q-shift filter
# flt14*: 14-tap q-shift filter 
# --------------------------------------------------------------------

proc initWaveletFilters { } {
    Filter flt6-0a { 0 {  2.3038e-01  7.1485e-01  6.3088e-01 -2.7984e-02 -1.8704e-01  3.0841e-02  3.2883e-02 -1.0597e-02 } }
    Filter flt6-0b { 1 {  2.3038e-01  7.1485e-01  6.3088e-01 -2.7984e-02 -1.8704e-01  3.0841e-02  3.2883e-02 -1.0597e-02 } }
    Filter flt6-1a { -1 { -.11430184 0 .58751830 .76027237 .23389032 -.08832942 0 .03516384 } }
    Filter flt6-1b { -5 {  .03516384 0 -.08832942 .23389032 .76027237 .58751830 0 -.11430184 } }
    
    Filter flt14-0a flt6-0a
    Filter flt14-0b flt6-0b
    Filter flt14-1a { -5 { -.0045569 -.00543948 .01702522 .2382538 -.10671180 .01186609 .56881042 .75614564 .27529538 -.11720389 -.0388728 .03466035 -.00388321 .00325314 } }
    Filter flt14-1b { -7 { .00325314 -.00388321 .03466035 -.0388728 -.11720389 .27529538 .75614564 .56881042 .01186609 -.10671180 .2382538 .01702522 -.00543948 -.0045569 } }
    
    Filter Daubechies-1 { 0 { 0.70710678118 0.70710678118 } }
    
    Filter Daubechies-2 { 0 { -0.129409522551 0.224143868042 0.836516303738 0.482962913145 } }
    
    Filter Daubechies-3 { 0 { 0.332671 0.806892 0.459878 -0.135011 -0.085441 0.035226 } }
    
    Filter Daubechies-4 { 0 { 0.230378 0.714847 0.630881 -0.027984 -0.187035 0.030841 0.032883 -0.010597 } }

    Filter Daubechies-5 { 0 { 0.160102 0.603829 0.724309 0.138428 -0.242295 -0.032245 0.077571 -0.006241 -0.012581 0.003336 } }

    Filter Coiflet-1 { -2 { -0.0727326195127 0.337897662457 0.85257202021 0.384864846863 -0.0727329651125 -0.0156557281354 } }

    Filter Coiflet-2 { -4 { 0.0163879067607 -0.0414647416487 -0.0673731341113 0.386110001012 0.81272307845 0.417004910495 -0.0764891547343 -0.0594337391722 0.0236795918883 0.00561159941548 -0.00182292128189 -0.000719834703246 } }

    Filter Coiflet-3 { -6 { -0.00379351474397 0.00778260108149 0.0234526954644 -0.0657719049474 -0.0611233849679 0.405176909092 0.79377722705 0.428483473869 -0.0717998205514 -0.0823019260291 0.0345550214622 0.0158805435031 -0.00900797470687 -0.00257451922175 0.00111752569912 0.000466218128252 -7.09793786954e-05 -3.46058058712e-05 } }

    Filter Coiflet-4 { -8 { 0.000892312189313 -0.00162948515083 -0.007346160634 0.0160689450339 0.0266823066925 -0.0812666934171 -0.0560773154139 0.415308405764 0.782238927296 0.434386061867 -0.0666274758684 -0.0962204462043 0.0393344269069 0.025082265468 -0.0152117336252 -0.00565828260518 0.00375144189049 0.00126655552432 -0.000589019948727 -0.00025997487917 6.23385338293e-05 3.12258354571e-05 -3.25269119345e-06 -1.78190908859e-06 } }

    Filter Coiflet-5 { -10 { -0.000212080839825 0.000358589687931 0.00217823635832 -0.00415935878179 -0.0101311175208 0.0234081567881 0.0281680289737 -0.0919200105687 -0.0520431631813 0.421566206732 0.774289603729 0.437991626215 -0.062035963969 -0.105574208714 0.0412892087542 0.0326835742703 -0.0197617789445 -0.00916423116338 0.00676418544871 0.0024333732129 -0.00166286370218 -0.000638131343108 0.000302259581843 0.000140541149716 -4.13404322766e-05 -2.1315026812e-05 3.73465517551e-06 2.06376185157e-06 -1.67442885785e-07 -9.51765727475e-08 } }
}