/**
 * Copyright 2007 Doug Judd (Zvents, Inc.)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at 
 *
 * http://www.apache.org/licenses/LICENSE-2.0 
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

extern "C" {
#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
}

#include "Common/Logger.h"

#include "CommitLog.h"
#include "CommitLogReaderLocal.h"

using namespace hypertable;
using namespace std;

/**
 *
 */
CommitLogReaderLocal::CommitLogReaderLocal(std::string &logDirRoot, std::string &logDir) : mLogDir(), mLogFileInfo(), mBlockBuffer(sizeof(CommitLogHeaderT)) {
  LogFileInfoT fileInfo;
  struct stat statbuf;
  int fd;
  CommitLogHeaderT  header;

  cout << "CommitLogReaderLocal logDirRoot='" << logDirRoot << "' logDir='" << logDir << "'" << endl;

  if (logDirRoot.find('/', logDirRoot.length()-1) == string::npos)
    mLogDir = logDirRoot + logDir;
  else
    mLogDir = logDirRoot.substr(0, logDirRoot.length()-1) + logDir;

  if (mLogDir.find('/', mLogDir.length()-1) == string::npos)
    mLogDir += "/";

  DIR *dirp = opendir(mLogDir.c_str());
  if (dirp == 0) {
    LOG_VA_ERROR("opendir('%s') failed - %s", mLogDir.c_str(), strerror(errno));
    return;
  }
  struct dirent dent;
  struct dirent *dp;

  if (readdir_r(dirp, &dent, &dp) != 0) {
    LOG_VA_ERROR("readdir('%s') failed - %s", mLogDir.c_str(), strerror(errno));
    (void)closedir(dirp);
    return;
  }
    
  while (dp != 0) {

    if (dp->d_name[0] != '.' && dp->d_name[0] != 0) {

      char *endptr;
      long num = strtol(dp->d_name, &endptr, 10);
      if (*endptr != 0) {
	LOG_VA_WARN("Invalid file '%s' found in commit log directory '%s'", dp->d_name, mLogDir.c_str());
      }
      else {
	fileInfo.num = (uint32_t)num;
	fileInfo.fname = mLogDir + dp->d_name;
	mLogFileInfo.push_back(fileInfo);
      }
    }

    if (readdir_r(dirp, &dent, &dp) != 0) {
      LOG_VA_ERROR("readdir('%s') failed - %s", mLogDir.c_str(), strerror(errno));
      exit(1);
    }
  }
  (void)closedir(dirp);

  sort(mLogFileInfo.begin(), mLogFileInfo.end());

  for (size_t i=0; i<mLogFileInfo.size(); i++) {

    mLogFileInfo[i].timestamp = 0;

    if (stat(mLogFileInfo[i].fname.c_str(), &statbuf) != 0) {
      LOG_VA_ERROR("Problem trying to stat commit log file '%s' - %s", mLogFileInfo[i].fname.c_str(), strerror(errno));
      exit(1);
    }

    if (statbuf.st_size < (off_t)sizeof(CommitLogHeaderT))
      continue;

    if ((fd = open(mLogFileInfo[i].fname.c_str(), O_RDONLY)) == -1) {
      LOG_VA_ERROR("Problem trying to open commit log file '%s' - %s", mLogFileInfo[i].fname.c_str(), strerror(errno));
      exit(1);
    }

    if (pread(fd, &header, sizeof(CommitLogHeaderT), statbuf.st_size-sizeof(CommitLogHeaderT)) != sizeof(CommitLogHeaderT)) {
      LOG_VA_ERROR("Problem reading trailing header in commit log file '%s' - %s", mLogFileInfo[i].fname.c_str(), strerror(errno));
      exit(1);
    }

    close(fd);

    if (!strncmp(header.marker, "-BLOCK--", 8))
      mLogFileInfo[i].timestamp = header.timestamp;

    //cout << mLogFileInfo[i].num << ":  " << mLogFileInfo[i].fname << " " << mLogFileInfo[i].timestamp << endl;
  }

}



void CommitLogReaderLocal::InitializeRead(uint64_t timestamp) {
  mCutoffTime = timestamp;
  mCurLogOffset = 0;
  mFp = 0;
}



bool CommitLogReaderLocal::NextBlock(CommitLogHeaderT **blockp) {
  bool done = false;
  size_t toread;

  while (!done) {

    if (mFp == 0) {

      while (mCurLogOffset < mLogFileInfo.size()) {
	if (mLogFileInfo[mCurLogOffset].timestamp == 0 ||
	    mLogFileInfo[mCurLogOffset].timestamp > mCutoffTime)
	  break;
	mCurLogOffset++;
      }

      if (mCurLogOffset == mLogFileInfo.size())
	return false;

      if ((mFp = fopen(mLogFileInfo[mCurLogOffset].fname.c_str(), "r")) == 0) {
	LOG_VA_ERROR("Problem trying to open commit log file '%s' - %s", mLogFileInfo[mCurLogOffset].fname.c_str(), strerror(errno));
	return false;
      }
    }

    mBlockBuffer.ptr = mBlockBuffer.buf;
    
    if (fread(mBlockBuffer.ptr, 1, sizeof(CommitLogHeaderT), mFp) != sizeof(CommitLogHeaderT)) {
      if (feof(mFp)) {
	fclose(mFp);
	mFp = 0;
	mCurLogOffset++;
      }
      else {
	int error = ferror(mFp);
	LOG_VA_ERROR("Problem trying to read commit log file '%s' - %s", mLogFileInfo[mCurLogOffset].fname.c_str(), strerror(error));
	fclose(mFp);
	mFp = 0;
	return false;
      }
    }
    else {
      CommitLogHeaderT *header = (CommitLogHeaderT *)mBlockBuffer.buf;
      if (header->length == sizeof(CommitLogHeaderT)) {
	if (feof(mFp)) {
	  fclose(mFp);
	  mFp = 0;
	  mCurLogOffset++;
	}
	continue;
      }
      mBlockBuffer.ptr += sizeof(CommitLogHeaderT);
      // TODO: sanity check this header
      toread = header->length-sizeof(CommitLogHeaderT);
      mBlockBuffer.ensure(toread);
      if (fread(mBlockBuffer.ptr, 1, toread, mFp) != toread) {
	LOG_VA_ERROR("Short read of commit log block '%s'", mLogFileInfo[mCurLogOffset].fname.c_str());
	fclose(mFp);
	mFp = 0;
	return false;
      }
      *blockp = (CommitLogHeaderT *)mBlockBuffer.buf;
      return true;
    }
  }

  return false;
  
}