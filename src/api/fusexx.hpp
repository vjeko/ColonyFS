/*
 * loggerfs: a virtual file system to store logs in a database
 * Copyright (C) 2007 John C. Matherly
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */
#ifndef FUSEXX_H_
#define FUSEXX_H_

#define FUSE_USE_VERSION 26 // earlier versions have deprecated functions

#define _unimplemented_() { return 0; };

// C++ Headers
#include <string> // memset
#include <cstring>

// C Headers
#include <fuse.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

namespace fusexx {
    /*
     * fuse
     * Manages all the fuse operations. A very simple interface to the C fuse_ops struct.
     */
    template <class T>
	class fuse {
		/*
		 * Public Methods
		 */
		public:
			/*
			 * Main function of fusexx::fuse.
			 * 
			 * Calls the fuse 'fuse_main' macro.
			 * 
			 * @param argc the argument counter passed to the main() function
 			 * @param argv the argument vector passed to the main() function
 			 * @param user_data user data set in context for init() method
 			 * @return 0 on success, nonzero on failure
 			 */
			static int main (int argc, char **argv, void *user_data, T *t) {
				// Zero the operations struct
				memset (&T::operations, 0, sizeof (struct fuse_operations));
				
				// Load the operations struct w/ pointers to the respective member functions
				T::loadOperations ();
				
				// The 'self' variable will be the equivalent of the 'this' pointer
				if (t == NULL)
					return -1;
				T::self = t;
				
				// Execute fuse_main
				return fuse_main (argc, argv, &T::operations, user_data);
			}
			
    		static struct fuse_operations operations;
			
		/*
		 * Overload these functions
		 */
		 public:
			  static int readlink (const char*, char*, size_t) { _unimplemented_(); }
		    static int getattr (const char*, struct stat * ) { _unimplemented_(); }
		    static int getdir (const char*, fuse_dirh_t, fuse_dirfil_t) { _unimplemented_(); }
		    static int mknod (const char*, mode_t, dev_t) { _unimplemented_(); }
		    static int mkdir (const char*, mode_t) { _unimplemented_(); }
		    static int unlink (const char*) { _unimplemented_(); }
		    static int rmdir (const char*) { _unimplemented_(); }
		    static int symlink (const char*, const char*) { _unimplemented_(); }
		    static int rename (const char*, const char*) { _unimplemented_(); }
		    static int link (const char*, const char*) { _unimplemented_(); }
		    static int chmod (const char*, mode_t) { _unimplemented_(); }
		    static int chown (const char*, uid_t, gid_t) { _unimplemented_(); }
		    static int truncate (const char*, off_t) { _unimplemented_(); }
		    static int utimens (const char*, struct utimbuf *) { _unimplemented_(); }
		    static int open (const char*, struct fuse_file_info *) { _unimplemented_(); }
		    static int read (const char*, char*, size_t, off_t, struct fuse_file_info *) { _unimplemented_(); }
		    static int write (const char*, const char*, size_t, off_t,struct fuse_file_info *) { _unimplemented_(); }
		    static int statfs (const char*, struct statvfs *) { _unimplemented_(); }
		    static int flush (const char*, struct fuse_file_info *) { _unimplemented_(); }
		    static int release (const char*, struct fuse_file_info *) { _unimplemented_(); }
		    static int fsync (const char*, int, struct fuse_file_info *) { _unimplemented_(); }
		    static int setxattr (const char*, const char*, const char*, size_t, int) { _unimplemented_(); }
		    static int getxattr (const char*, const char*, char*, size_t) { _unimplemented_(); }
		    static int listxattr (const char*, char*, size_t) { _unimplemented_(); }
		    static int removexattr (const char*, const char*) { _unimplemented_(); }
		    static int readdir (const char*, void *, fuse_fill_dir_t, off_t, struct fuse_file_info *) { _unimplemented_(); }
		    static int opendir (const char*, struct fuse_file_info *) { _unimplemented_(); }
		    static int releasedir (const char*, struct fuse_file_info *) { _unimplemented_(); }
		    static int fsyncdir (const char*, int, struct fuse_file_info *) { _unimplemented_(); }
		    static void *init (struct fuse_conn_info *conn) { return NULL; }
		    static void  destroy (void *) { }
		    static int access (const char*, int) { _unimplemented_(); }
		    static int create (const char*, mode_t, struct fuse_file_info *) { _unimplemented_(); }
		    static int ftruncate (const char*, off_t, struct fuse_file_info *) { _unimplemented_(); }
		    static int fgetattr (const char*, struct stat *, struct fuse_file_info *) { _unimplemented_(); }

		    static void loadOperations () {
				operations.readlink = T::readlink;
			    operations.getattr = T::getattr;
			    operations.getdir = T::getdir;
			    operations.mknod = T::mknod;
			    operations.mkdir = T::mkdir;
			    operations.unlink = T::unlink;
			    operations.rmdir = T::rmdir;
			    operations.symlink = T::symlink;
			    operations.rename = T::rename;
			    operations.link = T::link;
			    operations.chmod = T::chmod;
			    operations.chown = T::chown;
			    operations.truncate = T::truncate;
			    operations.utime = T::utime;
			    operations.open = T::open;
			    operations.read = T::read;
			    operations.write = T::write;
			    operations.statfs = T::statfs;
			    operations.flush = T::flush;
			    operations.release = T::release;
			    operations.fsync = T::fsync;
			    operations.setxattr = T::setxattr;
			    operations.getxattr = T::getxattr;
			    operations.listxattr = T::listxattr;
			    operations.removexattr = T::removexattr;
			    operations.readdir = T::readdir;
			    operations.opendir = T::opendir;
			    operations.releasedir = T::releasedir;
			    operations.fsyncdir = T::fsyncdir;
			    operations.init = T::init;
			    operations.destroy = T::destroy;
			    operations.access = T::access;
			    operations.create = T::create;
			    operations.ftruncate = T::ftruncate;
			    operations.fgetattr = T::fgetattr;
			}
			
			/*
			 * Protected variables
			 */
		protected:
			// allow static methods to access object methods/ variables using 'self' instead of 'this'
			static T *self;
	};
	
	template <class T> struct fuse_operations fuse<T> ::operations;
	template <class T> T * fuse<T> ::self;
}

#endif /*FUSEXX_H_*/
