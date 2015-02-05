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
#include "GenerateEulerColors.h"

#include <limits>

#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Math/MatrixMath.h"

#include "Generic/GenericConstants.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenerateEulerColors::GenerateEulerColors() :
  AbstractFilter(),
  m_CellPhasesArrayPath(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::Phases),
  m_CrystalStructuresArrayPath(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::CellEnsembleAttributeMatrixName, DREAM3D::EnsembleData::CrystalStructures),
  m_CellEulerAnglesArrayPath(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::EulerAngles),
  m_GoodVoxelsArrayPath(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::GoodVoxels),
  m_CellEulerColorsArrayName(DREAM3D::CellData::EulerColor),
  m_UseGoodVoxels(false),
  m_CellPhasesArrayName(DREAM3D::CellData::Phases),
  m_CellPhases(NULL),
  m_CellEulerAnglesArrayName(DREAM3D::CellData::EulerAngles),
  m_CellEulerAngles(NULL),
  m_CellEulerColors(NULL),
  m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
  m_CrystalStructures(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenerateEulerColors::~GenerateEulerColors()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateEulerColors::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FilterParameter::New("Required Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("Cell Phases", "CellPhasesArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getCellPhasesArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("Crystal Structures", "CrystalStructuresArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getCrystalStructuresArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("Cell Euler Angles", "CellEulerAnglesArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getCellEulerAnglesArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("Optional Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  QStringList linkedProps("GoodVoxelsArrayPath");
  parameters.push_back(LinkedBooleanFilterParameter::New("Apply to Good Voxels Only (Bad Voxels Will Be Black)", "UseGoodVoxels", getUseGoodVoxels(), linkedProps, true));
  parameters.push_back(FilterParameter::New("GoodVoxels", "GoodVoxelsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getGoodVoxelsArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("Created Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("CellEulerColors", "CellEulerColorsArrayName", FilterParameterWidgetType::StringWidget, getCellEulerColorsArrayName(), true, ""));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateEulerColors::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setUseGoodVoxels(reader->readValue("UseGoodVoxels", getUseGoodVoxels() ) );
  setCellEulerColorsArrayName(reader->readString("CellEulerColorsArrayName", getCellEulerColorsArrayName() ) );
  setGoodVoxelsArrayPath(reader->readDataArrayPath("GoodVoxelsArrayPath", getGoodVoxelsArrayPath() ) );
  setCellEulerAnglesArrayPath(reader->readDataArrayPath("CellEulerAnglesArrayPath", getCellEulerAnglesArrayPath() ) );
  setCrystalStructuresArrayPath(reader->readDataArrayPath("CrystalStructuresArrayPath", getCrystalStructuresArrayPath() ) );
  setCellPhasesArrayPath(reader->readDataArrayPath("CellPhasesArrayPath", getCellPhasesArrayPath() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int GenerateEulerColors::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(UseGoodVoxels)
  DREAM3D_FILTER_WRITE_PARAMETER(CellEulerColorsArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(GoodVoxelsArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(CellEulerAnglesArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(CrystalStructuresArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(CellPhasesArrayPath)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateEulerColors::dataCheck()
{
  DataArrayPath tempPath;
  setErrorCondition(0);

  QVector<size_t> dims(1, 1);
  m_CellPhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getCellPhasesArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CellPhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CellPhases = m_CellPhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  //typedef DataArray<unsigned int> XTalStructArrayType;
  m_CrystalStructuresPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<unsigned int>, AbstractFilter>(this, getCrystalStructuresArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CrystalStructuresPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  dims[0] = 3;
  m_CellEulerAnglesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getCellEulerAnglesArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CellEulerAnglesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CellEulerAngles = m_CellEulerAnglesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  tempPath.update(m_CellEulerAnglesArrayPath.getDataContainerName(), m_CellEulerAnglesArrayPath.getAttributeMatrixName(), getCellEulerColorsArrayName() );
  m_CellEulerColorsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint8_t>, AbstractFilter, uint8_t>(this, tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CellEulerColorsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CellEulerColors = m_CellEulerColorsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  // The good voxels array is optional, If it is available we are going to use it, otherwise we are going to create it
  dims[0] = 1;
  if (getUseGoodVoxels() == true)
  {
    // The good voxels array is optional, If it is available we are going to use it, otherwise we are going to create it
    dims[0] = 1;
    m_GoodVoxelsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<bool>, AbstractFilter>(this, getGoodVoxelsArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_GoodVoxelsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_GoodVoxels = m_GoodVoxelsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }
  else
  {
    m_GoodVoxels = NULL;
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateEulerColors::preflight()
{
  setInPreflight(true);
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
  setInPreflight(false);

  /* *** THIS FILTER NEEDS TO BE CHECKED *** */
  setErrorCondition(0xABABABAB);
  QString ss = QObject::tr("Filter is NOT updated for IGeometry Redesign. A Programmer needs to check this filter. Please report this to the DREAM3D developers.");
  notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  /* *** THIS FILTER NEEDS TO BE CHECKED *** */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateEulerColors::execute()
{
  int err = 0;
  QString ss;
  setErrorCondition(err);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  int64_t totalPoints = m_CellEulerAnglesPtr.lock()->getNumberOfTuples();

  size_t index = 0;

  int phase;
  float twoPi = 2.0f * DREAM3D::Constants::k_Pi;
  float halfPi = 0.5f * DREAM3D::Constants::k_Pi;
  float thirdPi = 0.333 * DREAM3D::Constants::k_Pi;
  float twoThirdPi = 0.6666 * DREAM3D::Constants::k_Pi;

  // Write the Euler Coloring Cell Data
  for (int i = 0; i < totalPoints; ++i)
  {
    phase = m_CellPhases[i];
    index = i * 3;

    if(m_CrystalStructures[phase] == Ebsd::CrystalStructure::Cubic_High) // m3m Symmetry Cubic
    {
      m_CellEulerColors[index] =    static_cast<unsigned char>(m_CellEulerAngles[index] / twoPi  * 255.0f);
      m_CellEulerColors[index + 1] = static_cast<unsigned char>(m_CellEulerAngles[index + 1] / halfPi * 255.0f);
      m_CellEulerColors[index + 2] = static_cast<unsigned char>(m_CellEulerAngles[index + 2] / halfPi * 255.0f);
    }
    else if(m_CrystalStructures[phase] == Ebsd::CrystalStructure::Hexagonal_High)
    {
      m_CellEulerColors[index] =    static_cast<unsigned char>(m_CellEulerAngles[index] / twoPi  * 255.0f);
      m_CellEulerColors[index + 1] = static_cast<unsigned char>(m_CellEulerAngles[index + 1] / halfPi * 255.0f);
      m_CellEulerColors[index + 2] = static_cast<unsigned char>(m_CellEulerAngles[index + 2] / thirdPi * 255.0f);
    }
    else if(m_CrystalStructures[phase] == Ebsd::CrystalStructure::Trigonal_High)
    {
      m_CellEulerColors[index] =    static_cast<unsigned char>(m_CellEulerAngles[index] / twoPi  * 255.0f);
      m_CellEulerColors[index + 1] = static_cast<unsigned char>(m_CellEulerAngles[index + 1] / halfPi * 255.0f);
      m_CellEulerColors[index + 2] = static_cast<unsigned char>(m_CellEulerAngles[index + 2] / twoThirdPi * 255.0f);
    }
  }
  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer GenerateEulerColors::newFilterInstance(bool copyFilterParameters)
{
  GenerateEulerColors::Pointer filter = GenerateEulerColors::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GenerateEulerColors::getCompiledLibraryName()
{ return Generic::GenericBaseName; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GenerateEulerColors::getGroupName()
{ return DREAM3D::FilterGroups::GenericFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GenerateEulerColors::getSubGroupName()
{ return DREAM3D::FilterSubGroups::MiscFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GenerateEulerColors::getHumanLabel()
{ return "Generate Euler Colors"; }

