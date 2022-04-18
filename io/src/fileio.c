#include "fileio.h"
#include <stddef.h>

ioStream *NewIoStream(int fd, int fd_t, size_t buf_t) {
  ioStream *out = calloc(1, sizeof(ioStream));
  check(out != NULL, "Could not create IO Stream");

  size_t b_size = buf_t == 0 ? 1024 * 5 : buf_t;

  out->fd = fd;
  out->fd_t = fd_t;

  switch (fd_t) {
  case SOCKFD:
    out->reader = &recv;
    out->writer = &send;
    break;
#ifdef HEADER_SSL_H
  case SSLFD:
    out->reader = &SSL_read;
    out->writer = &SSL_write;
    break;
#endif
  default:
    out->reader = &read;
    out->writer = &write;
    break;
  }

  out->in = RingBuffer_New(b_size);
  check(out->in != NULL, "Could not create in stream");

  return out;
error:
  return NULL;
}

ioStream *NewIoStreamFile(bstring path, int flags, int buf_t) {
  const char *pt = bdata(path);
  int fd = open(pt, flags);
  check(fd >= 0, "Could not open file %s", bdata(path));
  return NewIoStream(fd, FILEFD, buf_t);
error:
  return NULL;
}

ioStream *NewIoStreamFromFILE(FILE *fp, int buf_t) {
  int fd = fileno(fp);
  ioStream *stream = NewIoStream(fd, FILEFD, buf_t);
  return stream;
error:
  return NULL;
}

ioStream *NewIoStreamSocket(int inet, int type, int FD, int buf_t) {
  int fd = socket(inet, type, 0);
  check(fd != 0, "Could not open file");
  return NewIoStream(fd, FD, buf_t);
error:
  return NULL;
}

ioStream *NewIoStreamSocketSOC(int inet, int type, int buf_t, void *ssl) {
  ioStream *stream = NewIoStreamSocket(inet, type, SOCKFD, buf_t);
  if (stream != NULL)
    stream->ssl = ssl;
  return stream;
}

#ifdef HEADER_SSL_H
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
    close(io->fd);
    RingBuffer_Destroy(io->in);
    free(io);
  }
}

int IoStreamIoRead(ioStream *str) {
  int rc = 0;
  if (RingBuffer_Avaliable_Data(str->in) == 0) {
    str->in->start = str->in->end = 0;
  }

  if (str->fd_t == SOCKFD) {
    sockReader r = (sockReader)str->reader;
    rc = (*r)(str->fd, RingBuffer_Starts_At(str->in),
              RingBuffer_Avaliable_Space(str->in), 0);
  }

#ifdef HEADER_SSL_H
  if (str->fd_t == SSLFD) {
    sslSockReader r = (sslSockReader)str->reader;
    rc = (*r)(str->ssl, RingBuffer_Starts_At(str->in),
              RingBuffer_Avaliable_Space(str->in), 0);
  }
#endif

  if (str->fd_t == FILEFD) {
    fileReader r = (fileReader)str->reader;
    rc = (*r)(str->fd, RingBuffer_Starts_At(str->in),
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
  bstring data = RingBuffer_Get_All(str->in);
  check(data != NULL, "Failed to get data from buffer");
  check(bfindreplace(data, &NL, &CRLF, 0) == BSTR_OK,
        "Failed to replace new lines");

  if (str->fd_t == SOCKFD) {
    sockWriter w = (sockWriter)str->writer;
    rc = (*w)(str->fd, bdata(data), blength(data), 0);
  }

#ifdef HEADER_SSL_H
  if (str->fd_t == SSLFD) {
    sslSockWriter w = (sslSockWriter)str->writer;
    rc = (*w)(str->sslfd, bdata(data), blength(data), 0);
  }
#endif
  if (str->fd_t == FILEFD) {
    fileWriter w = (fileWriter)str->writer;
    rc = (*w)(str->fd, bdata(data), blength(data));
  }

  check(rc == blength(data), "Failed to write to %s",
        str->fd_t == SOCKFD ? "Socket" : "File");
  bdestroy(data);

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
