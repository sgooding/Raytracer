#ifndef __RAY_TRACE_H
#define __RAY_TRACE_H
//
// Author: Sean Gooding
// File: RayTrace.h
// Date: Tue Jan 27 20:41:53 MST 2009
// Purpose:
//          A simple ray tracer
//
//


#include "Primitives.h"
#include "PhotonMapHeinrik.h"
#include "PhotonMapAnn.h"

#include <math.h>
#include <vector>
#include <list>

#define VERY_SMALL_NUMBER 0.0000001

namespace smg{

class RayTrace
{
    public:

        Vector trace_ray( const int current_object, ray original_ray );

        /// Default Constructor
        RayTrace();

        /// Default destructor
        ~RayTrace();
    
        void ComputeBRDF( const primitive& Primitive, const Vector& position, const Vector& incoming_direction, const Vector& outgoing_direction, bool in_shade, double& R, double& G, double& B );

        bool InitializeMap(int maxNumPhotons );

        void compute_multipath( const primitive& Primitive, 
                                ray origin, 
                                double distance,  
                                int depth, 
                                Vector location, 
                                bool in_shadow, 
                                double&R, 
                                double&G, 
                                double&B );

        void TraceRay( const primitive& Primitive, 
                       ray origin,
                       double t,
                       Vector p,
                       bool in_shadow,
                       double& R,
                       double& G,
                       double& B,
                       bool test = false
                     );
                  
                  
        double FindIntensity();
        bool LoadLightSource();

        inline void GetLightSource( Vector& ls ) { ls = mLightSource; }

        inline void ViewPoint( Vector vp ) { mViewPoint = vp; }
        inline Vector GetViewPoint( ) { return mViewPoint; }
        inline void LightSource( Vector ls ) { mLightSource = ls; }
        inline void AmbientLight( double al ) { mAmbientLight = al; }
        inline void LightSourceIntensity( double lsi ) { mLightSourceIntensity = lsi; }
        inline void ScreenLowerLeftCorner( Vector in ) { mScreenLowerLeftCorner = in; }
        inline void ScreenHorizontalVector( Vector in ) { mScreenHorizontalVector = in; }
        inline void ScreenVerticalVector( Vector in ) { mScreenVerticalVector = in; }
        inline void ResolutionX( int in ) { mResolutionX = in; }
        inline void ResolutionY( int in ) { mResolutionY = in; }
        inline void AliasSize( double alias ) { mAliasSize = alias; } 
        inline double GetAliasSize() { return mAliasSize; } 
        inline bool MapInitialized() { 
            return ( mPhotonMap == NULL ) && (mCausticMap == NULL ); 
        }
        
        inline void MaxNumberPhotons( int in ) { mMaxNumberPhotons = in; }
        inline int GetMaxNumberPhotons() { return mMaxNumberPhotons; }


        void DirectionVector( double x, double y, ray& eq );
        
    
        void AddPrimitive( smg::primitive* pprimitive );
        
        void EmitPhotons(void);

        Vector TraceThroughScene( ray trace, int& depth);

        void SetEnablePhotonMapper(bool enable_map){ mEnablePhotonMapper = enable_map; }
        bool GetEnablePhotonMapper(){ return mEnablePhotonMapper; }

        void SetBackgroundColor( Vector& color ){ mBackgroundColor = color; }
        Vector GetBackgroundColor( ){ return mBackgroundColor; }

    private:

        RayTrace(const RayTrace& other) :
        mBackgroundColor       (other.mBackgroundColor       ),
        mPhotonMap             (other.mPhotonMap             ),
        mCausticMap             (other.mCausticMap             ),
        mEnablePhotonMapper    (other.mEnablePhotonMapper    ),
        mLightSource           (other.mLightSource           ),
        mLightSourceIntensity  (other.mLightSourceIntensity  ),
        mViewPoint             (other.mViewPoint             ),
        mAmbientLight          (other.mAmbientLight          ),
        mScreenLowerLeftCorner (other.mScreenLowerLeftCorner ),
        mScreenHorizontalVector(other.mScreenHorizontalVector),
        mScreenVerticalVector  (other.mScreenVerticalVector  ),
        mResolutionY           (other.mResolutionY           ),
        mResolutionX           (other.mResolutionX           ),
        mAliasSize             (other.mAliasSize             ),
        mMaxNumberPhotons      (other.mMaxNumberPhotons      ),
        mPrimitives            (other.mPrimitives            )
        {}

        RayTrace operator=(const RayTrace& other)
        {
            mBackgroundColor       =other.mBackgroundColor       ;
            mPhotonMap             =other.mPhotonMap             ;
            mCausticMap             =other.mCausticMap             ;
            mEnablePhotonMapper    =other.mEnablePhotonMapper    ;
            mLightSource           =other.mLightSource           ;
            mLightSourceIntensity  =other.mLightSourceIntensity  ;
            mViewPoint             =other.mViewPoint             ;
            mAmbientLight          =other.mAmbientLight          ;
            mScreenLowerLeftCorner =other.mScreenLowerLeftCorner ;
            mScreenHorizontalVector=other.mScreenHorizontalVector;
            mScreenVerticalVector  =other.mScreenVerticalVector  ;
            mResolutionY           =other.mResolutionY           ;
            mResolutionX           =other.mResolutionX           ;
            mAliasSize             =other.mAliasSize             ;
            mMaxNumberPhotons      =other.mMaxNumberPhotons      ;
            mPrimitives            =other.mPrimitives            ;
        }

        Vector mLightSource;
        double mLightSourceIntensity;
        Vector mViewPoint;
        double mAmbientLight;
        Vector mScreenLowerLeftCorner;
        Vector mScreenHorizontalVector;
        Vector mScreenVerticalVector;
        Vector mBackgroundColor;


        bool mEnablePhotonMapper;

        int mResolutionY;
        int mResolutionX;
        double mAliasSize;

        int mMaxNumberPhotons;

        //void trace_multiple_paths( const int current_object, ray original_ray, const Vector& power, bool& added );
        void trace_multiple_paths( const int current_object, ray original_ray, const Vector& power, bool& added , std::list<int>& path_list);
        

        Photon_map* mPhotonMap;
        Photon_map* mCausticMap;

        std::vector<smg::primitive*> mPrimitives;

    bool TestInShadow( int PrimitiveInx, Vector location);
        bool FindNearestObject( const ray& start, int& primitive_index, double& distance );
 
        Vector ComputeEmittance(  const primitive& object, const ray& trace, const double distance );

        Vector PhotonDirection( double Theta, double Phi );
        
        Vector GetPointColor( const primitive& Primitive, const ray& original_ray, double distance );

        ray GetReflectedRay( const primitive& Primitive, const ray& original_ray, double distance );
    //ray GetRefractedRay(  const primitive& Primitive, const ray& original_ray, double distance, const int current_object, const int new_object );

    ray GetRefractedRay(int prev_primitive, int curr_primitive,  const ray& original_ray, double distance);
    
    ray GetDiffuseReflectedRay( const primitive& Primitive, const ray& original_ray, double distance );
};

}
#endif
