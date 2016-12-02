/* ============================================================================
* Copyright (c) 2009-2016 BlueQuartz Software, LLC
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
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
* contributors may be used to endorse or promote products derived from this software
* without specific prior written permission.
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
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef _readhdf5treemodel_h_
#define _readhdf5treemodel_h_

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <QFileIconProvider>

#include <hdf5.h>

class ReadHDF5TreeModelItem;

class ReadHDF5TreeModel : public QAbstractItemModel
{
  Q_OBJECT

public:
  ReadHDF5TreeModel(hid_t fileId, QObject *parent = 0);
  ~ReadHDF5TreeModel();

  /**
   * @brief data
   * @param index
   * @param role
   * @return
   */
  QVariant data(const QModelIndex &index, int role) const;

  /**
   * @brief QAbstractItemModel::setData
   * @param index
   * @param value
   * @param role
   * @return
   */
  bool setData(const QModelIndex &index, const QVariant &value, int role);

  /**
   * @brief flags
   * @param index
   * @return
   */
  Qt::ItemFlags flags(const QModelIndex &index) const;

  /**
   * @brief headerData
   * @param section
   * @param orientation
   * @param role
   * @return
   */
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

  /**
   * @brief index
   * @param row
   * @param column
   * @param parent
   * @return
   */
  QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;

  /**
   * @brief parent
   * @param index
   * @return
   */
  QModelIndex parent(const QModelIndex &index) const;

  /**
   * @brief rowCount
   * @param parent
   * @return
   */
  int rowCount(const QModelIndex &parent = QModelIndex()) const;

  /**
   * @brief columnCount
   * @param parent
   * @return
   */
  int columnCount(const QModelIndex &parent = QModelIndex()) const;

  /**
   * @brief hdfPathForIndex
   * @param index
   * @return
   */
  QString hdfPathForIndex(const QModelIndex &index);

  /**
   * @brief hasChildren
   * @param parent
   * @return
   */
  bool hasChildren(const QModelIndex &parent) const;

  /**
   * @brief getSelectedHDF5Paths
   * @return
   */
  QList<QString> getSelectedHDF5Paths();

signals:
  void modelChanged();

private:
  ReadHDF5TreeModelItem*                    m_RootItem;
  hid_t                                     m_FileId;
  QFileIconProvider                         m_IconProvider;
  QList<QString>                            m_SelectedHDF5Paths;

  /**
   * @brief setupModelData
   */
  void setupModelData();
};


#endif /* _readhdf5treemodel_h_ */