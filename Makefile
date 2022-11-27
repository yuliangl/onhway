#
# Makefile for onhway
#	auth : onhway
#	date : 2022-11-27
#

curdir=/home/onhway/workspace/onhway
src=$(curdir)/src
bin=$(curdir)/bin
test=$(curdir)/testmain
lib=

VPATH = $(src)

Apps = testlog
srclist = $(src)/log.cc

$(Apps) : $(srclist)
	g++ -o 







