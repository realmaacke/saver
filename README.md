# Saver
Saver is a version control built in c++.
Saver aims to be git like.
Saver requires Saver-remote to be running on a server "https://github.com/realmaacke/saver-remote"

### Under devlopment.

## Commands

### General commands
- help -> Displays helpful information.
- project -> Displays information about current project (not done) (should contain: chapter, uploads, describes)

### Authentication commands
- connect <username> <password> - connects the saver instance to the remote (will be reworked).
- disconnect - disconnects the saver instance from the remote

### Project specific commands
- start <path> <optional-flag> - starts a new project, (requires saver to be connected).
- add <path/to/file> - adds files to the staging environment.
- remove <path/to/file> - removes file/files from the staging environment.
- reset - resets the staging environment.
- describe "<message>" - Describes the staging environment.
- upload - sends the describe to the remote.
- chapter:
    - <new> <chapter-name> - creates a new chapter.
    - <use> <chapter-name> - use an existing chapter (rewrite HEAD)
    - <del> <chapter-name> - delete an existing chapter
    - <sync> - Syncronizes the local chapters with the remote chapters
