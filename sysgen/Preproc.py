# -*- coding: latin-1 -*-

__version__="0.9.0"

__copyright__="""
   k_os (Konnex Operating-System based on the OSEK/VDX-Standard).
 
   (C) 2007-2010 by Christoph Schueler <chris@konnex-tools.de,
                                        cpu12.gems@googlemail.com>
  
   All Rights Reserved
 
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
"""

'''
try:
    import psyco
    psyco.full()
except ImportError:
    pass
'''

import os
import re
import sys

SCANNING=0
MULTI_LINE_COMMENT=1

FileList=[]
PathList=[]

of=None

E_UNBALANCED_DELIMS=1


INCLUDE_DIRECTIVE=re.compile(r'[ \t]*?#[ \t]*?include[ \t]*?(?P<sdelim>[<"])(?P<incfile>.*?)(?P<edelim>[>"])(?P<rest>.*)')
STRING=re.compile(r'(?P<before>.*?)["](?P<string>.*?)["](?P<after>.*)')
START_COMMENT=re.compile(r'\/(?P<char>\/|\*)')
END_COMMENT=re.compile(r'\*\/')
USE_FILE=re.compile(r'\s*?USE\s*?=\s*?"(?P<includeFile>.*?)"\s*?;.*')


def StripStrings(line):
    match=STRING.match(line)
    while match is not None:
        line=match.group('before')+match.group('after')
        match=STRING.match(line)
    return line


def CreateOutFile(fname):
    dir,fn=os.path.split(fname)
    root,ext=os.path.splitext(fn)
    return open(os.path.join(dir,root+".i"),"w")


def AddToPathList(path):
    if path not in PathList:
        print "Adding Include Path '%s'.\n" % (path)
        PathList.append(path)


def AddToFileList(fname):
    if fname not in FileList:
        FileList.append(fname)


def TryOpen(fname):
    if not os.path.exists(fname):
         return False
    else:
        return True


def locateIncludeFile(quoted,fname):
    path=None
    found=False
    if quoted:
        ## Try current directory first.
        path=os.path.join(os.path.curdir,fname)
        found=TryOpen(path)
    if not found:
        for p in PathList:
            path=os.path.join(p,fname)
            found=TryOpen(path)
            if found:
                break
    return (found,path)


def lineDirective(file_name,lineNo):
    global of
    print >> of,'#line %u "%s"' % (lineNo,file_name)


def Parse(fname,errorObj):
    try:    
        inf=open(fname)
    except IOError as e:
        print e
        sys.exit(2)
    print
    print '=' * 79
    print " SCANNING: '%s' ..." % (fname)
    print '=' * 79

    lineDirective(fname,1)

    state=SCANNING
    cmtStartLline=0

    useFiles=[]
    AddToFileList(fname)
    
    for lineNo,line in enumerate(inf,1):
        if state==SCANNING:
            tmpLine=StripStrings(line)
            startCommentTmp=START_COMMENT.search(tmpLine)
            if startCommentTmp is not None:
                startComment=START_COMMENT.search(line)
                start=startComment.start()
                if startComment.group('char')=='/':
                    line=line[:start]+'\n'   ## Strip Single-Line-Comment.
                else:
                    endComment=END_COMMENT.search(line)
                    if endComment is None:
                        state=MULTI_LINE_COMMENT   ## Start Multi-Line-Comment.
                        cmtStartLine=lineNo
                        line=line[:start]+'\n'
                        #print "Multi-Line-Comment startet  @ line: ",cmtStartLline
                    else:
                        end=endComment.end()
                        line=line[:start]+line[end:-1]+'\n'
            else:
                pass
            includeStmt=INCLUDE_DIRECTIVE.match(line)
            if includeStmt is not None:
                sdelim,incfile,edelim,_=includeStmt.groups(('sdelim','incfile','edelim','rest'))
                if (sdelim=='"' and edelim!='"') or (sdelim=='<' and edelim!='>'):
                    ## todo: Error-Module!!!
                    errorObj.fatalError('Unbalanced deliminiters '+sdelim+','+edelim+' in #include-statement',
                        filename=fname,lineno=lineNo)
                else:
                    ## Warn, if no comment.
                    print >> of,'\n',
                    found,incPath=locateIncludeFile(sdelim=='"',incfile)
                    if found:
                        Parse(incPath,errorObj)
                        lineDirective(fname,lineNo+1)
                    else:
                        errorObj.fatalError("Could not open include file '%s'.\n" % (incfile,),
                            filename=fname,lineno=lineNo)
            else:
                uf=USE_FILE.match(line)
                if uf is not None:
                    useFiles.append(uf.groupdict('includeFile'))
                print >> of,line,
        elif state==MULTI_LINE_COMMENT:
            endComment=END_COMMENT.search(line)
            if endComment is not None:
                endCommentLine=lineNo
                state=SCANNING
                #print "Multi-Line-Comment finished @ line: ",endCommentLine
                print >> of
            else:
                print >> of
    for f in useFiles:
        found,incPath=locateIncludeFile(sdelim=='"',incfile)
        
    inf.close()


def Parser(fname,errorObj):
    inc_paths=os.getenv('KOS_INCLUDE')
    if inc_paths is not None:
        for p in inc_paths.split(';'):
            AddToPathList(p)
    global of
    of=CreateOutFile(fname)
    Parse(fname,errorObj)
    of.close()

if __name__=='__main__':
    Parser(r'test.oil')
