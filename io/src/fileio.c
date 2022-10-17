#include "fileio.h"
#include "bstrlib.h"
#include <stddef.h>

ioStream *NewIoStream(int fd, int fd_t, size_t buf_t) {
  ioStream *out = calloc(1, sizeof(ioStream));
  check(out != NULL, "Could not create IO Stream");

  size_t b_size = buf_t == 0 ? 1024 * 5 : buf_t;

  out->fd = fd;
  out->fd_t = fd_t;

  switch (fd_t) {
#if defined(UNIX) && !defined(WIN32)
  case SOCKFD:
    out->reader = &recv;
    out->writer = &send;
    break;
#elif defined(_WITH_OPEN_SSL)
  case SSLFD:
    out->reader = &SSL_read;
    out->writer = &SSL_write;
    break;
#endif
  default:
    out->reader = _read;
    out->writer = _write;
  break;
  }

  out->in = RingBuffer_New(b_size);
  check(out->in != NULL, "Could not create in stream");

  return out;
error:
  return NULL;
}

ioStream *NewIoStreamFile(bstring path, int flags, int mode, int buf_t) {
  const char *pt = bdata(path);
  int fd = _open(pt, flags, mode);
  check(fd >= 0, "Could not open file %s", bdata(path));
  return NewIoStream(fd, FILEFD, buf_t);
error:
  return NULL;
}

ioStream *NewIoStreamFromFILE(FILE *fp, int buf_t) {
  int fd = _fileno(fp);
  ioStream *stream = NewIoStream(fd, FILEFD, buf_t);
  return stream;
error:
  return NULL;
}

#if defined(UNIX) && !defined(WIN32)
ioStream *NewIoStreamSocket(int inet, int type, int FD, int buf_t) {
  int fd = socket(inet, type, 0);
  check(fd != 0, "Could not open file");
  return NewIoStream(fd, FD, buf_t);
error:
  return NULL;
}

#if defined(UNIX) && !defined(WIN32)
ioStream *NewIoStreamSocketSOC(int inet, int type, int buf_t, void *ssl) {
  ioStream *stream = NewIoStreamSocket(inet, type, SOCKFD, buf_t);
#ifdef _WITH_OPEN_SSL
  if (stream != NULL)
    stream->ssl = ssl;
#endif
  return stream;
}
#endif
#endif

#ifdef ASOC_SSL_H_
ioStream *NewIoStreamSocketSSL(SSL *ssl, int inet, int type, int buf_t) {
  ioStream *str = NewIoStreamSocket(inet, type, SSLFD, buf_t);
  check(str != NULL, "Could not create stream");
  str->ssl = ssl;
  return str;
error:
  return NULL;
}
#endif

void DestroyIoStream(ioStream *io) {
  if (io != NULL) {
    _close(io->fd);
    RingBuffer_Destroy(io->in);
    free(io);
  }
}

int IoStreamIoRead(ioStream *str) {
  int rc = 0;
  if (RingBuffer_Avaliable_Data(str->in) == 0) {
    str->in->start = str->in->end = 0;
  }

#if defined(UNIX)
  if (str->fd_t == SOCKFD) {
    sockReader r = (sockReader)str->reader;
    rc = r(str->fd, RingBuffer_Starts_At(str->in),
           RingBuffer_Avaliable_Space(str->in), 0);
  }
#endif

#ifdef _WITH_OPEN_SSL
  if (str->fd_t == SSLFD) {
    check(str->ssl != NULL, "SSL is null at read");
    sslSockReader r = (sslSockReader)str->reader;
    rc = r(str->ssl, RingBuffer_Starts_At(str->in),
           RingBuffer_Avaliable_Space(str->in));
  }
#endif

  if (str->fd_t == FILEFD) {
    fileReader r = (fileReader)str->reader;
    rc = r(str->fd, RingBuffer_Starts_At(str->in),
           RingBuffer_Avaliable_Space(str->in));
  }

  check(rc != 0, "Failed to read form %s",
        str->fd_t == FILEFD  ? "File"
        : str->fd_t == SSLFD ? "SSLSocket"
                             : "Socket");

  RingBuffer_Commit_Write(str->in, rc);
  return rc;
error:
  return 0;
}

int IoStreamIoWrite(ioStream *str) {
  int rc = 0;

#if defined(UNIX) && !defined(WIN32)
  if (str->fd_t == SOCKFD) {
    sockWriter w = (sockWriter)str->writer;
    rc = w(str->fd, RingBuffer_Starts_At(str->in),
           RingBuffer_Avaliable_Data(str->in), 0);
  }
#endif

#ifdef _WITH_OPEN_SSL
  if (str->fd_t == SSLFD) {
    check(str->ssl != NULL, "SSL is null at write");
    sslSockWriter w = (sslSockWriter)str->writer;
    rc = w(str->ssl, RingBuffer_Starts_At(str->in),
           RingBuffer_Avaliable_Data(str->in));
  }
#endif

  if (str->fd_t == FILEFD) {
    fileWriter w = (fileWriter)str->writer;
    rc = w(str->fd, RingBuffer_Starts_At(str->in),
           RingBuffer_Avaliable_Data(str->in));
  }

  RingBuffer_Commit_Read(str->in, rc);

  check(rc > 0, "Failed to write to %s",
        str->fd_t == FILEFD  ? "File"
        : str->fd_t == SSLFD ? "SSLSocket"
                             : "Socket");

  return rc;
error:
  return -1;
}

bstring IoStreamBuffRead(ioStream *str) {
  bstring data = RingBuffer_Get_All(str->in);
  check(data != NULL, "Failed to get data from buffer");
  return data;
error:
  return NULL;
}

int IoStreamBuffWrite(ioStream *str, bstring input) {
  if (RingBuffer_Avaliable_Data(str->in) == 0) {
    str->in->start = str->in->end = 0;
  }

  int rc = RingBuffer_Write(str->in, bdata(input), blength(input));

  check(rc != 0, "Failed to read form %s",
        str->fd_t == SOCKFD ? "Socket" : "File");
  RingBuffer_Commit_Write(str->in, rc);

  return rc;
error:
  return -1;
}


#if defined(UNIX) && !defined(WIN32)
int IoFileStream_FileExists(bstring file) {
  struct stat st;
  int rc = stat(file->data, &st);
  check(rc >= 0, "File Not Found");
  return rc;
error:
  return -1;
}

int IoFileStream_FileCreate(bstring file, mode_t mode) {
  int fd = open(bdata(file), mode);
  check(fd > 0, "File %s could not be created", bdata(file));
  return fd;
error:
  close(fd);
  return -1;
}

int IoFileStream_DirectoryExists(bstring directory) {
  struct stat st;
  int rc = stat(directory->data, &st);
  check(rc >= 0, "Directory Not Found");
  return 0;
error:
  return -1;
}

int IoFileStream_DirectoryCreate(bstring directory, mode_t mode) {
  int fd = mkdir(bdata(directory), mode);
  check(fd >= 0, "Directory could not be created");
  close(fd);
  return 0;
error:
  close(fd);
  return -1;
}
#endif
