// hello.hpp
#include "fusexx.hpp"
#include "../synchronization.hpp"
#include "../api/aggregator.hpp"
#include "../xmlrpc/attribute.hpp"
#include "../xmlrpc/values.hpp"
#include "../debug.hpp"

#include <string>
#include <boost/filesystem/path.hpp>
#include <boost/unordered_set.hpp>
#include <boost/unordered_map.hpp>


#include <tbb/task.h>
#include <tbb/atomic.h>
#include <tbb/task_scheduler_init.h>


typedef colony::aggregator<colony::xmlrpc::attribute_value>  metadata_sink_t;
typedef colony::aggregator<colony::storage::chunk_data>      data_sink_t;



typedef tbb::atomic<int> counter_type;



class dht_task : public tbb::task {


public:


  dht_task(
      std::string filename,
      counter_type& counter,
      metadata_sink_t& sink,
      data_sink_t& data_sink) :
    filename_(filename),
    counter_(counter),
    sink_(sink),
    data_sink_(data_sink){}

  tbb::task* execute() {

    sink_.flush(filename_);

    counter_.fetch_and_increment();

    Sync::FM()[filename_].unlock();

    return NULL;
  }





  std::string       filename_;
  counter_type&     counter_;
  metadata_sink_t&  sink_;
  data_sink_t&      data_sink_;

};




class colonyfs_fusexx: public fusexx::fuse<colonyfs_fusexx> {
public:


  colonyfs_fusexx(); // Constructor




  /**
   * Check file access permissions
   *
   * This will be called for the access() system call.  If the
   * 'default_permissions' mount option is given, this method is not
   * called.
   *
   * This method is not called under Linux kernel versions 2.4.x
   *
   * Introduced in version 2.5
   *
   * The access function checks to see whether the file named by filename
   * can be accessed in the way specified by the how argument.  The how
   * argument either can be the bitwise OR of the flags R_OK, W_OK, X_OK,
   * or the existence test F_OK.
   *
   * This function uses the real user and group IDs of the calling
   * process, rather than the effective IDs, to check for access
   * permission. As a result, if you use the function from a setuid or
   * setgid program (see How Change Persona), it gives information
   * relative to the user who actually ran the program.
   *
   * The return value is 0 if the access is permitted, and -1 otherwise.
   *(In other words, treated as a predicate function, access returns true
   * if the requested access is denied.)
   *
   * In addition to the usual file name errors (see File Name Errors), the
   * following errno error conditions are defined for this function:
   *
   *    EACCES
   *        The access specified by how is denied.
   *    ENOENT
   *        The file doesn't exist.
   *    EROFS
   *        Write permission was requested for a file on a read-only file system.
   *
   */
  static int access(const char* filename, int mode);




  /** Get file attributes.
   *
   * Similar to stat().  The 'st_dev' and 'st_blksize' fields are
   * ignored.  The 'st_ino' field is ignored except if the 'use_ino'
   * mount option is given.
   */
  static int getattr(const char* filename, struct stat* stat);




  /** Change the access and/or modification times of a file
   *
   * Deprecated, use utimens() instead.
   */
  static int utime(const char* filename, struct utimbuf* time);

  /**
   * The normal return value from symlink is 0. A return value of -1
   * indicates an error. In addition to the usual file name syntax
   * errors (see File Name Errors), the following errno error conditions
   * are defined for this function:
   *
   * EEXIST
   *  There is already an existing file named newname.
   * EROFS
   *  The file newname would exist on a read-only file system.
   * ENOSPC
   *  The directory or file system cannot be extended to make the new link.
   * EIO
   *  A hardware error occurred while reading or writing data on the disk.
   */
  static int symlink(const char* src, const char* dst);




  static int rename(const char* oldname, const char* newname);




  /**
   * Read the target of a symbolic link
   *
   * The buffer should be filled with a null terminated string.  The
   * buffer size argument includes the space for the terminating
   * null character.  If the linkname is too long to fit in the
   * buffer, it should be truncated.  The return value should be 0
   * for success.
   */
  static int readlink (const char *filename, char *buffer, size_t size);




