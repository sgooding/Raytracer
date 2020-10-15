
#include "PhotonMapAnn.h"
#include "ANN/ANN.h"
#include <assert.h>

namespace smg{

    PhotonMapAnn::PhotonMapAnn( const int max_photons ):
       mSumFound(0.0),
       mAvgCount(0.0),
       mMax(-99999.9),
       mMin(9999.9),
       mAvgDir(0.0,0.0,0.0),
       mMaxPhotons(max_photons),
       mPhotons(max_photons),
       mDataPts(NULL),
       mQueryPt(NULL),
       mNNIdx(NULL), 
       mNNDistance(NULL),
       mAnnTree(NULL),
       mNumSearchPts(0),
       mAnnDim(0),
       mStoredPhotons(0)
    { 
        if( max_photons <= 0 )
        {
            std::cerr << "Must have more than 0 max photons : " << max_photons << std::endl;
            assert(0);
        }
        mStoredPhotons = 0;

        mAnnDim  = 3;
        std::cerr << "Begin Tree Enabled" << std::endl;

        mQueryPt = annAllocPt( mAnnDim );

        mDataPts = annAllocPts( mMaxPhotons, mAnnDim );

        ANNkd_tree* my_tree = new ANNkd_tree( mDataPts,   // The point array
                                              mMaxPhotons,    // number of points
                                              mAnnDim );  // dimensions

        std::cerr << "Tree Enabled" << std::endl;
        std::cerr << "Tree Test" << std::endl;
 
    }

     void PhotonMapAnn::scale_photon_power( const float scale )
     {
         std::cout << "Scale: " << scale << std::endl;
         for( std::vector<Photon>::iterator ii = mPhotons.begin();
                 ii != mPhotons.end();
                 ii++ )
         {
             ii->power[0] *= scale;
             ii->power[1] *= scale;
             ii->power[2] *= scale;
         }
     }

    void PhotonMapAnn:: store(
            const float power[3],   // photon power
            const float pos[3],     // photon position
            const float dir[3] )    // photon direction
    {
        if( mStoredPhotons >= mMaxPhotons )
            return;

        Photon *const node = &mPhotons[mStoredPhotons];

        for (int i=0; i<3; i++) {
            node->pos[i] = pos[i];
            node->power[i] = power[i];
            node->ddir[i] = dir[i];
        }

        
        for( int i = 0; i < 3; i++ )
        {
            mDataPts[mStoredPhotons][i] =mPhotons[mStoredPhotons].pos[i];
        }
        mStoredPhotons++;
    }


