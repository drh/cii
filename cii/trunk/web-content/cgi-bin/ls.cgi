#!/usr/local/bin/python

# $Id$

import sys
import os
import cgi
import cgitb

def ls(dir=".", title=None):
    def process(_, dirpath, namelist):
        for name in namelist:
            if name[0] == '.':
		namelist.remove(name)
		continue
            path = os.path.join(dirpath, name)
            if os.path.isfile(path):
                print '<a href="%s">%s</a><br>' % (path, path[len(dir)+1:])
    while dir.endswith('/'):
	dir = dir[0:-1]
    if dir == "":
        dir = "."
    if not title:
        title = dir
    print """Content-Type: text/html

<!DOCTYPE HTML PUBLIC "-//IETF//DTD HTML//EN">
<HTML>
<HEAD><LINK HREF="http://drh.home.dyndns.org/" REV="made" TITLE="David R. Hanson">
<TITLE>%s</TITLE></HEAD>
<BODY>
<p>""" % title
    os.path.walk(dir, process, 0)
    print """</p>
<hr>
<body>
<html>"""

if __name__ == "__main__":
   cgitb.enable()
   f = cgi.FieldStorage()
   dir = f.getfirst("dir")
   title = f.getfirst("title")
   ls(dir, title)
