This program spawns a child process and passes all command line arguments to it.  It's basically intended as a simple way to create aliases for a command.

To use, rename predirect.exe to the name you want to invoke, and create a configuration file which matches the alias name with ".predirect.config" appended to it.  For example, if you use "ruby.exe" as your alias, its configuration file should be "ruby.exe.predirect.config".

The contents of the configuration file should be the PATH to the actual executable you want to run.

If the alias is located in the SAME folder as the target executable, you can omit the path in the configuration file, providing only the executable name instead.

Example
---

Aptana Studio 3 on Windows hardcodes "ruby.exe" as the name of the Ruby executable.  To use JRuby with Aptana Studio 3, we need to alias "ruby.exe" to JRuby.

To do this:

 - copy predirect.exe to any location on your PATH
 - rename predirect.exe as ruby.exe
 - create a configuration file named *ruby.exe.predirect.config* file in the same folder as the ruby.exe we created
 - put the absolute PATH to jruby.exe as the sole contents of the *ruby.exe.predirect.config* file

This could also be used to seamlessly switch between different versions of apps located in different directories.  For example, set up python.exe in a utility folder at the beginning of your PATH and by editing the *python.exe.predirect.config* file you can switch between multiple versions of python at will without having to  modify your PATH.