    void PhotonMapAnn::compute_color( Vector& color,
                        const primitive& Primitive,
                        const Vector& IncomingDirection,
                        const Vector& Position,
                        const Vector& Normal,
                        const float   MaxSearchDistance, // Max Search Distance
                        const int     NumPhotons )
    {
        Vector IncomingFlipped( IncomingDirection*(-1.0) );
        if( mMaxPhotons < NumPhotons ) 
        {
            std::cerr << "Error: Trying to query with " << NumPhotons 
                      << " but map size is only " 
                      << mMaxPhotons << std::endl;
            assert(0);
        }

        float max_dist_sq = MaxSearchDistance*MaxSearchDistance;

        if( NumPhotons != mNumSearchPts )
        {
            ResizeQuery( NumPhotons );
        }

        // Might need to change this to soften
        float error_bound(0.0);
        mQueryPt[0] = Position.x;
        mQueryPt[1] = Position.y;
        mQueryPt[2] = Position.z;

        ANNdist sqRad( max_dist_sq );

        int num_photons_found(0);

        // Find the point around the query
        //mAnnTree->annkSearch(
        // Radius Search
        mAnnTree->annkFRSearch(
                mQueryPt,       // query point
                sqRad,          // squared radius
                mNumSearchPts,     // number of near neighbors to return
                mNNIdx,         // nearest neighbor array (modified)
                mNNDistance,    // dist to near neighbors (modified)
                error_bound);   // error bound

        for( int i = 0; i < mNumSearchPts; i++ )
        {
            if( mNNIdx[i] != ANN_NULL_IDX ) 
            {
                num_photons_found++;
            } else
            {
                break;
            }
        }
        //std::cout << "Stored Photons = " << mStoredPhotons << std::endl;
        //std::cout << "Photons Found = " << num_photons_found << std::endl;


        if( num_photons_found < 8 )
        {
            color = Vector(0.0, 0.0, 0.0);
            //std::cout << "Less than 8 found returning 0" << std::endl;
            return;
        }

        float pdir[3];
        Vector color_sum;
        float Idr(0.0), Idb(0.0), Idg(0.0), Is(0.0);

        //Vector sum;
        // sum irradiance for all photons
        //float max_distance = -999.9;
        float cone_filter_k = 2.0;
        for( int i = 0; i < num_photons_found; i++ )
        {
            const Photon& photon = (mPhotons[ mNNIdx[i] ]);
            //photon_dir(pdir, &photon);

            if( ( photon.ddir[0]*Normal.x+
                  photon.ddir[1]*Normal.y+
                  photon.ddir[2]*Normal.z ) < 0.0f )
            {
                //std::cout << "Distances: " << mNNDistance[i] << std::endl;


            Vector brdf_r( Primitive.compute_brdf_r( Position,
                                                     Vector( photon.ddir[0], photon.ddir[1], photon.ddir[2] ),
                                                     IncomingDirection ) );
            // Cone Filter
            float weight_photon = 1.0 - sqrt(mNNDistance[i])/cone_filter_k;
            /*
            std::cout << "Distance : " << sqrt(mNNDistance[i])
                      << "\nPosition: " << photon.pos[0] << ", " << photon.pos[1] << ", " << photon.pos[2]
                      << "\nQuery   : " << Position << std::endl;
                      */

            color_sum = color_sum + brdf_r.times( Vector( photon.power[0],
                                                          photon.power[1], 
                                                          //photon.power[2] ) )*( weight_photon ); 
                                                          //photon.power[2] ) )*( IncomingFlipped.dot( Normal ) ); 
                                                          //photon.power[2] ) )*( IncomingFlipped.dot( Normal ) )*(weight_photon); 
                                                          photon.power[2] ) );//*( IncomingFlipped.dot( Normal ) )*(weight_photon); 

            } else
            {
                //std::cout << "Other Distances: " << mNNDistance[i] << std::endl;
                Vector brdf_r( Primitive.compute_brdf_r( Position,
                            Vector( -1.0*photon.ddir[0], -1.0*photon.ddir[1], -1.0*photon.ddir[2] ),
                            IncomingDirection ) );
                // Cone Filter
                float weight_photon = 1.0 - sqrt(mNNDistance[i])/cone_filter_k;
                /*
                   std::cout << "Distance : " << sqrt(mNNDistance[i])
                   << "\nPosition: " << photon.pos[0] << ", " << photon.pos[1] << ", " << photon.pos[2]
                   << "\nQuery   : " << Position << std::endl;
                 */

                color_sum = color_sum + brdf_r.times( Vector( photon.power[0],
                            photon.power[1], 
                            //photon.power[2] ) )*( weight_photon ); 
                            //photon.power[2] ) )*( IncomingDirection.dot( Normal ) ); 
                            //photon.power[2] ) )*( IncomingFlipped.dot( Normal ) )*(weight_photon); 
                             photon.power[2] ) );//*( IncomingFlipped.dot( Normal ) )*(weight_photon); 


            }
            /*
            std::cout << "Max Distance = " << mNNDistance[0] << std::endl
                      << "InputMax     = " << sqRad << std::endl
                      << "Curr Distance =" << mNNDistance[i] << std::endl;
                      */
        }
        
        const float tmp=(1.0f/M_PI)/(mNNDistance[3]);    // estimate of density
        // Cone Fileter
        //const float tmp=1.0/( 1.0 - 2.0*(1.0/(3.0*cone_filter_k))*(M_PI*mNNDistance[0]));
        //const float tmp=(1.0f/M_PI)/(sqRad*num_photons_found);    // estimate of density
        //std::cout << "max_distance = " << max_distance << std::endl; 
        //color = ( color_sum*tmp*mStoredPhotons);
        color = ( color_sum*tmp);
    }

