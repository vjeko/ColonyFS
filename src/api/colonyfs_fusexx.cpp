// hello.cpp
#include "colonyfs_fusexx.hpp"

#include <string>
#include <cstring>
#include <iostream>
#include <assert.h>

#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>




metadata_map_t metadata_map_;
data_map_t data_map_;




colonyfs_fusexx::colonyfs_fusexx() {

  fattribute metadata;

  metadata.stbuf.st_mode = S_IFDIR | 0777;
  metadata.stbuf.st_nlink = 1;

  clock_gettime(CLOCK_REALTIME, &metadata.stbuf.st_mtim);
  metadata_map_.commit("/", metadata);
}




int colonyfs_fusexx::symlink (const char* target, const char* link) {

  rLog(fuse_control_, "symlink: %s -> %s", target, link);

  // Create metadata for that filename.
  fattribute metadata;

  // Set the attributes.
  metadata.stbuf.st_mode = S_IFLNK | 0777; // File flags.
  clock_gettime(CLOCK_REALTIME, &metadata.stbuf.st_mtim); // Modification time.
  metadata.list.push_back(target); // Target..

//  metadata_map_.commit(link, metadata);
  metadata_map_.commit(link, metadata);

  boost::filesystem::path full(link);
  boost::filesystem::path branch = full.branch_path();
  boost::filesystem::path leaf = full.leaf();

  // Register file creating with the parent.
//  fattribute& pmetadata = metadata_map_[ branch.string() ];
//  pmetadata.list.push_back(leaf.string());
  fattribute parent_metadata = metadata_map_[ branch.string() ];
  parent_metadata.list.push_back(leaf.string());
  metadata_map_.commit(branch.string(), parent_metadata);

  return 0;
}




int colonyfs_fusexx::readlink (const char *linkname, char *buffer, size_t size) {

  fattribute metadata = metadata_map_[ linkname ];
  std::string targetname = metadata.list.front();

  rLog(fuse_control_, "readlink: %s -> %s", linkname, targetname.c_str());

  // We have to null-terminate the buffer!
  memcpy(buffer, targetname.c_str(), targetname.size() + 1);

  return 0;
}




int colonyfs_fusexx::truncate(const char* filename, off_t offset) {

  rLog(fuse_control_, "truncate: %s", filename);

  if (offset < 0)
    return EINVAL;

  std::string& data = data_map_[ filename ];
  data.resize(offset);

  return 0;
}




int colonyfs_fusexx::rename(const char* oldname, const char* newname) {

  rLog(fuse_control_, "rename: %s -> %s", oldname, newname);

  boost::filesystem::path ofull(oldname);
  boost::filesystem::path obranch = ofull.branch_path();
  boost::filesystem::path oleaf = ofull.leaf();

  boost::filesystem::path nfull(newname);
  boost::filesystem::path nbranch = nfull.branch_path();
  boost::filesystem::path nleaf = nfull.leaf();

  fattribute& pometadata = metadata_map_[ obranch.string() ];
  metadata_map_t::mapped_type::list_t& polist = pometadata.list;

  metadata_map_t::mapped_type::list_t::iterator poiterator = std::find(
      polist.begin(), polist.end(), oleaf.string());


  polist.erase(poiterator);

  /*
   * In the case that a new filename is not in the same directory,
   * add it to a new directory (parent).
   */
  if (obranch != nbranch) {

    fattribute& pnmetadata = metadata_map_[ nbranch.string() ];
    pnmetadata.list.push_back(nleaf.string());
    metadata_map_.commit(nbranch.string(), pnmetadata);

  } else {

    polist.push_back(nleaf.string());

  }

  fattribute& ometadata = metadata_map_[ ofull.string() ];;
  metadata_map_.commit( nfull.string(), ometadata );
  metadata_map_.erase(ofull.string());

  std::string odata = data_map_[ ofull.string() ];
  data_map_.commit( nfull.string(), odata);
  data_map_.erase(ofull.string());

  return 0;
}




/*
 * NOTE:
 * access() is only  appropriate to use in setuid programs.
 * A non-setuid program will always use the effective ID rather than the real ID.
 */
int colonyfs_fusexx::access(const char* filename, int mode) {
  rLog(fuse_control_, "access: %s", filename);
  return 0;
}




