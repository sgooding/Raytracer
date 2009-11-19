//------------------------------------------------
// An ANN based photon map implementation
// Author: Sean Gooding
// Date: Tue Apr 28 18:33:51 MDT 2009
// Description: Uses the ANN library for "hopefully" a better storage of the photons
//------------------------------------------------

#ifndef __SMG_PhotonMapAnn_H_
#define __SMG_PhotonMapAnn_H_

#include "PhotonMap.h"
#include <vector>
#include <ANN/ANN.h>

namespace smg {

class PhotonMapAnn : public Photon_map
{
    public:
        double mSumFound;
        double mAvgCount;
        double mMax;
        double mMin;
        Vector mAvgDir;
        PhotonMapAnn( const int max_photons );
        virtual ~PhotonMapAnn();

        void store(
                const float power[3],         // photon power
                const float pos[3],           // photon position
                const float dir[3] );         // photon direction

        void irradiance_estimate( float irrad[3],               // returned irradiance
                                  const float pos[3],           // surface position
                                  const float normal[3],        // surface normal at pos
                                  const float max_dist,         // max distance to look for photons
                                  const int nphotons ) ;   // number of photons to use


        void compute_color( Vector& color,
                const primitive& Primitive,
                const Vector& IncomingDirection,
                const Vector& Position,
                const Vector& Normal,
                const float   MaxSearchDistance,
                const int     NumPhotons );

        void balance() { return; }

        int get_stored_photons() { return mStoredPhotons; }

        void scale_photon_power( const float scale );

    private:

        PhotonMapAnn( const PhotonMapAnn& other ) :
            mNumSearchPts(other.mNumSearchPts),
            mAnnDim(other.mAnnDim),
            mStoredPhotons(other.mStoredPhotons), 
            mMaxPhotons( other.mMaxPhotons ),
            mPhotons   ( other.mPhotons    ),
            mDataPts   ( other.mDataPts    ),
            mQueryPt   ( other.mQueryPt    ),
            mNNIdx     ( other.mNNIdx      ),
            mNNDistance( other.mNNDistance ),
            mAnnTree   ( other.mAnnTree    )
    {}



        PhotonMapAnn operator=(const PhotonMapAnn& other)
        {
            mNumSearchPts=other.mNumSearchPts;
            mAnnDim=other.mAnnDim;
            mStoredPhotons=other.mStoredPhotons;
        }


        void ResizeQuery( const int NumPhotons );

        int             mMaxPhotons;
        std::vector<Photon> mPhotons;

        int             mNumSearchPts;

        int             mAnnDim;        // K-D Dimensions
        ANNpointArray   mDataPts;       // Data Points
        ANNpoint        mQueryPt;      // Query Point
        ANNidxArray     mNNIdx;         // Near Neighbor Indices
        ANNdistArray    mNNDistance;    // Near Neighbor Distances

        ANNkd_tree*     mAnnTree;

        int             mStoredPhotons;

        void Close();
};

}

#endif
