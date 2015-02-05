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

#include "AbaqusSurfaceMeshWriter.h"

#include <stdio.h>

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>


#include "DREAM3DLib/Common/ScopedFileMonitor.hpp"

#include "IO/IOConstants.h"

#define TRI_ELEMENT_TYPE "SFM3D3"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbaqusSurfaceMeshWriter::AbaqusSurfaceMeshWriter() :
  AbstractFilter(),
  m_OutputFile(""),
  m_SurfaceMeshFaceLabelsArrayPath(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::FaceAttributeMatrixName, DREAM3D::FaceData::SurfaceMeshFaceLabels),
  m_SurfaceMeshFaceLabelsArrayName(DREAM3D::FaceData::SurfaceMeshFaceLabels),
  m_SurfaceMeshFaceLabels(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbaqusSurfaceMeshWriter::~AbaqusSurfaceMeshWriter()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbaqusSurfaceMeshWriter::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FileSystemFilterParameter::New("Output File", "OutputFile", FilterParameterWidgetType::OutputFileWidget, getOutputFile(), false, "", "*.inp"));
  parameters.push_back(FilterParameter::New("Required Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("SurfaceMeshFaceLabels", "SurfaceMeshFaceLabelsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getSurfaceMeshFaceLabelsArrayPath(), true, ""));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbaqusSurfaceMeshWriter::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setSurfaceMeshFaceLabelsArrayPath(reader->readDataArrayPath("SurfaceMeshFaceLabelsArrayPath", getSurfaceMeshFaceLabelsArrayPath() ) );
  setOutputFile( reader->readString( "OutputFile", getOutputFile() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AbaqusSurfaceMeshWriter::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(SurfaceMeshFaceLabelsArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(OutputFile)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbaqusSurfaceMeshWriter::dataCheck()
{
  setErrorCondition(0);
  if (m_OutputFile.isEmpty() == true)
  {
    setErrorCondition(-1003);
    notifyErrorMessage(getHumanLabel(), "Stl Output Directory is Not set correctly", -1003);
  }

  DataContainer::Pointer sm = getDataContainerArray()->getPrereqDataContainer<AbstractFilter>(this, getSurfaceMeshFaceLabelsArrayPath().getDataContainerName(), false);
  if(getErrorCondition() < 0) { return; }

  IGeometry::Pointer geom = sm->getGeometry();
  if(NULL == geom.get())
  {
    setErrorCondition(-385);
    QString ss = QObject::tr("DataContainer Geometry is missing.");
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  TriangleGeom::Pointer triangles = sm->getGeometryAs<TriangleGeom>();
  if(NULL == triangles.get())
  {
    setErrorCondition(-384);
    QString ss = QObject::tr("DataContainer Geometry is not compatible. The Geometry type is %1").arg(geom->getGeometryTypeAsString());
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  // We MUST have Nodes
  if (NULL == triangles->getVertices().get())
  {
    setErrorCondition(-386);
    notifyErrorMessage(getHumanLabel(), "DataContainer Geometry missing Vertices", getErrorCondition());
  }
  // We MUST have Triangles defined also.
  if (NULL == triangles->getTriangles().get())
  {
    setErrorCondition(-387);
    notifyErrorMessage(getHumanLabel(), "DataContainer Geometry missing Triangles", getErrorCondition());
  }
  else
  {
    QVector<size_t> dims(1, 2);
    m_SurfaceMeshFaceLabelsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getSurfaceMeshFaceLabelsArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_SurfaceMeshFaceLabelsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_SurfaceMeshFaceLabels = m_SurfaceMeshFaceLabelsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbaqusSurfaceMeshWriter::preflight()
{
  setInPreflight(true);
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
  setInPreflight(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbaqusSurfaceMeshWriter::execute()
{
  int err = 0;

  dataCheck();
  if(getErrorCondition() < 0) { return; }

  DataContainer::Pointer sm = getDataContainerArray()->getDataContainer(getSurfaceMeshFaceLabelsArrayPath().getDataContainerName());

  // Make sure any directory path is also available as the user may have just typed
  // in a path without actually creating the full path
  QFileInfo fi(getOutputFile());
  QDir parentPath = fi.path();
  if(!parentPath.mkpath("."))
  {
    QString ss = QObject::tr("Error creating parent path '%1'").arg(parentPath.absolutePath());
    setErrorCondition(-8005);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  TriangleGeom::Pointer triangleGeom = sm->getGeometryAs<TriangleGeom>();

  // Store all the unique Spins
  QSet<int> uniqueSpins;
  for (int64_t i = 0; i < triangleGeom->getNumberOfTris(); i++)
  {
    uniqueSpins.insert(m_SurfaceMeshFaceLabels[i * 2]);
    uniqueSpins.insert(m_SurfaceMeshFaceLabels[i * 2 + 1]);
  }

  FILE* f = fopen(m_OutputFile.toLatin1().data(), "wb");
  ScopedFileMonitor fileMonitor(f);

  err = writeHeader(f, triangleGeom->getNumberOfVertices(), triangleGeom->getNumberOfTris(), uniqueSpins.size() - 1);
  if(err < 0)
  {
    setErrorCondition(-8000);
  }
  err = writeNodes(f);
  if(err < 0)
  {
    setErrorCondition(-8001);
  }
  err = writeTriangles(f);
  if(err < 0)
  {
    setErrorCondition(-8002);
  }
  err = writeFeatures(f);
  if(err < 0)
  {
    setErrorCondition(-8003);
  }

  setErrorCondition(0);
  notifyStatusMessage(getHumanLabel(), "Complete");

  return;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AbaqusSurfaceMeshWriter::writeHeader(FILE* f, int64_t nodeCount, int64_t triCount, int featureCount)
{
  if (NULL == f)
  {
    return -1;
  }
  fprintf(f, "*HEADING\n");
  fprintf(f, "** File Created with DREAM3D Version %s\n", DREAM3DLib::Version::Complete().toStdString().c_str());
  fprintf(f, "** There are 3 Sections to this INP File: Nodes, Elements and Sets of Elements for each grain\n");
  fprintf(f, "** This file represents a trianguglar based mesh. The element type selected is %s for the triangles\n", TRI_ELEMENT_TYPE);
  fprintf(f, "** This file is an experimental output from DREAM3D. The user is responsible for verifying all elements in Abaqus\n");
  fprintf(f, "** We have selected to use a 'shell' element type currently. No boundary elements are written\n");
  fprintf(f, "**Number of Nodes: %lld     Number of Triangles: %lld   Number of Grains: %d\n", (long long int)nodeCount, (long long int)triCount, featureCount);
  fprintf(f, "*PREPRINT,ECHO=NO,HISTORY=NO,MODEL=NO\n");
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AbaqusSurfaceMeshWriter::writeNodes(FILE* f)
{
  TriangleGeom::Pointer triangleGeom = getDataContainerArray()->getDataContainer(getSurfaceMeshFaceLabelsArrayPath().getDataContainerName())->getGeometryAs<TriangleGeom>();
  float* nodes = triangleGeom->getVertexPointer(0);

  int64_t numNodes = triangleGeom->getNumberOfVertices();
  int err = 0;
  fprintf(f, "*Node,NSET=NALL\n");
  //1, 72.520433763730, 70.306420652241, 100.000000000000
  // Abaqus Starts number at 1 NOT 0(Zero).
  for(int64_t i = 1; i <= numNodes; ++i)
  {
    fprintf(f, "%lld, %0.6f, %0.6f, %0.6f\n", (long long int)i, nodes[(i-1)*3], nodes[(i-1)*3+1], nodes[(i-1)*3+2]);
  }

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AbaqusSurfaceMeshWriter::writeTriangles(FILE* f)
{
  int err = 0;
  TriangleGeom::Pointer triangleGeom = getDataContainerArray()->getDataContainer(getSurfaceMeshFaceLabelsArrayPath().getDataContainerName())->getGeometryAs<TriangleGeom>();
  int64_t numTri = triangleGeom->getNumberOfTris();
  int64_t* triangles = triangleGeom->getTriPointer(0);

  fprintf(f, "*ELEMENT, TYPE=%s\n", TRI_ELEMENT_TYPE);
  for(int64_t i = 1; i <= numTri; ++i)
  {
    // When we get the node index, add 1 to it because Abaqus number is 1 based.
    int64_t nId0 = triangles[(i-1)*3] + 1;
    int64_t nId1 = triangles[(i-1)*3+1] + 1;
    int64_t nId2 = triangles[(i-1)*3+2] + 1;
    fprintf(f, "%lld, %lld, %lld, %lld\n", (long long int)i, (long long int)nId0, (long long int)nId1, (long long int)nId2);
  }
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AbaqusSurfaceMeshWriter::writeFeatures(FILE* f)
{

  //*Elset, elset=Feature1
  //1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16

  int err = 0;

  TriangleGeom::Pointer triangleGeo = getDataContainerArray()->getDataContainer(getSurfaceMeshFaceLabelsArrayPath().getDataContainerName())->getGeometryAs<TriangleGeom>();

//  VertexArray::Pointer nodesPtr = getDataContainerArray()->getDataContainerAs<SurfaceDataContainer>(getSurfaceMeshFaceLabelsArrayPath().getDataContainerName())->getVertices();
 // FaceArray::Pointer trianglePtr = getDataContainerArray()->getDataContainerAs<SurfaceDataContainer>(getSurfaceMeshFaceLabelsArrayPath().getDataContainerName())->getFaces();
  // Get the Labels(FeatureIds or Region Ids) for the triangles

  int64_t nTriangles = triangleGeo->getNumberOfTris();

  // Store all the unique Spins
  QSet<int> uniqueSpins;
  for (int64_t i = 0; i < nTriangles; i++)
  {
    uniqueSpins.insert(m_SurfaceMeshFaceLabels[i * 2]);
    uniqueSpins.insert(m_SurfaceMeshFaceLabels[i * 2 + 1]);
  }

  int spin = 0;

  //Loop over the unique Spins
  for (QSet<int>::iterator spinIter = uniqueSpins.begin(); spinIter != uniqueSpins.end(); ++spinIter )
  {
    spin = *spinIter;
    if(spin < 0) { continue; }

    fprintf(f, "*ELSET, ELSET=Feature%d\n", spin);

    {
      QString ss = QObject::tr("Writing ELSET for Feature Id %1").arg(spin);
      notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
    }

    // Loop over all the triangles for this spin
    int lineCount = 0;
    for(int t = 0; t < nTriangles; ++t)
    {
      if (m_SurfaceMeshFaceLabels[t * 2] != spin && m_SurfaceMeshFaceLabels[t * 2 + 1] != spin)
      {
        continue; // We do not match either spin so move to the next triangle
      }

      // Only print 15 Triangles per line
      if (lineCount == 15)
      {
        fprintf(f, ", %d\n", t);
        lineCount = 0;
      }
      else if(lineCount == 0) // First value on the line
      {
        fprintf(f, "%d", t);
        lineCount++;
      }
      else
      {
        fprintf(f, ", %d", t);
        lineCount++;
      }

    }
    // Make sure we have a new line at the end of the section
    if (lineCount != 0)
    {
      fprintf(f, "\n");
    }
  }
  return err;
}




// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer AbaqusSurfaceMeshWriter::newFilterInstance(bool copyFilterParameters)
{
  AbaqusSurfaceMeshWriter::Pointer filter = AbaqusSurfaceMeshWriter::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AbaqusSurfaceMeshWriter::getCompiledLibraryName()
{ return IO::IOBaseName; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AbaqusSurfaceMeshWriter::getGroupName()
{ return DREAM3D::FilterGroups::IOFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AbaqusSurfaceMeshWriter::getSubGroupName()
{ return DREAM3D::FilterSubGroups::OutputFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AbaqusSurfaceMeshWriter::getHumanLabel()
{ return "Write Abaqus SurfaceMesh"; }