int colonyfs_fusexx::open(const char *filename, struct fuse_file_info *fi) {
  rLog(fuse_control_, "open: %s", filename);
  return 0;
}




int colonyfs_fusexx::readdir(
    const char* filename, void* buffer,
    fuse_fill_dir_t filler, off_t offset,
    struct fuse_file_info* fi) {

  rLog(fuse_control_, "readdir: %s", filename);

  boost::filesystem::path full(filename);
  boost::filesystem::path branch;

  if(full.string().compare("/") == 0)
    branch = boost::filesystem::path("/");
  else
    branch = full.branch_path();

  full.parent_path();

  try {
    // Retrieve the metadata for our file.
    fattribute& metadata = metadata_map_[filename];

    // Fill the buffer with the directory content.
    // Standard dot dot dot.
    filler(buffer, ".", NULL, 0);
    filler(buffer, "..", NULL, 0);

    // And all other files.
    BOOST_FOREACH(std::string& value, metadata.list) {

      //TODO: root directory branch path problem
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




int colonyfs_fusexx::mkdir(const char* filename, mode_t mode) {

  boost::filesystem::path path(filename);
  boost::filesystem::path branch = path.branch_path();
  boost::filesystem::path leaf = path.leaf();

  fattribute metadata;

  memset(&metadata.stbuf, 0, sizeof(struct stat));
  metadata.stbuf.st_mode = S_IFDIR | mode;
  metadata.stbuf.st_nlink = 1;
  metadata.stbuf.st_uid = getuid();
  metadata.stbuf.st_gid = getgid();
  clock_gettime(CLOCK_REALTIME, &metadata.stbuf.st_mtim);

  metadata_map_.commit(filename, metadata);

  rLog(fuse_control_, "mkdir: %s", filename);

  fattribute& pmetadata = metadata_map_[ branch.string() ];
  pmetadata.list.push_back(leaf.string());
  metadata_map_.commit(branch.string(), pmetadata);

  return 0;
}

int colonyfs_fusexx::read(
    const char *filename, char *buffer,
    size_t size, off_t offset, struct fuse_file_info *fi) {

  rLog(fuse_control_, "read: %s", filename);

  (void) fi;

  fattribute& metadata = metadata_map_[ filename ];
  std::string& data = data_map_[ filename ];

  size_t length = metadata.stbuf.st_size;

  if (static_cast<size_t>(offset) < length) {

    if (static_cast<size_t>(offset) + size > length)
      size = length - offset;

    memcpy(buffer, data.c_str() + offset, size);

  } else
    size = 0;

  return size;
}


int colonyfs_fusexx::getattr(const char *filename, struct stat *stat) {

  try {

    const metadata_map_t::mapped_type& metadata =
        metadata_map_[filename]; // Retrieve metadata for our file.

    memset(stat, 0, sizeof(struct stat)); // Zero out the file stat buffer
    memcpy(stat, &metadata.stbuf, sizeof(struct stat)); // Copy metadata info to a given buffer.

    rLog(fuse_control_, "getattr: (+) %s", filename);

    return 0;

  } catch (colony::lookup_e& e) {
    rLog(fuse_control_, "getattr: (-) %s", filename);
    return-ENOENT;
  }
}

int colonyfs_fusexx::fgetattr (const char* filename, struct stat* stbuf, struct fuse_file_info* fi) {

  try {
    const metadata_map_t::mapped_type& metadata = metadata_map_[filename];

    // Zero out the file stat buffer
    memset(stbuf, 0, sizeof(struct stat));
    memcpy(stbuf, &metadata.stbuf, sizeof(struct stat));

    clock_gettime(CLOCK_REALTIME, &stbuf->st_mtim);
    rLog(fuse_control_, "fgetattr: (+) %s", filename);
    return 0;

  } catch (colony::lookup_e& e) {
    rLog(fuse_control_, "fgetattr: (-) %s", filename);
    return -ENOENT;
  }
}

int colonyfs_fusexx::utime(const char* filename,  struct utimbuf* time) {

  rLog(fuse_control_, "utime: %s", filename);

  fattribute& metadata = metadata_map_[ filename ];

  metadata.stbuf.st_atim.tv_sec = time->actime;
  metadata.stbuf.st_mtim.tv_sec = time->modtime;

  return 0;
}

int colonyfs_fusexx::create(
    const char* filename, mode_t mode, struct fuse_file_info* fi) {

  boost::filesystem::path full(filename);
  boost::filesystem::path branch = full.branch_path();
  boost::filesystem::path leaf = full.leaf();

  // Create metadata for that filename.
  fattribute metadata;

  // Set the attributes.
  metadata.stbuf.st_mode = S_IFREG | mode; // File flags.
  metadata.stbuf.st_nlink = 1;
  metadata.stbuf.st_uid = getuid();
  metadata.stbuf.st_gid = getgid();
  clock_gettime(CLOCK_REALTIME, &metadata.stbuf.st_mtim); // Modification time.

  metadata_map_.commit(filename, metadata);

  rLog(fuse_control_, "create: %s", filename);

  // Register a new file with the parent.
  fattribute& pmetadata = metadata_map_[ branch.string() ];
  pmetadata.list.push_back(leaf.string());
  metadata_map_.commit(branch.string(), pmetadata);

  std::string data;
  data_map_.commit(filename, data);

  return 0;
}

int colonyfs_fusexx::write (
    const char* filename, const char* buffer,
    size_t size, off_t offset, struct fuse_file_info* fi) {

  // Write size.
  size_t length = offset + size;

  // Resize the data container if necessary.
  std::string& data = data_map_[ filename ];

  if (length > data.size()) {
    data.resize(length);
  }

  // Copy the data from buffer to FS.
  memcpy(&data[offset], buffer, size);

  // Adjust the file size attribute.
  fattribute& metadata = metadata_map_[ filename ];
  metadata.stbuf.st_size = data.size();
  metadata_map_.commit(filename, metadata);

  rLog(fuse_control_, "write: (%lu) %s", size, filename);

  return size;
}

int colonyfs_fusexx::unlink(const char* filename) {

  rLog(fuse_control_, "unlink: %s", filename);

  boost::filesystem::path path(filename);
  boost::filesystem::path branch = path.branch_path();
  boost::filesystem::path leaf = path.leaf();

  // Remove the node from the parent.
  fattribute& metadata = metadata_map_[ branch.string() ];
  fattribute::list_t& content = metadata.list;
  content.remove(leaf.string());
  metadata_map_.commit(branch.string(), metadata);

  // Delete the binary data.
  data_map_.erase(filename);

  // Delete metadata.
  metadata_map_.erase(filename);

  return 0;
}

int colonyfs_fusexx::chmod(const char* filename, mode_t mode) {

  rLog(fuse_control_, "chmod: %s", filename);

  int status;

  // What are the ownership flags for this file?
  fattribute& metadata = metadata_map_[ filename ];
  mode_t file_mode = metadata.stbuf.st_mode;

  // See if we are allowed to mess with the file.
  if ( (file_mode | mode) == mode ) {
    metadata.stbuf.st_mode = mode;
    status = 0;
  } else
    status = -EACCES;

  metadata_map_.commit(filename, metadata);

  return status;
}

int colonyfs_fusexx::opendir (const char* filename, struct fuse_file_info* fi) {

  rLog(fuse_control_, "opendir %s", filename);

  return 0;
}

int colonyfs_fusexx::rmdir(const char* filename) {

  rLog(fuse_control_, "rmdir: %s", filename);

  boost::filesystem::path path(filename);
  boost::filesystem::path branch = path.branch_path();
  boost::filesystem::path leaf = path.leaf();

  // Remove metadata for the given file.
  metadata_map_.erase(filename);

  try {
    // Get the directory content of the parent.
    metadata_map_t::mapped_type& pmetadata = metadata_map_[branch.string()];

    // Examine the durectory content and remove the entry.
    pmetadata.list.remove(leaf.string());

    // Commit the change.
    metadata_map_.commit(leaf.string(), pmetadata);

    return 0;
  } catch (colony::lookup_e& e) {
    rError("... metadata corruption -- parent directory not found");
    return -ENODEV;
  }
}
/*
metadata_map_t::mapped_type& colonyfs_fusexx::get_metadata(const char* filename) {

}
*/
rlog::RLogChannel* colonyfs_fusexx::fuse_control_( RLOG_CHANNEL( "fuse/control" ) );
