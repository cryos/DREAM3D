/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice, this
 * list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * Neither the name of Michael A. Groeber, Michael A. Jackson, the US Air Force,
 * BlueQuartz Software nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior written
 * permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  This code was written under United States Air Force Contract number
 *                           FA8650-07-D-5800
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "VisualizeGBCD.h"


#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include <tbb/partitioner.h>
#include <tbb/task_scheduler_init.h>
#endif

#include "DREAM3DLib/Common/MatrixMath.h"
#include "DREAM3DLib/Common/DREAM3DMath.h"

const static float m_pi = static_cast<float>(M_PI);
const static float m_pi2 = static_cast<float>(2*M_PI);


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VisualizeGBCD::VisualizeGBCD() :
  SurfaceMeshFilter(),
  m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
  m_GBCDArrayName(DREAM3D::EnsembleData::GBCD),
  m_CrystalStructures(NULL),
  m_GBCD(NULL)
{
    m_OrientationOps = OrientationMath::getOrientationOpsVector();
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VisualizeGBCD::~VisualizeGBCD()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VisualizeGBCD::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> parameters;
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VisualizeGBCD::writeFilterParameters(AbstractFilterParametersWriter* writer)
{
  /* Place code that will write the inputs values into a file. reference the
   AbstractFilterParametersWriter class for the proper API to use. */
  /*  writer->writeValue("OutputFile", getOutputFile() ); */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VisualizeGBCD::dataCheckSurfaceMesh(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  SurfaceMeshDataContainer* sm = getSurfaceMeshDataContainer();
  if(NULL == sm)
  {
    addErrorMessage(getHumanLabel(), "SurfaceMeshDataContainer is missing", -383);
    setErrorCondition(-383);
  }
  else
  {
    // We MUST have Nodes
    if(sm->getVertices().get() == NULL)
    {
      addErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Nodes", -384);
      setErrorCondition(-384);
    }

    // We MUST have Triangles defined also.
    if(sm->getFaces().get() == NULL)
    {
      addErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Triangles", -383);
      setErrorCondition(-384);
    }
    else
    {
		GET_PREREQ_DATA(sm, DREAM3D, EnsembleData, GBCD, ss, -388, float, FloatArrayType, ensembles, (36*18*36*36*18))
	}

  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VisualizeGBCD::preflight()
{
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  dataCheckSurfaceMesh(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VisualizeGBCD::execute()
{
  int err = 0;
  std::stringstream ss;
  setErrorCondition(err);
  SurfaceMeshDataContainer* sm = getSurfaceMeshDataContainer();
  if(NULL == sm)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The SurfaceMeshing DataContainer Object was NULL", -999);
    return;
  }
  VoxelDataContainer* m = getVoxelDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The SurfaceMeshing DataContainer Object was NULL", -999);
    return;
  }
  setErrorCondition(0);
  notifyStatusMessage("Starting");

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
  tbb::task_scheduler_init init;
  bool doParallel = true;
#endif

  DREAM3D::SurfaceMesh::VertListPointer_t nodesPtr = sm->getVertices();

  DREAM3D::SurfaceMesh::FaceListPointer_t trianglesPtr = sm->getFaces();
  size_t totalFaces = trianglesPtr->GetNumberOfTuples();

  // Run the data check to allocate the memory for the centroid array
  dataCheckSurfaceMesh(false, 0, totalFaces, sm->getNumEnsembleTuples());

//  FloatArrayType::Pointer gbcdDeltasArray = FloatArrayType::NullPointer();
//  FloatArrayType::Pointer gbcdLimitsArray = FloatArrayType::NullPointer();
//  Int32ArrayType::Pointer gbcdSizesArray = Int32ArrayType::NullPointer();
//  gbcdDeltasArray = FloatArrayType::CreateArray(5, "GBCDDeltas");
//  gbcdDeltasArray->SetNumberOfComponents(1);
//  gbcdDeltasArray->initializeWithZeros();
//  gbcdLimitsArray = FloatArrayType::CreateArray(10, "GBCDLimits");
//  gbcdLimitsArray->SetNumberOfComponents(1);
//  gbcdLimitsArray->initializeWithZeros();
//  gbcdSizesArray = Int32ArrayType::CreateArray(5, "GBCDSizes");
//  gbcdSizesArray->SetNumberOfComponents(1);
//  gbcdSizesArray->initializeWithZeros();
//  float* m_GBCDdeltas = gbcdDeltasArray->GetPointer(0);
//  int* m_GBCDsizes = gbcdSizesArray->GetPointer(0);
//  float* m_GBCDlimits = gbcdLimitsArray->GetPointer(0);
//
//  m_GBCDlimits[0] = 0.0;
//  m_GBCDlimits[1] = cosf(1.0*m_pi);
//  m_GBCDlimits[2] = 0.0;
//  m_GBCDlimits[3] = 0.0;
//  m_GBCDlimits[4] = cosf(1.0*m_pi);
//  m_GBCDlimits[5] = 2.0*m_pi;
//  m_GBCDlimits[6] = cosf(0.0);
//  m_GBCDlimits[7] = 2.0*m_pi;
//  m_GBCDlimits[8] = 2.0*m_pi;
//  m_GBCDlimits[9] = cosf(0.0);
//
//  float binsize = 10.0*m_pi/180.0;
//  float binsize2 = binsize*(2.0/m_pi);
//  m_GBCDdeltas[0] = binsize;
//  m_GBCDdeltas[1] = binsize2;
//  m_GBCDdeltas[2] = binsize;
//  m_GBCDdeltas[3] = binsize;
//  m_GBCDdeltas[4] = binsize2;
//
//  m_GBCDsizes[0] = int((m_GBCDlimits[5]-m_GBCDlimits[0])/m_GBCDdeltas[0]);
//  m_GBCDsizes[1] = int((m_GBCDlimits[6]-m_GBCDlimits[1])/m_GBCDdeltas[1]);
//  m_GBCDsizes[2] = int((m_GBCDlimits[7]-m_GBCDlimits[2])/m_GBCDdeltas[2]);
//  m_GBCDsizes[3] = int((m_GBCDlimits[8]-m_GBCDlimits[3])/m_GBCDdeltas[3]);
//  m_GBCDsizes[4] = int((m_GBCDlimits[9]-m_GBCDlimits[4])/m_GBCDdeltas[4]);
//    
//  int inversion = 1;
//  float tol = 5.0;
//      
//  FloatArrayType::Pointer xyz_temp_ph = FloatArrayType::NullPointer();
//  xyz_temp_ph = FloatArrayType::CreateArray((3*m_GBCDsizes[3]*m_GBCDsizes[4]),"xyz");
//  
//  xyz_temp_ph[1:2,*] = Array_indices(Reform(gbcd[0,0,0,*,*]), Findgen(szgbcd[4],szgbcd[5]))
//  xyz_temp_ph[1,*] *= delta[3]
//
//  xyz_temp_ph[1,*] += (minGBCD[3])
//  xyz_temp_ph[2,*] *= delta[4]
//
//  xyz_temp_ph[2,*] += (minGBCD[4])
//  xyz_temp_ph[2,*] = Acos(xyz_temp_ph[2,*])
//  
//  Sphere2cart, xyz_temp_ph, xyz_temp
//  xyz_temp1 = Fltarr(4, szgbcd[4]*szgbcd[5])
//  xyz_temp1[0:2,*] = xyz_temp
//  
//  float mis_quat[5];
//  float mis_quat1[5];
//  float mis_quat2[5];
//  float miseuler[3];
//  float miseuler1[3];
//
//  misvect = mis_vect/Sqrt(Total(mis_vect^2.))
//  misvect *= mis_angle/!radeg
//  
//  miseuler = Rotmat2eulerang(Misvect2rotmat2(misvect))
//  mis_quat = Eulerang2quat(miseuler)
//    
//  nchunk = szgbcd[4]*szgbcd[5]
//  xyz = Fltarr(4,nchunk*n_sym*n_sym*2)
//  counter = 0l
//  for(int q=0,q<2,q++)
//  {
//    if(q == 1)
//	{ 
//		mis_quat = OrientationMath::invertQuaternion(mis_quat);
//	}
//	n_sym = m_OrientationOps[1]->getNumSymmetryOps();
//    for(int i=0, i<n_sym, i++)
//	{
//      mis_quat1 = Quat_multiply(symmetry[*,i], mis_quat)
//      for(int j=0, j<n_sym, j++)
//	  {
//        mis_quat2 = Quat_multiply(mis_quat1, Quat_inverse(symmetry[*,j]))
//        mis_euler1 = Quat2eulerang(mis_quat2)
//        
//        mis_euler1[1] = Cos(mis_euler1[1])
//        
//        location = Long((mis_euler1-mingbcd[0:2])/delta[0:2])
//        //make sure that euler angles are within the GBCD space
//        if(Min(location) >= 0 && Min(location < szGBCD[1:3]) > 0)
//		{
//          interface_D = gbcd[location[0], location[1], location[2], *,*]
//          ;stop
//          
//          xyz_temp1[3,*] =  Reform(interface_D, szgbcd[4]*szgbcd[5])
//          
//          trash = Quat_vector(Quat_inverse(symmetry[*,i]),xyz_temp )
//          if(q == 1)
//		  {
//			  trash = -1.*Quat_vector(Quat_inverse(mis_quat), trash)
//		  }
//          xyz_temp1[0:2,*] = trash
//          xyz[*,counter*nchunk:(counter+1)*nchunk -1] = xyz_temp1
//          counter += 1l
//          
//		}
//	  }
//	}
//  }
//  
//  xyz = xyz[*,0:nchunk*counter-1]
//  temp = xyz
//  poles_raw = xyz
//  intensity = xyz[3,*]
//  xyz = xyz[0:2,*]
//  xyz_ph = Cv_coord(from_rect=xyz, /to_sphere)
//  
//  Grid_input, xyz_ph[0,*], xyz_ph[1,*], intensity, Xyz, f1, /sphere, duplicates='Avg', Epsilon=tol/!radeg
//  intensity = f1
//  IF Keyword_set(no_smooth) THEN BEGIN
//    Return, [xyz, Transpose(intensity)]
//  ENDIF ELSE BEGIN
//    IF tol LT 4.0 THEN BEGIN
//      tol= 4.0
//      Print, "Tolerence has been reset to 4.0 degrees for speed reasons."
//    ENDIF
//    
//    delta = [3.,3.]
//    dimensions = [360.,180.]/delta
//    
//    start = [-!pi, -!pi/2.]
//    xout = Findgen(dimensions[0])*delta[0]/!radeg+start[0]
//    yout = Findgen(dimensions[1])*delta[1]/!radeg+start[1]
//    map = Griddata(xyz[0,*], xyz[1,*], xyz[2,*], f1, /sphere, method='Kriging',/grid, xout=xout, yout=yout)
//    ;map = Griddata(xyz[0,*], xyz[1,*], xyz[2,*], f1, /sphere, method='RadialBasisFunction',/grid, xout=xout, yout=yout)
//    xyztemp = Float(Array_indices(map, Findgen(Product(dimensions))))
//    xyztemp[0,*] = xout[xyztemp[0,*]]
//    xyztemp[1,*] = yout[xyztemp[1,*]]
//    intensity = Reform(map, 1,Product(dimensions))
//    xyz = Cv_coord(from_sphere = [xyztemp[0,*], xyztemp[1,*], (Fltarr(1,N_elements(map))+1.0)], /to_rect)
//    
//    Return, [xyz[0,*], xyz[1,*], xyz[2,*], intensity]
//  ENDELSE
//END

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage("Complete");
}

