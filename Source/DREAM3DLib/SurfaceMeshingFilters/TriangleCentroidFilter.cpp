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
#include "TriangleCentroidFilter.h"


#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include <tbb/partitioner.h>
#include <tbb/task_scheduler_init.h>
#endif

#include "DREAM3DLib/Common/DREAM3DMath.h"

/**
 * @brief The CalculateCentroidsImpl class
 */
class CalculateCentroidsImpl
{
    StructArray<SurfaceMesh::DataStructures::Vert_t>::Pointer m_Nodes;
    StructArray<SurfaceMesh::DataStructures::Face_t>::Pointer m_Triangles;
    double* m_Centroids;

  public:
    CalculateCentroidsImpl(StructArray<SurfaceMesh::DataStructures::Vert_t>::Pointer nodes, StructArray<SurfaceMesh::DataStructures::Face_t>::Pointer triangles, double* centroids) :
      m_Nodes(nodes),
      m_Triangles(triangles),
      m_Centroids(centroids)
    {}
    virtual ~CalculateCentroidsImpl(){}

    void generate(size_t start, size_t end) const
    {

      SurfaceMesh::DataStructures::Vert_t* nodes = m_Nodes->GetPointer(0);
      SurfaceMesh::DataStructures::Face_t* triangles = m_Triangles->GetPointer(0);

      for (size_t i = start; i < end; i++)
      {
        m_Centroids[i*3]  = (nodes[triangles[i].verts[0]].pos[0] + nodes[triangles[i].verts[1]].pos[0] + nodes[triangles[i].verts[2]].pos[0])/3.0;
        m_Centroids[i*3 + 1] = (nodes[triangles[i].verts[0]].pos[1] + nodes[triangles[i].verts[1]].pos[1] + nodes[triangles[i].verts[2]].pos[1])/3.0;
        m_Centroids[i*3 + 2]  = (nodes[triangles[i].verts[0]].pos[2] + nodes[triangles[i].verts[1]].pos[2] + nodes[triangles[i].verts[2]].pos[2])/3.0;
      }
    }

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
    void operator()(const tbb::blocked_range<size_t> &r) const
    {
      generate(r.begin(), r.end());
    }
#endif


};


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TriangleCentroidFilter::TriangleCentroidFilter() :
AbstractFilter(),
m_SurfaceMeshTriangleCentroidsArrayName(DREAM3D::CellData::SurfaceMeshTriangleCentroids),
m_SurfaceMeshTriangleCentroids(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TriangleCentroidFilter::~TriangleCentroidFilter()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TriangleCentroidFilter::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> parameters;
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TriangleCentroidFilter::writeFilterParameters(AbstractFilterParametersWriter* writer)
{
  /* Place code that will write the inputs values into a file. reference the
   AbstractFilterParametersWriter class for the proper API to use. */
  /*  writer->writeValue("OutputFile", getOutputFile() ); */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TriangleCentroidFilter::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  SurfaceMeshDataContainer* sm = getSurfaceMeshDataContainer();
  if(NULL == sm)
  {
    addErrorMessage(getHumanLabel(), "SurfaceMeshDataContainer is missing", -383);
    setErrorCondition(-384);
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
      CREATE_NON_PREREQ_DATA(sm, DREAM3D, CellData, SurfaceMeshTriangleCentroids, ss, double, DoubleArrayType, 0, voxels, 3)
    }

  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TriangleCentroidFilter::preflight()
{
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TriangleCentroidFilter::execute()
{
  int err = 0;
  std::stringstream ss;
  setErrorCondition(err);
  SurfaceMeshDataContainer* m = getSurfaceMeshDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The SurfaceMeshing DataContainer Object was NULL", -999);
    return;
  }
  setErrorCondition(0);
  notifyStatusMessage("Starting");

  StructArray<SurfaceMesh::DataStructures::Vert_t>::Pointer nodesPtr = getSurfaceMeshDataContainer()->getVertices();

  StructArray<SurfaceMesh::DataStructures::Face_t>::Pointer trianglesPtr = getSurfaceMeshDataContainer()->getFaces();
  size_t totalPoints = trianglesPtr->GetNumberOfTuples();

  // Run the data check to allocate the memory for the centroid array
  dataCheck(false, trianglesPtr->GetNumberOfTuples(), 0, 0);

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
  tbb::parallel_for(tbb::blocked_range<size_t>(0, totalPoints),
                    CalculateCentroidsImpl(nodesPtr, trianglesPtr, m_SurfaceMeshTriangleCentroids), tbb::auto_partitioner());

#else
  CalculateCentroidsImpl serial(nodesPtr, trianglesPtr, m_SurfaceMeshTriangleCentroids);
  serial.generate(0, totalPoints);
#endif

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage("Complete");
}
