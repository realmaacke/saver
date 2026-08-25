# Saver
Saver is a version control built in c++.
Saver aims to be git like but with more control in the terminal.
Saver requires Saver-remote to be running on a server "https://github.com/realmaacke/saver-remote"

### Under devlopment.

## Commands

### General commands
- help -> Displays helpful information
- project -> Displays information about current project

### Authentication commands
- login -> Connects to the remote.
- logout -> Removes the "session"

### Project specific commands
- start -> initializes a project
- add <path/to/file> -> adds file/files to cache
- remove <path/to/file> -> remove file/files from cache
- reset -> Removes all files from cache
- describe "<message>" -> Describe the changes
- upload -> Loads cache to server
- download <url/to/project> -> Downloads project