/* ============================================================================
 * Copyright (c) 2011 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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

#include "InputCrystalCompliances.h"

#include <sstream>

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/DataArrays/IDataArray.h"
#include "OrientationLib/Math/OrientationMath.h"

#include "Generic/GenericConstants.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
InputCrystalCompliances::InputCrystalCompliances() :
  AbstractFilter(),
  m_CellEnsembleAttributeMatrixName(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::CellEnsembleAttributeMatrixName, ""),
  m_CrystalCompliancesArrayName("CrystalCompliances"),
  m_CrystalCompliances(NULL)
{
  m_Compliances.v11 = 1;
  m_Compliances.v12 = 1;
  m_Compliances.v13 = 1;
  m_Compliances.v14 = 0;
  m_Compliances.v15 = 0;
  m_Compliances.v16 = 0;

  m_Compliances.v22 = 1;
  m_Compliances.v23 = 1;
  m_Compliances.v24 = 0;
  m_Compliances.v25 = 0;
  m_Compliances.v26 = 0;

  m_Compliances.v33 = 1;
  m_Compliances.v34 = 0;
  m_Compliances.v35 = 0;
  m_Compliances.v36 = 0;

  m_Compliances.v44 = 1;
  m_Compliances.v45 = 0;
  m_Compliances.v46 = 0;

  m_Compliances.v55 = 1;
  m_Compliances.v56 = 0;

  m_Compliances.v66 = 1;

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
InputCrystalCompliances::~InputCrystalCompliances()
{
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InputCrystalCompliances::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FilterParameter::New("Required Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("Compliance Values", "Compliances", FilterParameterWidgetType::Symmetric6x6Widget, getCompliances(), true, "10^-11 Pa^-1"));
  parameters.push_back(FilterParameter::New("Created Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("Cell Feature Attribute Matrix Name", "CellEnsembleAttributeMatrixName", FilterParameterWidgetType::AttributeMatrixSelectionWidget, getCellEnsembleAttributeMatrixName(), true, ""));
  parameters.push_back(FilterParameter::New("CrystalCompliances", "CrystalCompliancesArrayName", FilterParameterWidgetType::StringWidget, getCrystalCompliancesArrayName(), true, ""));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
void InputCrystalCompliances::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setCellEnsembleAttributeMatrixName(reader->readDataArrayPath("CellEnsembleAttributeMatrixName", getCellEnsembleAttributeMatrixName()));
  setCompliances(reader->readFloatVec21("Compliances", getCompliances() ) );
  setCrystalCompliancesArrayName(reader->readString("CrystalCompliancesArrayName", getCrystalCompliancesArrayName() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int InputCrystalCompliances::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(CellEnsembleAttributeMatrixName)
  DREAM3D_FILTER_WRITE_PARAMETER(Compliances)
  DREAM3D_FILTER_WRITE_PARAMETER(CrystalCompliancesArrayName)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InputCrystalCompliances::dataCheck()
{
  DataArrayPath tempPath;
  setErrorCondition(0);

  //create compliances
  QVector<size_t> dims(2, 6);//6 by 6 array
  tempPath.update(getCellEnsembleAttributeMatrixName().getDataContainerName(), getCellEnsembleAttributeMatrixName().getAttributeMatrixName(), getCrystalCompliancesArrayName() );
  m_CrystalCompliancesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, 0, dims);
  if( NULL != m_CrystalCompliancesPtr.lock().get() )
  { m_CrystalCompliances = m_CrystalCompliancesPtr.lock()->getPointer(0); }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InputCrystalCompliances::preflight()
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
void InputCrystalCompliances::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  //determine number of phases
  size_t numPhases = m_CrystalCompliancesPtr.lock()->getNumberOfTuples();

  //convert from 10^-2 GPa^-1 to Pa^-1
  m_Compliances.v11 /= 100000000000;
  m_Compliances.v12 /= 100000000000;
  m_Compliances.v13 /= 100000000000;
  m_Compliances.v14 /= 100000000000;
  m_Compliances.v15 /= 100000000000;
  m_Compliances.v16 /= 100000000000;

  m_Compliances.v22 /= 100000000000;
  m_Compliances.v23 /= 100000000000;
  m_Compliances.v24 /= 100000000000;
  m_Compliances.v25 /= 100000000000;
  m_Compliances.v26 /= 100000000000;

  m_Compliances.v33 /= 100000000000;
  m_Compliances.v34 /= 100000000000;
  m_Compliances.v35 /= 100000000000;
  m_Compliances.v36 /= 100000000000;

  m_Compliances.v44 /= 100000000000;
  m_Compliances.v45 /= 100000000000;
  m_Compliances.v46 /= 100000000000;

  m_Compliances.v55 /= 100000000000;
  m_Compliances.v56 /= 100000000000;

  m_Compliances.v66 /= 100000000000;

  //loop over each phase
  for(size_t i = 0; i < numPhases; i++)
  {
    //for now just give every phase the same values
    size_t index = 36 * i;
    m_CrystalCompliances[index + 0] = m_Compliances.v11;
    m_CrystalCompliances[index + 1] = m_Compliances.v12;
    m_CrystalCompliances[index + 2] = m_Compliances.v13;
    m_CrystalCompliances[index + 3] = m_Compliances.v14;
    m_CrystalCompliances[index + 4] = m_Compliances.v15;
    m_CrystalCompliances[index + 5] = m_Compliances.v16;

    m_CrystalCompliances[index + 6] = m_Compliances.v12;
    m_CrystalCompliances[index + 7] = m_Compliances.v22;
    m_CrystalCompliances[index + 8] = m_Compliances.v23;
    m_CrystalCompliances[index + 9] = m_Compliances.v24;
    m_CrystalCompliances[index + 10] = m_Compliances.v25;
    m_CrystalCompliances[index + 11] = m_Compliances.v26;

    m_CrystalCompliances[index + 12] = m_Compliances.v13;
    m_CrystalCompliances[index + 13] = m_Compliances.v23;
    m_CrystalCompliances[index + 14] = m_Compliances.v33;
    m_CrystalCompliances[index + 15] = m_Compliances.v34;
    m_CrystalCompliances[index + 16] = m_Compliances.v35;
    m_CrystalCompliances[index + 17] = m_Compliances.v36;

    m_CrystalCompliances[index + 18] = m_Compliances.v14;
    m_CrystalCompliances[index + 19] = m_Compliances.v24;
    m_CrystalCompliances[index + 20] = m_Compliances.v34;
    m_CrystalCompliances[index + 21] = m_Compliances.v44;
    m_CrystalCompliances[index + 22] = m_Compliances.v45;
    m_CrystalCompliances[index + 23] = m_Compliances.v46;

    m_CrystalCompliances[index + 24] = m_Compliances.v15;
    m_CrystalCompliances[index + 25] = m_Compliances.v25;
    m_CrystalCompliances[index + 26] = m_Compliances.v35;
    m_CrystalCompliances[index + 27] = m_Compliances.v45;
    m_CrystalCompliances[index + 28] = m_Compliances.v55;
    m_CrystalCompliances[index + 29] = m_Compliances.v56;

    m_CrystalCompliances[index + 30] = m_Compliances.v16;
    m_CrystalCompliances[index + 31] = m_Compliances.v26;
    m_CrystalCompliances[index + 32] = m_Compliances.v36;
    m_CrystalCompliances[index + 33] = m_Compliances.v46;
    m_CrystalCompliances[index + 34] = m_Compliances.v56;
    m_CrystalCompliances[index + 35] = m_Compliances.v66;
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer InputCrystalCompliances::newFilterInstance(bool copyFilterParameters)
{
  InputCrystalCompliances::Pointer filter = InputCrystalCompliances::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString InputCrystalCompliances::getCompiledLibraryName()
{ return Generic::GenericBaseName; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString InputCrystalCompliances::getGroupName()
{ return DREAM3D::FilterGroups::GenericFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString InputCrystalCompliances::getSubGroupName()
{ return DREAM3D::FilterSubGroups::CrystallographyFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString InputCrystalCompliances::getHumanLabel()
{ return "Input Crystal Compliances"; }

