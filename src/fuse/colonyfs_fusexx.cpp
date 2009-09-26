
#include "../debug.hpp"
#include "colonyfs_fusexx.hpp"

#include <string>
#include <cstring>
#include <iostream>
#include <assert.h>
#include <string.h>

#include <boost/foreach.hpp>




colonyfs_fusexx::colonyfs_fusexx() {

  using namespace colony::xmlrpc;


  const std::string root_path("/");

  std::cout << "Initializing!" << std::endl;

  // Create the attribute associated with the root directory.
  //shared_ptr<attribute_value> pair = make_shared<attribute_value>(root_path);
  shared_ptr<attribute_value> pair = metadata_sink_[root_path];
  fattribute& attribute = pair->get_mapped();

  // What is the access mode?
  attribute.stbuf.st_mode = S_IFDIR | 0777;

  // What is the total number of entries within the directory?
  attribute.stbuf.st_nlink = 1;

  // Time of last modification?
  clock_gettime(CLOCK_REALTIME, &attribute.stbuf.st_mtim);

  // Commit.
  DHT::Instance().set_pair(pair);

  //metadata_sink_.flush();

}




int colonyfs_fusexx::symlink (const char* target, const char* link) {

  using namespace colony::xmlrpc;


  const boost::filesystem::path full(link);
  const boost::filesystem::path branch = full.parent_path();
  const boost::filesystem::path leaf = full.filename();

  rLog(fuse_control_, "symlink: %s -> %s", target, link);

  // Create the attribute what that be associated with the new link.
  //shared_ptr<attribute_value> pair = make_shared<attribute_value>(link);
  shared_ptr<attribute_value> pair = metadata_sink_[link];
  fattribute& attribute = pair->get_mapped();


  // Set the file mode for the new link.
  attribute.stbuf.st_mode = S_IFLNK | 0777;

  // What is the actual target for this link?
  attribute.list.push_back(target);

  // Set the modification time.
  clock_gettime(CLOCK_REALTIME, &attribute.stbuf.st_mtim); // Modification time.



  /* Register the link with its parent directory */

  // Retrieve the attribute associated with link's parent directory.
  shared_ptr<attribute_value> parent_pair = metadata_sink_[ branch.string() ];
  fattribute& parent_attribute = parent_pair->get_mapped();


  // Register the new link with the parent directory.
  parent_attribute.list.push_back(leaf.string());


  return 0;

}




int colonyfs_fusexx::readlink (const char *linkname, char *buffer, size_t size) {

  using namespace colony::xmlrpc;


  // Retrieve the attribute associated with the link.
  shared_ptr<attribute_value> pair = metadata_sink_( linkname );
  fattribute& attribute = pair->get_mapped();


  // Is the file is a symbolic link?
  if (!S_ISLNK(attribute.stbuf.st_mode)) return -EINVAL;

  // Read the target where the link points to.
  const std::string targetpath = attribute.list.front();

  rLog(fuse_control_, "readlink: %s -> %s", linkname, targetpath.c_str());

  // We have to null-terminate the buffer!
  memset(buffer, NULL, size);
  memcpy(buffer, targetpath.c_str(), targetpath.size() + 1);
  // TODO: What if the linkname is too long to fit in the buffer?

  return 0;

}




int colonyfs_fusexx::truncate(const char* filepath, off_t length) {

  using namespace colony::xmlrpc;

  try {

    const boost::filesystem::path full(filepath);

    rLog(fuse_control_, "truncate: %s to %ld", filepath, length);

    // Is length negative?
    if (length < 0) return -EINVAL;

    // Modify the file size information.
    shared_ptr<attribute_value> pair = metadata_sink_[ full.string() ];
    fattribute& attribute = pair->get_mapped();

    data_sink_.truncate(full, length, attribute.stbuf.st_size);

    attribute.stbuf.st_size = length;


    return 0;

  } catch (colony::lookup_e& e) {

    rError("... metadata corruption -- file not found");

    return -ENODEV;

  }


}




