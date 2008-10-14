Introduction
=============

This project was started as part of an assignment at Murdoch University.
The assignment was to create a unix shell with basic features one might
expect from a shell. These features include:

 * Reconfigurable shell prompt (default "%"). This is not the same as
   PS1 - no parsing is done. Changing the prompt can be done via the
   `prompt` built-in
 * `pwd` as a shell built-in
 * Directory walking (`cd`)
 * Globbing - expanding expressions such as "*.c"
 * Input and output redirections via ">" and "<", i,e,. "cat <
   input > output"
 * Piping output from one command to another
 * Background job execution
 * Sequential job execution
 * `exit` as a shell built-in

Generally this shell is very primitive. It does not feature
tab-completion, history, or any of the other "luxuries" that other
shells, such as Bash or ZSH, have.

Installing
==========

Mush comes with a Makefile. As there aren't any dependencies, a
configure script, or the use of another auto-configuring build system
seemed unnecessary. To compile and install run the following commands:

	make
	make install

Alternatively wscripts have been provided. The waf executable itself is
not tracked by the Git repository, thus you must first build waf, then
you can execute the following commands:

	./waf configure
	./waf

The executable will be placed in ./build/default/src/mush.

Requirements
============

Mush has been tested on Mac OS and Linux, but it should run on any POSIX
compliant system. Mush does not depend on any libraries.

The unit tests, however, use [Cmockery](http://cmockery.googlecode.com).

License
=======

Mush is released under the MIT license. For more details, see the
LICENSE file.