  /** File open operation
   *
   * No creation (O_CREAT, O_EXCL) and by default also no
   * truncation (O_TRUNC) flags will be passed to open(). If an
   * application specifies O_TRUNC, fuse first calls truncate()
   * and then open(). Only if 'atomic_o_trunc' has been
   * specified and kernel version is 2.6.24 or later, O_TRUNC is
   * passed on to open.
   *
   * Unless the 'default_permissions' mount option is given,
   * open should check if the operation is permitted for the
   * given flags. Optionally open may also return an arbitrary
   * filehandle in the fuse_file_info structure, which will be
   * passed to all file operations.
   *
   * Changed in version 2.2
   */
  static int open(
      const char* filename,
      struct fuse_file_info* fi);




  /** Read data from an open file
   *
   * Read should return exactly the number of bytes requested except
   * on EOF or error, otherwise the rest of the data will be
   * substituted with zeroes.  An exception to this is when the
   * 'direct_io' mount option is specified, in which case the return
   * value of the read system call will reflect the return value of
   * this operation.
   *
   * Changed in version 2.2
   */
  static int read(
      const char* filename,
      char* buf,
      size_t size,
      off_t offset,
      struct fuse_file_info* fi);




  /**
   * Get file system statistics
   *
   * The 'f_frsize', 'f_favail', 'f_fsid' and 'f_flag' fields are ignored
   *
   * Replaced 'struct statfs' parameter with 'struct statvfs' in
   * version 2.5
   */
  static int write(
      const char* filename,
      const char* buf,
      size_t size,
      off_t offset,
      struct fuse_file_info* fi);




  /** Remove a file */
  static int unlink(
      const char* filename);




  /** Change the size of a file */
  static int truncate(const char* filename, off_t offset);




  /** Open directory
   *
   * This method should check if the open operation is permitted for
   * this  directory
   *
   * Introduced in version 2.3
   */
  static int opendir (
      const char* filename,
      struct fuse_file_info* fi);




  /** Read directory
   *
   * This supersedes the old getdir() interface.  New applications
   * should use this.
   *
   * The filesystem may choose between two modes of operation:
   *
   * 1) The readdir implementation ignores the offset parameter, and
   * passes zero to the filler function's offset.  The filler
   * function will not return '1' (unless an error happens), so the
   * whole directory is read in a single readdir operation.  This
   * works just like the old getdir() method.
   *
   * 2) The readdir implementation keeps track of the offsets of the
   * directory entries.  It uses the offset parameter and always
   * passes non-zero offset to the filler function.  When the buffer
   * is full (or an error happens) the filler function will return
   * '1'.
   *
   * Introduced in version 2.3
   */
  static int readdir(
      const char* filename,
      void* buf,
      fuse_fill_dir_t filler,
      off_t offset,
      struct fuse_file_info* fi);




  /** Create a directory
   *
   * Note that the mode argument may not have the type specification
   * bits set, i.e. S_ISDIR(mode) can be false.  To obtain the
   * correct directory type bits use  mode|S_IFDIR
   * */
  static int mkdir(
      const char* filename,
      mode_t mode);




  /** Remove a directory */
  static int rmdir(
      const char* filename);




  /**
   * Get attributes from an open file
   *
   * This method is called instead of the getattr() method if the
   * file information is available.
   *
   * Currently this is only called after the create() method if that
   * is implemented (see above).  Later it may be called for
   * invocations of fstat() too.
   *
   * Introduced in version 2.5
   */
  static int fgetattr(
      const char* filename,
      struct stat* stat,
      struct fuse_file_info* fi);




  /**
   * Create and open a file
   *
   * If the file does not exist, first create it with the specified
   * mode, and then open it.
   *
   * If this method is not implemented or under Linux kernel
   * versions earlier than 2.6.15, the mknod() and open() methods
   * will be called instead.
   *
   * Introduced in version 2.5
   */
  static int create(
      const char* filename,
      mode_t mode,
      struct fuse_file_info* fi);




  /** Change the permission bits of a file */
  static int chmod(
      const char* filename, mode_t mode);




  static int chown(const char* filepath, uid_t uid, gid_t gid);




  static int flush(const char* filepath, struct fuse_file_info * fi);


  static int lock(
      const char* filepath,
      struct fuse_file_info* fi,
      int cmd,
      struct flock* fl);


  // TODO: Implement sfatfs().


  static int release(const char* filepath, struct fuse_file_info* fi);





  static bool validate_path(boost::filesystem::path path);

  static metadata_sink_t                                  metadata_sink_;
  static colony::aggregator<colony::storage::chunk_data>  data_sink_;
  static rlog::RLogChannel                               *fuse_control_;
};