int colonyfs_fusexx::rename(const char* oldpath, const char* newpath) {

  // TODO: Input validation checking. See glibc manual.

  using namespace colony::xmlrpc;


  rLog(fuse_control_, "rename: %s -> %s", oldpath, newpath);

  const boost::filesystem::path oldfull(oldpath);
  const boost::filesystem::path oldbranch = oldfull.parent_path();
  const boost::filesystem::path oldleaf = oldfull.filename();

  const boost::filesystem::path newfull(newpath);
  const boost::filesystem::path newbranch = newfull.parent_path();
  const boost::filesystem::path newleaf = newfull.filename();


  // Retrieve the attribute for the old branch path.
  shared_ptr<attribute_value> oldbranch_pair = metadata_sink_[ oldbranch.string() ];
  fattribute& oldbranch_attribute = oldbranch_pair->get_mapped();


  // Find the old leaf.
  metadata_sink_t::mapped_type::list_t::iterator oldbranch_iterator = std::find(
      oldbranch_attribute.list.begin(),
      oldbranch_attribute.list.end(),
      oldleaf.string()
      );

  // And erase it.
  oldbranch_attribute.list.erase(oldbranch_iterator);


  // Retrieve the attribute for the new branch path.
  shared_ptr<attribute_value> newbranch_pair = metadata_sink_[ newbranch.string() ];
  fattribute& newbranch_attribute = newbranch_pair->get_mapped();

  newbranch_attribute.list.push_back(newleaf.string());


  // Retrieve the attribute for the new path.
  shared_ptr<attribute_value> oldfull_pair =
      metadata_sink_( oldfull.string() );

  // Associate the old attribute with the new path.
  shared_ptr<attribute_value> newfull_pair = metadata_sink_[newfull.string()];
  newfull_pair->get_mapped() = oldfull_pair->get_mapped();


  metadata_sink_.erase( oldfull_pair );

  // FIXME: Implement data rename.
  data_sink_.rename(oldfull, newfull, newfull_pair->get_mapped().stbuf.st_size);

  return 0;

}




int colonyfs_fusexx::opendir (const char* filepath, struct fuse_file_info* fi) {
  // TODO: Implement this.
  rLog(fuse_control_, "opendir %s", filepath);

  return 0;
}




int colonyfs_fusexx::readdir(
    const char* filepath, void* buffer,
    fuse_fill_dir_t filler, off_t offset,
    struct fuse_file_info* fi) {

  using namespace colony::xmlrpc;


  rLog(fuse_control_, "readdir: %s", filepath);

  boost::filesystem::path full(filepath);

  try {

    // Retrieve the attribute for the directory path.
    shared_ptr<attribute_value> pair = metadata_sink_( filepath );
    fattribute& attribute = pair->get_mapped();


    // Make sure we are dealing with the directory.
    if (!S_ISDIR(attribute.stbuf.st_mode)) {
      rError("... not a directory");
      return -ENOTDIR;
    }

    // Fill the buffer with the directory content.
    filler(buffer, ".", NULL, 0);
    filler(buffer, "..", NULL, 0);

    // And all other files.
    BOOST_FOREACH(const std::string& value, attribute.list) {

      rLog(fuse_control_, "*\t%s", value.c_str());

      // Pass the stat information as well!
      filler(buffer, value.c_str(),  NULL, 0);
    }

    return 0;

  } catch(colony::lookup_e& e) {

    rError("... directory not found");

    return -ENOENT;
  }

}




int colonyfs_fusexx::mkdir(const char* filepath, mode_t mode) {

  using namespace colony::xmlrpc;


  const boost::filesystem::path full(filepath);
  const boost::filesystem::path branch = full.parent_path();
  const boost::filesystem::path leaf = full.filename();

  if (!validate_path(leaf)) return -ENAMETOOLONG;


  // TODO: Check if filename already exists.
  // Create the attribute for the new directory.
  shared_ptr<attribute_value> pair = metadata_sink_[ full.string() ];
  fattribute& metadata = pair->get_mapped();


  memset(&metadata.stbuf, 0, sizeof(struct stat));
  metadata.stbuf.st_mode = S_IFDIR | mode;
  metadata.stbuf.st_nlink = 1;
  metadata.stbuf.st_uid = getuid();
  metadata.stbuf.st_gid = getgid();
  clock_gettime(CLOCK_REALTIME, &metadata.stbuf.st_mtim);



  rLog(fuse_control_, "mkdir: %s", full.string().c_str());


  // Retrieve the attribute associated with directory's parent.
  shared_ptr<attribute_value> parent_pair = metadata_sink_[ branch.string() ];
  fattribute& parent_attribute = parent_pair->get_mapped();


  parent_attribute.list.push_back(leaf.string());


  return 0;

}




int colonyfs_fusexx::access(const char* filepath, int mode) {
  // TODO: Implement this.
  rLog(fuse_control_, "access: %s", filepath);

  return 0;

}