    void PhotonMapAnn::ResizeQuery( const int num_photons )
    {
        mNumSearchPts = num_photons;

        if( mNNIdx != NULL )
            delete[] mNNIdx;
        if( mNNDistance != NULL )
            delete[] mNNDistance;
        if( mAnnTree != NULL )
            delete mAnnTree;

       
        mNNIdx      = new ANNidx[mNumSearchPts];
        mNNDistance = new ANNdist[mNumSearchPts];
        mAnnTree    = new ANNkd_tree( mDataPts,
                                      mNumSearchPts,
                                      mAnnDim );
    }

    void
    PhotonMapAnn::Close()
    {
        delete[] mNNIdx;
        delete[] mNNDistance;
        delete mAnnTree;
        annClose();
        /*
        std::cout << "\n PhotonMapAnn Avg Found: " 
                  << mSumFound/mAvgCount 
                  << "\n Count: "
                  << mAvgCount 
                  << "\n Max: "
                  << mMax
                  << "\n Min: "
                  << mMin
                  << "\n Dir: " 
                  << mAvgDir/mAvgCount
                  << std::endl;
                  */
    }


    PhotonMapAnn::~PhotonMapAnn()
    {
        Close();
    }

    void PhotonMapAnn::irradiance_estimate( float irrad[3],            
                                  const float pos[3],        
                                  const float normal[3],     
                                  const float max_dist,      
                                  const int NumPhotons ) 
    {
        if( mMaxPhotons < NumPhotons ) 
        {
            std::cerr << "Error: Trying to query with " << NumPhotons 
                << " but map size is only " 
                << mMaxPhotons << std::endl;
            assert(0);
        }

        ANNdist sqRad( max_dist*max_dist);

        if( NumPhotons != mNumSearchPts )
        {
            ResizeQuery( NumPhotons );
        }

        // Might need to change this to soften
        float error_bound(0.0);
        mQueryPt[0] = pos[0];
        mQueryPt[1] = pos[1];
        mQueryPt[2] = pos[2];


        int num_photons_found(0);

        // Find the point around the query
        //mAnnTree->annkSearch(
        // Radius Search
        mAnnTree->annkFRSearch(
                mQueryPt,       // query point
                sqRad,          // squared radius
                mNumSearchPts,     // number of near neighbors to return
                mNNIdx,         // nearest neighbor array (modified)
                mNNDistance,    // dist to near neighbors (modified)
                error_bound);   // error bound

        for( int i = 0; i < mNumSearchPts; i++ )
        {
            if( mNNIdx[i] != ANN_NULL_IDX ) 
            {
                num_photons_found++;
            } else
            {
                break;
            }
        }


        if( num_photons_found < 8 )
        {
            //std::cout << "Less than 8 found returning 0" << std::endl;
            return;
        }

        float sum_dist(9999.9);
        float avg_count(0.0);
        // sum irradiance from all photons
        for (int i=0; i<num_photons_found; i++) 
        {
            const Photon *p = &mPhotons[mNNIdx[i]];


            sum_dist += sqrt( mNNDistance[i] );
            avg_count++;


            if( ( p->ddir[0]*normal[0]+
                        p->ddir[1]*normal[1]+
                        p->ddir[2]*normal[2] ) < 0.0f )
            { 
                irrad[0] += p->power[0];
                irrad[1] += p->power[1];
                irrad[2] += p->power[2];
            }
        }

        //const float tmp=(1.0f/M_PI)/(sqRad);    // estimate of density
        float distance( sum_dist/avg_count );
        //const float tmp=(1.0f/M_PI)/( (distance*distance) );    // estimate of density
        const float tmp=(1.0f/M_PI)/( (mNNDistance[0]) );    // estimate of density
        /*
        std::cout << "Ann Scale : " << tmp << std::endl;
        std::cout << "Ann sqRad: " << sqRad << std::endl;
        abort();
        */

        irrad[0] *= tmp;
        irrad[1] *= tmp;
        irrad[2] *= tmp;

        return;
    }
}
