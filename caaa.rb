# Documentation: https://docs.brew.sh/Formula-Cookbook
#                https://rubydoc.brew.sh/Formula
# PLEASE REMOVE ALL GENERATED COMMENTS BEFORE SUBMITTING YOUR PULL REQUEST!
class Caaa < Formula
  desc "c with aaa batteres"
  homepage ""
  url "https://github.com/oonray/CAAA/archive/refs/tags/v1.1.20.tar.gz"
  license "MIT"

  depends_on "cmake" => :build
  depends_on "openssl" => :build

  def install
    # ENV.deparallelize  # if your formula fails when building in parallel
    # Remove unrecognized options if warned by configure
    # https://rubydoc.brew.sh/Formula.html#std_configure_args-instance_method
    #system "./configure", *std_configure_args, "--disable-silent-rules"
     system "cmake", "-S", ".", "-B", "build", *std_cmake_args
     system "make", "-C", "build"
     system "make", "-C", "build", "install"
  end

  test do
    system "make", -C, "build", "test"
  end
end
