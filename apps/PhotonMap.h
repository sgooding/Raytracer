#ifndef _SMG_Photon_map_H_
#define _SMG_Photon_map_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "Vector.h"
#include "Primitives.h"

namespace smg {

    /*  This is the photon
     *  The power is not compressed so the
     *  size is 28 bytes
     */
    //**********************
    typedef struct Photon {
        //**********************
        float pos[3];                  // photon position
        float dpos[3];                  // photon position
        short plane;                   // splitting plane for kd-tree
        unsigned char theta, phi;      // incoming direction
        float ddir[3];                // incoming direction
        float power[3];                // photon power (uncompressed)
    } Photon;


    /* This structure is used only to locate the
     * nearest photons
     */
    //*******************************
    typedef struct NearestPhotons {
        //*******************************
        int max;
        int found;
        int got_heap;
        float pos[3];
        float *dist2;
        const Photon **index;
    } NearestPhotons;

    class Photon_map
    {
        public:
            Photon_map( )
            {
                //----------------------------------------
                // initialize direction conversion tables
                //----------------------------------------

                for (int i=0; i<256; i++) {
                    float angle = float(i)*(1.0/256.0)*M_PI;
                    costheta[i] = cos( angle );
                    sintheta[i] = sin( angle );
                    cosphi[i]   = cos( 2.0*angle );
                    sinphi[i]   = sin( 2.0*angle );
                }
            }

            Photon_map( int max_photon );

            virtual ~Photon_map(){}
  
            virtual void balance(void) = 0;             // balance the kd-tree (before use!)

            virtual void store(
                    const float power[3],         // photon power
                    const float pos[3],           // photon position
                    const float dir[3] ) = 0;         // photon direction

            virtual int get_stored_photons() = 0;

            virtual void scale_photon_power(const float scale ) = 0; // 1/(number of emitted photons)

            virtual void irradiance_estimate( float irrad[3],               // returned irradiance
                                              const float pos[3],           // surface position
                                              const float normal[3],        // surface normal at pos
                                              const float max_dist,         // max distance to look for photons
                                              const int nphotons ) = 0;   // number of photons to use

            virtual void compute_color( Vector& color,
                                        const primitive& Primitive,
                                        const Vector& incoming_direction,
                                        const Vector& surface_position,
                                        const Vector& surface_normal,
                                        const float max_dist,
                                        const int nphotons ) = 0;



            float costheta[266];
            float sintheta[266];
            float cosphi[256];
            float sinphi[256];


            /* photon.dir returns the direction of a photon
             */
            //*****************************************************************
            void photon_dir( float *dir, const Photon *p ) const
                //******************************************************************
            {
                //dir[0] = sin(p->theta)*cos(p->phi);
                dir[0] = sintheta[p->theta]*cosphi[p->phi];
                //dir[1] = sin(p->theta)*sin(p->phi);
                dir[1] = sintheta[p->theta]*sinphi[p->phi];
                //dir[2] = cos(p->theta);
                dir[2] = costheta[p->theta];
            }
    };

}
#endif
