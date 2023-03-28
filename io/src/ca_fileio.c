#include <bstring/bstrlib.h>
#include <ca_fileio.h>
#include <stddef.h>
#include <sys/stat.h>

ca_io_stream *ca_io_stream_new(int fd, int fd_t, size_t buf_t) {
  ca_io_stream *out = calloc(1, sizeof(ca_io_stream));
  check(out != NULL, "Could not create IO Stream");

  size_t b_size = buf_t == 0 ? 1024 * 5 : buf_t;

  out->fd = fd;
  out->fd_t = fd_t;

  switch (fd_t) {
  case CA_SOCKFD:
    out->reader = &recv;
    out->writer = &send;
    break;
#ifdef _CA_IO_WITH_OPEN_SSL
  case CA_SSLFD:
    out->reader = &SSL_read;
    out->writer = &SSL_write;
    break;
#endif
  default:
    out->reader = &read;
    out->writer = &write;
    break;
  }

  out->in = ca_ringbuffer_new(b_size);
  check(out->in != NULL, "Could not create in stream");

  return out;
error:
  return NULL;
}

ca_io_stream *ca_io_stream_new_file(bstring path, int flags, int buf_t) {
  const char *pt = bdata(path);
  struct stat sb;
  int fd = 0;

  int st = stat(pt, &sb);
  if (st == 0) {
    fd = open(pt, flags);
  } else {
    fd = creat(pt, flags);
  }
  check(fd >= 0, "Could not open file %s", bdata(path));

  return ca_io_stream_new(fd, CA_FILEFD, buf_t);
error:
  return NULL;
}

ca_io_stream *ca_io_stream_new_from_file(FILE *fp, int buf_t) {
  int fd = fileno(fp);
  ca_io_stream *stream = ca_io_stream_new(fd, CA_FILEFD, buf_t);
  return stream;
error:
  return NULL;
}

ca_io_stream *ca_io_stream_new_socket(int inet, int type, int buf_t) {
  int s_fd = socket(inet, type, 0);
  check(s_fd != 0, "Could not open file");
  return ca_io_stream_new(s_fd, CA_SOCKFD, buf_t);
error:
  return NULL;
}

void ca_io_stream_destroy(ca_io_stream *io) {
  if (io != NULL) {
    close(io->fd);
    ca_ringbuffer_destroy(io->in);
    free(io);
  }
}

int ca_io_stream_io_read(ca_io_stream *str) {
  int rc = 0;
  if (ca_ringbuffer_avaliable_data(str->in) == 0) {
    str->in->start = str->in->end = 0;
  }

  if (str->fd_t == CA_SOCKFD) {
    ca_sock_reader r = (ca_sock_reader)str->reader;
    rc = r(str->fd, ca_ringbuffer_starts_at(str->in),
           ca_ringbuffer_avaliable_space(str->in), 0);
  }

#ifdef _CA_IO_WITH_OPEN_SSL
  if (str->fd_t == CA_SSLFD) {
    check(str->ssl != NULL, "SSL is null at read");
    ca_sock_reader_ssl r = (ca_sock_reader_ssl)str->reader;
    rc = r(str->ssl, ca_ringbuffer_starts_at(str->in),
           ca_ringbuffer_avaliable_space(str->in));
  }
#endif

  if (str->fd_t == CA_FILEFD) {
    io_file_reader r = (io_file_reader)str->reader;
    rc = r(str->fd, ca_ringbuffer_starts_at(str->in),
           ca_ringbuffer_avaliable_space(str->in));
  }

  check(rc != 0, "Failed to read form %s",
        str->fd_t == CA_FILEFD  ? "File"
        : str->fd_t == CA_SSLFD ? "SSLSocket"
                                : "Socket");

  ca_ringbuffer_commit_write(str->in, rc);
  return rc;
error:
  return 0;
}

int ca_io_stream_io_write(ca_io_stream *str) {
  int rc = 0;

  if (str->fd_t == CA_SOCKFD) {
    ca_sock_writer w = (ca_sock_writer)str->writer;
    rc = w(str->fd, ca_ringbuffer_starts_at(str->in),
           ca_ringbuffer_avaliable_data(str->in), 0);
  }

#ifdef CA_FILEIO_SSL_H_
  if (str->fd_t == CA_SSLFD) {
    check(str->ssl != NULL, "SSL is null at write");
    ca_sock_writer_ssl w = (ca_sock_writer_ssl)str->writer;
    rc = w(str->ssl, ca_ringbuffer_Starts_At(str->in),
           ca_ringbuffer_Avaliable_Data(str->in));
  }
#endif

  if (str->fd_t == CA_FILEFD) {
    io_file_writer w = (io_file_writer)str->writer;
    rc = w(str->fd, ca_ringbuffer_starts_at(str->in),
           ca_ringbuffer_avaliable_data(str->in));
  }

  ca_ringbuffer_commit_read(str->in, rc);

  check(rc > 0, "Failed to write to %s",
        str->fd_t == CA_FILEFD  ? "File"
        : str->fd_t == CA_SSLFD ? "SSLSocket"
                                : "Socket");

  return rc;
error:
  return -1;
}

bstring ca_io_stream_buff_read(ca_io_stream *str) {
  bstring data = ca_ringbuffer_get_all(str->in);
  check(data != NULL, "Failed to get data from buffer");
  return data;
error:
  return NULL;
}

int ca_io_stream_buff_write(ca_io_stream *str, bstring input) {
  if (ca_ringbuffer_avaliable_data(str->in) == 0) {
    str->in->start = str->in->end = 0;
  }

  int rc = ca_ringbuffer_write(str->in, bdata(input), blength(input));

  check(rc != 0, "Failed to read form %s",
        str->fd_t == CA_SOCKFD ? "Socket" : "File");

  ca_ringbuffer_commit_write(str->in, rc);

  return rc;
error:
  return -1;
}

int io_stream_file_exists(bstring file) {
  struct stat st;
  int rc = stat(file->data, &st);
  check(rc >= 0, "File Not Found");
  return rc;
error:
  return -1;
}

int ca_io_stream_file_create(bstring file, mode_t mode) {
  int fd = open(bdata(file), mode);
  check(fd > 0, "File %s could not be created", bdata(file));
  return fd;
error:
  close(fd);
  return -1;
}

int ca_io_stream_directory_exists(bstring directory) {
  struct stat st;
  int rc = stat(bdata(directory), &st);
  check(rc >= 0, "Directory Not Found");
  return 0;
error:
  return -1;
}

int io_stream_directorycreate(bstring directory, mode_t mode) {
  int fd = mkdir(bdata(directory), mode);
  check(fd >= 0, "Directory could not be created");
  close(fd);
  return 0;
error:
  close(fd);
  return -1;
}
