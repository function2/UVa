#!/usr/bin/env python2
# Copyright (C) 2015  Michael Seyfert <michael@codesand.org>

# Build instructions for projecteuler.net solutions.

import os,os.path,re,subprocess,glob

# Use clang
env = Environment(CXX = 'clang++')

BUILD_DIR = 'build'
SRC_DIR = 'src'
INCLUDE_DIR = 'src'

env.SConsignFile("%s/scons/scons-signatures" % BUILD_DIR)

INCLUDE_FLAGS = ' -I/usr/local/include -I%s ' % INCLUDE_DIR
BUILD_FLAGS = INCLUDE_FLAGS

# Run with 'scons debug=1' to get a debug build.
if ARGUMENTS.get('debug', 0):
    BUILD_FLAGS += ' -DDEBUG -g '
    LINK_FLAGS = ' -L/usr/local/lib '
else:
#    BUILD_FLAGS += ' -O9 -mmmx -msse -msse2 -msse3 '
    BUILD_FLAGS += ' -Ofast '
    LINK_FLAGS = ' -L/usr/local/lib -s '

# Use c++11 (14?) features.
BUILD_FLAGS += ' -std=c++1y '
BUILD_FLAGS += ' -Wall -Werror -Wunreachable-code '

env.Append(LINKFLAGS = LINK_FLAGS)

################################################################################

# Sources is a list of tuples of
# (Output object file, Input source file)
sources = []

r = re.compile("") # Remove "directory"
r2 = re.compile("\.cpp") # Remove ".cpp"

# All source files.
g = glob.glob("1_1000/*/*.cpp")
g += glob.glob("10000_11000/*/*.cpp")

for source in g:
    name = r.sub('',source)
    name = r2.sub('',name)
    sources.append( ('obj/' + name, source) )

extra_link_flags = {
#    'problem_25' : '-lgmp -lgmpxx',
}

r = re.compile("obj/") # Remove "obj/"
for x in sources:
    out_file = '%s/%s' % (BUILD_DIR, x[0])
    in_file  = x[1]
    obj = env.StaticObject(out_file + '.o', in_file, parse_flags = BUILD_FLAGS)
    name = r.sub('',x[0])
    extra_link = extra_link_flags.get(name,'')
    env.Program(
        "%s/%s" % (BUILD_DIR, name),
        obj,
        parse_flags = ' ' + extra_link,)
