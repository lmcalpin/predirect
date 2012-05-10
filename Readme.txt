This program simply spawns a child process and passes all command line arguments to it.

To build, launch MingW and execute build.sh.

Configure predirect.config.

Example Usage:

Aptana Studio 3 on Windows hardcodes "ruby.exe" as the name of the Ruby executable.
To use JRuby with Aptana Studio 3, simply put predirect.exe on your PATH, rename the file as 
ruby.exe, and create a predirect.config file with the absolute PATH to your jruby.exe file
as the sole contents.

After that, when you launch Aptana Studio 3, and it should work.