int colonyfs_fusexx::open(const char *filepath, struct fuse_file_info *fi) {
  // TODO: Need to check if the operation is permitted for the given flags.
  rLog(fuse_control_, "open: %s", filepath);

  return 0;
}




int colonyfs_fusexx::read(
    const char *filepath,
    char *buffer,
    size_t size,
    off_t offset,
    struct fuse_file_info *fi) {

  using namespace colony::xmlrpc;


  const boost::filesystem::path full(filepath);

  rLog(fuse_control_, "read: %s", full.string().c_str());


  // Retrieve the attribute for the file path.
  shared_ptr<attribute_value> pair = metadata_sink_( full.string() );
  fattribute& attribute = pair->get_mapped();


  // TODO: Figure out size_t/off_t inconsistency.
  size_t length = attribute.stbuf.st_size;

  if (static_cast<size_t>(offset) > length) {

    size = 0;

  } else {

    if (static_cast<size_t>(offset) + size > length)
      size = length - offset;

    data_sink_.read(full, buffer, size, offset);

  }

  return size;


}




int colonyfs_fusexx::getattr(const char *filepath, struct stat *stat) {

  using namespace colony::xmlrpc;


  const boost::filesystem::path full(filepath);
  const boost::filesystem::path branch = full.parent_path();
  const boost::filesystem::path leaf = full.filename();

  if (!validate_path(leaf)) return -ENAMETOOLONG;

  try {

    // Retrieve the attribute for the file path.
    shared_ptr<attribute_value> pair = metadata_sink_( full.string() );
    const fattribute& attribute = pair->get_mapped();


    // Double check that the returned attribute is valid.
    BOOST_ASSERT(
        (attribute.stbuf.st_nlink != 0) ||
        S_ISLNK(attribute.stbuf.st_mode)
        );


    memset(stat, 0, sizeof(struct stat));
    memcpy(stat, &attribute.stbuf, sizeof(struct stat));

    rLog(fuse_control_, "getattr: (+) %s", full.string().c_str());

    return 0;

  } catch (colony::lookup_e& e) {

    rLog(fuse_control_, "getattr: (-) %s", full.string().c_str());

    return -ENOENT;

  }

}



int colonyfs_fusexx::fgetattr (
    const char* filename,
    struct stat* stbuf,
    struct fuse_file_info* fi) {

  return getattr(filename, stbuf);

}




int colonyfs_fusexx::utime(const char* filepath,  struct utimbuf* time) {

  using namespace colony::xmlrpc;


  const boost::filesystem::path full(filepath);

  rLog(fuse_control_, "utime: %s", full.string().c_str());


  // Update the time.
  shared_ptr<attribute_value> pair = metadata_sink_[ full.string() ];
  fattribute& attribute = pair->get_mapped();


  attribute.stbuf.st_atim.tv_sec = time->actime;
  attribute.stbuf.st_mtim.tv_sec = time->modtime;


  return 0;

}




int colonyfs_fusexx::create(
    const char* filepath,
    mode_t mode,
    struct fuse_file_info* fi) {

  using namespace colony::xmlrpc;

  const boost::filesystem::path full(filepath);
  const boost::filesystem::path branch = full.parent_path();
  const boost::filesystem::path leaf = full.filename();

  if (!validate_path(leaf)) return -ENAMETOOLONG;


  rLog(fuse_control_, "create: %s", full.string().c_str());


  // Create the attribute for the new file.
  shared_ptr<attribute_value> pair = metadata_sink_[ full.string() ];
  fattribute& attribute = pair->get_mapped();


  // Set the attribute.
  attribute.stbuf.st_mode = S_IFREG | mode; // File flags.
  attribute.stbuf.st_nlink = 1;
  attribute.stbuf.st_uid = getuid();
  attribute.stbuf.st_gid = getgid();
  attribute.stbuf.st_blksize = CHUNK_SIZE;
  clock_gettime(CLOCK_REALTIME, &attribute.stbuf.st_mtim); // Modification time.


  // Register the new file with its parent.
  shared_ptr<attribute_value> parent_pair = metadata_sink_[ branch.string() ];
  fattribute& parent_attribute = parent_pair->get_mapped();

  parent_attribute.list.push_back(leaf.string());


  // Initialize the data structure.
  data_sink_.write(full, 0, 0, 0);


  return 0;

}




