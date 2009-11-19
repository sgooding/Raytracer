#ifndef _SMG_PhotonMapHeinrik_H_
#define _SMG_PhotonMapHeinrik_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "Vector.h"
#include "Primitives.h"
#include "PhotonMap.h"

namespace smg {

/* This is the PhotonMapHeinrik class
 */
//*****************
class PhotonMapHeinrik : 
    public Photon_map 
    {
//*****************
public:

    double mSumFound;
    double mAvgCount;
    double mMax;
    double mMin;
    Vector mAvgDir;

  PhotonMapHeinrik( const int max_phot );

  virtual ~PhotonMapHeinrik();


  void store(
    const float power[3],         // photon power
    const float pos[3],           // photon position
    const float dir[3] );         // photon direction

  void scale_photon_power(
    const float scale );          // 1/(number of emitted photons)

  void balance(void);             // balance the kd-tree (before use!)

  void compute_color( Vector& color,
                        const primitive& Primitive,
                        const Vector& incoming_direction,
                        const Vector& surface_position,
                        const Vector& surface_normal,
                        const float max_dist,
                        const int nphotons );

  void irradiance_estimate(
    float irrad[3],               // returned irradiance
    const float pos[3],           // surface position
    const float normal[3],        // surface normal at pos
    const float max_dist,         // max distance to look for photons
    const int nphotons ) ;   // number of photons to use

  void locate_photons(
    NearestPhotons *const np,     // np is used to locate the photons
    const int index ) const;      // call with index = 1

  /*
  void photon_dir(
    float *dir,                    // direction of photon (returned)
    const Photon *p ) const;       // the photon
    */

  int get_stored_photons(){ return stored_photons; }

private:

  PhotonMapHeinrik(const smg::PhotonMapHeinrik& other):
      photons(other.photons),
      stored_photons(other.stored_photons),
      half_stored_photons(other.half_stored_photons),
      max_photons(other.max_photons),
      prev_scale(other.prev_scale)
        {}
  PhotonMapHeinrik operator=(const PhotonMapHeinrik&){}

  void balance_segment(
    Photon **pbal,
    Photon **porg,
    const int index,

    const int start,
    const int end );

  void median_split(
    Photon **p,
    const int start,
    const int end,
    const int median,
    const int axis );

  Photon *photons;

  int stored_photons;
  int half_stored_photons;
  int max_photons;
  int prev_scale;


  float bbox_min[3];     // use bbox_min;
  float bbox_max[3];     // use bbox_max;
};

} // end smg

#endif
