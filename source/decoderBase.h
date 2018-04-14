/*  This file is part of YUView - The YUV player with advanced analytics toolset
*   <https://github.com/IENT/YUView>
*   Copyright (C) 2015  Institut für Nachrichtentechnik, RWTH Aachen University, GERMANY
*
*   This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 3 of the License, or
*   (at your option) any later version.
*
*   In addition, as a special exception, the copyright holders give
*   permission to link the code of portions of this program with the
*   OpenSSL library under certain conditions as described in each
*   individual source file, and distribute linked combinations including
*   the two.
*   
*   You must obey the GNU General Public License in all respects for all
*   of the code used other than OpenSSL. If you modify file(s) with this
*   exception, you may extend this exception to your version of the
*   file(s), but you are not obligated to do so. If you do not wish to do
*   so, delete this exception statement from your version. If you delete
*   this exception statement from all source files in the program, then
*   also delete it here.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DECODERBASE_H
#define DECODERBASE_H

#include "fileSourceAnnexBFile.h"
#include "statisticHandler.h"
#include "statisticsExtensions.h"
#include "videoHandlerYUV.h"

using namespace YUV_Internals;

/* This class is the abstract base class for all decoders. All decoders work like this:
 * 1. Create an instance and configure it (if required)
 * 2. Push data to the decoder until it returns that it can not take any more data. When you pushed all of the bitstream into the decoder, call setEOF.
 * 3. Read frames until no new frames are coming out. Go back to 2.
*/
class decoderBase
{
public:
  // Create a new decoder. cachingDecoder: Is this a decoder used for caching or interactive decoding?
  decoderBase(bool cachingDecoder=false);
  virtual ~decoderBase() {};

  // Does the loaded library support the extraction of prediction/residual data?
  virtual int nrSignalsSupported() const { return 1; }
  virtual QStringList getSignalNames() const { return QStringList() << "Reconstruction"; }
  void setDecodeSignal(int signalID) { if (signalID < nrSignalsSupported()) decodeSignal = signalID; }

  // If the current frame nr is valid (not -1), the current frame can be retrieved using getYUVFrameData.
  virtual bool getCurrentFrameNr() { return currentFrameNumber; }
  virtual QByteArray getYUVFrameData(int frameIdx) = 0;
  virtual yuvPixelFormat getYUVPixelFormat() = 0;
  QSize getFrameSize() { return frameSize; }

  // Get the statistics values for the current frame. In order to enable statistics retrievel, 
  // activate it, reset the decoder and decode to the current frame again.
  virtual bool statisticsSupported() const { return false; }
  virtual bool statisticsEnabled() const { return retrieveStatistics; }
  void enableStatisticsRetrieval() { retrieveStatistics = true; }
  virtual statisticsData getStatisticsData(int frameIdx, int typeIdx) = 0;
  virtual void fillStatisticList(statisticHandler &statSource) const = 0;

  // Error handling
  bool errorInDecoder() const { return decoderError; }
  QString decoderErrorString() const { return errorString; }

  // Get the full path and filename to the decoder library that is being used
  virtual QString getLibraryPath() const = 0;

  // Get the deocder name (everyting that is needed to identify the deocder library)
  // If needed, also version information (like HM 16.4)
  virtual QString getDecoderName() const = 0;
  
protected:
  
  int decodeSignal; ///< Which signal should be decoded?
  bool isCachingDecoder; ///< Is this the caching or the interactive decoder?

  int currentFrameNumber;
  bool retrieveStatistics;
  QSize frameSize;
  
  // Error handling
  void setError(const QString &reason);
  bool decoderError;
  QString errorString;
  
  // Statistics caching
  QHash<int, statisticsData> curPOCStats;  // cache of the statistics for the current POC [statsTypeID]
  int statsCacheCurPOC;                    // the POC of the statistics that are in the curPOCStats
};

#endif // DECODERBASE_H