int colonyfs_fusexx::write (
    const char* filepath,
    const char* buffer,
    size_t size,
    off_t offset,
    struct fuse_file_info* fi) {

  using namespace colony::xmlrpc;


  rLog(fuse_control_, "write: (%lu) %s", size, filepath);

  boost::filesystem::path full(filepath);

  data_sink_.write(full, buffer, size, offset);

  // Update the attribute.
  shared_ptr<attribute_value> pair = metadata_sink_[ filepath ];
  fattribute& metadata = pair->get_mapped();

  const int end_pointer = offset + size;
  if (metadata.stbuf.st_size < end_pointer) metadata.stbuf.st_size = end_pointer;


  return size;

}




int colonyfs_fusexx::unlink(const char* filepath) {


  using namespace colony::xmlrpc;


  rLog(fuse_control_, "unlink: %s", filepath);

  const boost::filesystem::path full(filepath);
  const boost::filesystem::path branch = full.parent_path();
  const boost::filesystem::path leaf = full.filename();

  if (!validate_path(leaf)) return -ENAMETOOLONG;


  // Remove the file from the parent.
  shared_ptr<attribute_value> pair = metadata_sink_[ branch.string() ];
  fattribute& attribute = pair->get_mapped();


  if ( attribute.stbuf.st_uid != getuid() && getuid() != 0 ) return -EACCES;

  fattribute::list_t& content = attribute.list;
  content.remove(leaf.string());


  // Delete the binary data.
  data_sink_.erase(full.string(), attribute.stbuf.st_size);

  // Delete attribute.
  metadata_sink_.erase(full.string());


  return 0;

}




int colonyfs_fusexx::chmod(const char* filename, mode_t mode) {

  using namespace colony::xmlrpc;


  rLog(fuse_control_, "chmod: %s", filename);

  int status;

  // Check User ID.
  shared_ptr<attribute_value> pair = metadata_sink_[ filename ];
  fattribute& attribute = pair->get_mapped();


  if ( attribute.stbuf.st_uid == getuid() || getuid() == 0 ) {
    attribute.stbuf.st_mode = mode;
    status = 0;
  } else {
    status = -EACCES;
  }


  return status;

}


int colonyfs_fusexx::chown(const char* filepath, uid_t uid, gid_t gid) {

  using namespace colony::xmlrpc;


  const boost::filesystem::path full(filepath);

  rLog(fuse_control_, "chown: %s | UID: %ud GID: %ud", filepath, uid, gid);


  // Change user ID and group ID.
  shared_ptr<attribute_value> pair = metadata_sink_[ full.string() ];
  fattribute& metadata = pair->get_mapped();


  metadata.stbuf.st_uid = uid;
  metadata.stbuf.st_gid = gid;


  return 0;

}



int colonyfs_fusexx::rmdir(const char* filepath) {

  using namespace colony::xmlrpc;

  rLog(fuse_control_, "rmdir: %s", filepath);

  const boost::filesystem::path full(filepath);
  const boost::filesystem::path branch = full.parent_path();
  const boost::filesystem::path leaf = full.filename();


  // Remove metadata for the given file.
  metadata_sink_.erase(full.string().c_str());

  try {

    // Get the directory content of the parent.
    shared_ptr<attribute_value> parent_pair = metadata_sink_[ branch.string() ];
    fattribute& parent_metadata = parent_pair->get_mapped();


    // Examine the durectory content and remove the entry.
    parent_metadata.list.remove(leaf.string());

    return 0;

  } catch (colony::lookup_e& e) {

    rError("... metadata corruption -- parent directory not found");

    return -ENODEV;

  }

}


bool colonyfs_fusexx::validate_path(boost::filesystem::path path) {

  const size_t max_size = 255;
  bool result = true;

  if (path.string().length() > max_size) {
    result = false;
    std::cout << path.string().length() << "\t\tTOO LONG\n";
  }


  return result;

}

int colonyfs_fusexx::flush(const char* filepath, struct fuse_file_info * fi) {
  data_sink_.flush();
  metadata_sink_.flush();

  return 0;
}



int colonyfs_fusexx::lock (
    const char* filepath,
    struct fuse_file_info* fi,
    int cmd,
    struct flock* fl) {

  rError("Lock called: File: %s: CMD: %d", filepath, cmd);

  return 0;
}




metadata_sink_t colonyfs_fusexx::metadata_sink_;
colony::aggregator<colony::storage::chunk_data>  colonyfs_fusexx::data_sink_;
rlog::RLogChannel* colonyfs_fusexx::fuse_control_( RLOG_CHANNEL( "fuse/control" ) );
