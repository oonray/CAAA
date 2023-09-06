#include "ca_serial.h"
#include <bstring/bstrlib.h>
#include <ca_fileio.h>
#include <stddef.h>
#include <sys/fcntl.h>
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
  case CA_SERIAL:
    out->tty = calloc(1, sizeof(struct termios));
    check(out->tty != NULL, "Could not allocate tty");
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
ca_io_stream *ca_io_stream_new_serial(bstring path, int baud, int buf_t,
                                      int vtime, int vmin) {
  int fd = open(bdata(path), O_RDWR);
  ca_io_stream *stream = ca_io_stream_new(fd, CA_SERIAL, buf_t);
  check(stream != NULL, "Could not create SERIAL Stream");

  // set tty speed
  cfsetispeed(stream->tty, baud == 0 ? CA_B_DFAULT : baud);
  cfsetospeed(stream->tty, baud == 0 ? CA_B_DFAULT : baud);

  stream->tty->c_cflag = (stream->tty->c_cflag & ~CSIZE) | CS8; // 8-bit chars
  stream->tty->c_iflag &= ~IGNBRK; // disable break processing
  stream->tty->c_lflag = 0;        // no signaling chars, no echo,
  stream->tty->c_oflag = 0;        // no remapping, no delays
  stream->tty->c_cc[VMIN] = vmin;  // read doesn't block
  stream->tty->c_cc[VTIME] = 5;    // 0.5 seconds read timeout
  stream->tty->c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl
  stream->tty->c_cflag |= (CLOCAL | CREAD);        // ignore modem controls,
  stream->tty->c_cflag &= ~(PARENB | PARODD);      // shut off parity
  stream->tty->c_cflag |= 0;                       // parity
  stream->tty->c_cflag &= ~CSTOPB;
  stream->tty->c_cflag &= ~CRTSCTS;

  check(tcsetattr(stream->fd, TCSANOW, stream->tty) == 0,
        "Could not save tty config");

  return stream;
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

  switch (str->fd_t) {
  case CA_SOCKFD: {
    ca_sock_reader r = (ca_sock_reader)str->reader;
    rc = r(str->fd, ca_ringbuffer_starts_at(str->in),
           ca_ringbuffer_avaliable_space(str->in), 0);
    break;
  }

#ifdef _CA_IO_WITH_OPEN_SSL
  case CA_SSLFD: {
    check(str->ssl != NULL, "SSL is null at read");
    ca_sock_reader_ssl r = (ca_sock_reader_ssl)str->reader;
    rc = r(str->ssl, ca_ringbuffer_starts_at(str->in),
           ca_ringbuffer_avaliable_space(str->in));
    break;
  }
#endif

  default: {
    io_file_reader r = (io_file_reader)str->reader;
    rc = r(str->fd, ca_ringbuffer_starts_at(str->in),
           ca_ringbuffer_avaliable_space(str->in));
    break;
  }
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

  switch (str->fd_t) {
  case CA_SOCKFD: {
    ca_sock_writer w = (ca_sock_writer)str->writer;
    rc = w(str->fd, ca_ringbuffer_starts_at(str->in),
           ca_ringbuffer_avaliable_data(str->in), 0);
    break;
  }
#ifdef CA_FILEIO_SSL_H_
  case CA_SSLFD: {
    check(str->ssl != NULL, "SSL is null at write");
    ca_sock_writer_ssl w = (ca_sock_writer_ssl)str->writer;
    rc = w(str->ssl, ca_ringbuffer_Starts_At(str->in),
           ca_ringbuffer_Avaliable_Data(str->in));
    break;
  }
#endif

  default: {
    io_file_writer w = (io_file_writer)str->writer;
    rc = w(str->fd, ca_ringbuffer_starts_at(str->in),
           ca_ringbuffer_avaliable_data(str->in));
    break;
  }
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
  int rc = ca_ringbuffer_write(str->in, bdata(input), blength(input));

  check(rc != 0, "Failed to read form %s",
        str->fd_t == CA_SOCKFD ? "Socket" : "File");

  return rc;
error:
  return -1;
}

int io_stream_file_exists(bstring file) {
  struct stat st;
  int rc = stat(bdata(file), &st);
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
int io_stream_directory_create(bstring directory, mode_t mode) {
  int fd = mkdir(bdata(directory), mode);
  check(fd >= 0, "Directory could not be created");
  close(fd);
  return 0;
error:
  close(fd);
  return -1;
}

void ca_io_stream_destroy_pipe(ca_io_stream_pipe *io) {
  if (io->in) {
    ca_io_stream_pipe_close(io, CA_INN);
    ca_io_stream_destroy(io->in);
  }
  if (io->out) {
    ca_io_stream_pipe_close(io, CA_OUT);
    ca_io_stream_destroy(io->out);
  }
  free(io);
}

ca_io_stream_pipe *ca_io_stream_new_pipe(size_t buff_t) {
  ca_io_stream_pipe *out = calloc(1, sizeof(ca_io_stream_pipe));
  check(out != NULL, "could not allocate pipe");

  pipe(out->f);

  out->in = ca_io_stream_new(out->f[0], CA_PIPEFD, buff_t);
  check(out->in != NULL, "could not create in pipe");

  out->out = ca_io_stream_new(out->f[1], CA_PIPEFD, buff_t);
  check(out->out != NULL, "could not create out pipe");

  return out;
error:
  if (out != NULL)
    ca_io_stream_destroy_pipe(out);
  return NULL;
}
int ca_io_stream_pipe_close(ca_io_stream_pipe *str, int io) {
  int rc;
  ca_io_stream *data;
  switch (io) {
  case CA_INN:
    data = str->in;
    break;
  case CA_OUT:
    data = str->out;
    break;
  default:
    log_error("only in and out supported");
    goto error;
  }

  rc = fcntl(str->in->fd, F_GETFD);
  if (rc != -1)
    close(data->fd);
  return 0;
error:
  return -1;
}
int ca_io_stream_pipe_open(ca_io_stream_pipe *str, int io) {
  switch (io) {
  case CA_INN:
    return fcntl(str->in->fd, F_GETFD);
    break;
  case CA_OUT:
    return fcntl(str->out->fd, F_GETFD);
    break;
  default:
    goto error;
  }
error:
  return -1;
}

bstring ca_io_stream_buff_read_pipe(ca_io_stream_pipe *str, int io) {
  switch (io) {
  case CA_INN:
    return ca_io_stream_buff_read(str->in);
    break;
  case CA_OUT:
    return ca_io_stream_buff_read(str->out);
    break;
  default:
    goto error;
  }
error:
  return NULL;
}
int ca_io_stream_buff_write_pipe(ca_io_stream_pipe *str, int io,
                                 bstring input) {
  switch (io) {
  case CA_INN:
    return ca_io_stream_buff_write(str->in, input);
    break;
  case CA_OUT:
    return ca_io_stream_buff_write(str->out, input);
    break;
  default:
    goto error;
  }
error:
  return -1;
}

int ca_io_stream_io_read_pipe(ca_io_stream_pipe *str, int io) {
  switch (io) {
  case CA_INN:
    return ca_io_stream_io_read(str->in);
    break;
  case CA_OUT:
    return ca_io_stream_io_read(str->out);
    break;
  default:
    log_error("only in and out are allowed");
    goto error;
    break;
  }
error:
  return 0;
}
int ca_io_stream_io_write_pipe(ca_io_stream_pipe *str, int io) {
  switch (io) {
  case CA_INN:
    return ca_io_stream_io_write(str->in);
    break;
  case CA_OUT:
    return ca_io_stream_io_write(str->out);
    break;
  default:
    log_error("Only in and out are allowed");
    goto error;
    break;
  }
error:
  return 0;
}
