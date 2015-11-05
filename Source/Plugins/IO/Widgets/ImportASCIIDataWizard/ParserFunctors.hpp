/* ============================================================================
 * Copyright (c) 2011, Michael A. Jackson (BlueQuartz Software)
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
 * Neither the name of Michael A. Jackson nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef _ParserFunctors_hpp_
#define _ParserFunctors_hpp_

#include <QtCore/QByteArray>

#include "SIMPLib/SIMPLib.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class Int8Functor
{
  public:
    virtual ~Int8Functor() {}

    int8_t operator() (QByteArray token, bool &ok)
    {
      int8_t value = token.toInt(&ok);
      return value;
    }

  private:
    Int8Functor(const Int8Functor&); // Copy Constructor Not Implemented
    void operator=(const Int8Functor&); // Operator '=' Not Implemented
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class UInt8Functor
{
public:
  virtual ~UInt8Functor() {}

  uint8_t operator() (QByteArray token, bool &ok)
  {
    uint8_t value = token.toUInt(&ok);
    return value;
  }

private:
  UInt8Functor(const UInt8Functor&); // Copy Constructor Not Implemented
  void operator=(const UInt8Functor&); // Operator '=' Not Implemented
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class Int16Functor
{
public:
  virtual ~Int16Functor() {}

  int16_t operator() (QByteArray token, bool &ok)
  {
    int16_t value = token.toInt(&ok);
    return value;
  }

private:
  Int16Functor(const Int16Functor&); // Copy Constructor Not Implemented
  void operator=(const Int16Functor&); // Operator '=' Not Implemented
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class UInt16Functor
{
public:
  virtual ~UInt16Functor() {}

  uint16_t operator() (QByteArray token, bool &ok)
  {
    uint16_t value = token.toUInt(&ok);
    return value;
  }
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class Int32Functor
{
public:
  virtual ~Int32Functor() {}

  int32_t operator()(QByteArray token, bool &ok)
  {
    int32_t value = token.toInt(&ok);
    return value;
  }
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class UInt32Functor
{
public:
  virtual ~UInt32Functor() {}

  uint32_t operator() (QByteArray token, bool &ok)
  {
    uint32_t value = token.toUInt(&ok);
    return value;
  }

private:
  UInt32Functor(const UInt32Functor&); // Copy Constructor Not Implemented
  void operator=(const UInt32Functor&); // Operator '=' Not Implemented
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class Int64Functor
{
public:
  virtual ~Int64Functor() {}

  int64_t operator() (QByteArray token, bool &ok)
  {
    int64_t value = token.toLongLong(&ok);
    return value;
  }

private:
  Int64Functor(const Int64Functor&); // Copy Constructor Not Implemented
  void operator=(const Int64Functor&); // Operator '=' Not Implemented
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class UInt64Functor
{
public:
  virtual ~UInt64Functor() {}

  uint64_t operator() (QByteArray token, bool &ok)
  {
    uint64_t value = token.toULongLong(&ok);
    return value;
  }

private:
  UInt64Functor(const UInt64Functor&); // Copy Constructor Not Implemented
  void operator=(const UInt64Functor&); // Operator '=' Not Implemented
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class FloatFunctor
{
public:
  virtual ~FloatFunctor() {}

  float operator() (QByteArray token, bool &ok)
  {
    float value = token.toFloat(&ok);
    return value;
  }

private:
  FloatFunctor(const FloatFunctor&); // Copy Constructor Not Implemented
  void operator=(const FloatFunctor&); // Operator '=' Not Implemented
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class DoubleFunctor
{
public:
  virtual ~DoubleFunctor() {}

  double operator() (QByteArray token, bool &ok)
  {
    double value = token.toDouble(&ok);
    return value;
  }

private:
  DoubleFunctor(const DoubleFunctor&); // Copy Constructor Not Implemented
  void operator=(const DoubleFunctor&); // Operator '=' Not Implemented
};

#endif /* PARSERFUNCTORS_HPP_ */

