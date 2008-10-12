#!/usr/bin/env python

APPNAME = 'mush'
VERSION = '1.0'

srcdir = '.'
blddir = 'build'

def set_options(opt):
	pass

def configure(conf):
	conf.check_tool('gcc')

def build(bld):
	bld.add_subdirs('src')
