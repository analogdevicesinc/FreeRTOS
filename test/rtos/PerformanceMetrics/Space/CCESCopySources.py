#*********************************************************************************
#  Global Definitions
#
#*********************************************************************************
from xml.dom.minidom import parse
from xml.dom.minidom import getDOMImplementation
from win32com.client import gencache
from pywintypes import com_error
from shutil import copy2
from shutil import copytree, ignore_patterns
from stat import *
import pythoncom, time, msvcrt, os,string,threading
import sys
import getopt
import struct
import warnings
import shutil
import subprocess

__usage__ = """ \

Usage:
  CCESCopySources.py 

Description:
	Copies some checked-out sources to the given install directory.
Options:
  -d,   --directory    The directory to copy the sources to, should teh RTOS root.
  					   e.g. uCOS-III-Rel1.0.2

EXAMPLE:
python CCESCopySources.py -d 

"""

def DeleteFolder(dst, folder):

	cmd1 = 'rmdir /Q /S ' + folder
	print cmd1
	sys.stdout.flush()
	
	here = os.getcwd()
	os.chdir(dst)
	print os.getcwd()
	sys.stdout.flush()
	
	p1 = subprocess.Popen(cmd1, shell=True, stdout=subprocess.PIPE, stdin=subprocess.PIPE)
	returncode = p1.wait()
	#print 'Returncode : ' + str(returncode)
	
	os.chdir(here)

	
#*********************************************************************************

def CopySource(install, source):
	
	dstroot = os.path.join(install, 'uCOS-III', 'common', 'uCOS-III')
	dst = os.path.join(dstroot, 'Source')
	src = os.path.join(source, 'Source')
	
	
	print 'Copying ' + src + '\n'
	print 'To ' + dst + '\n'
	
	if os.path.isdir(dst):
		print 'Destination already exists '
		print 'Deleting'
		DeleteFolder(dstroot, 'Source')
	
	
	# now copy:
	copytree(src, dst, ignore=ignore_patterns('.svn', 'ARM*'))

def CopySharcPort(install, source):
	dstroot = os.path.join(install, 'uCOS-III','SHARC','uCOS-III')
	dst = os.path.join(dstroot, 'Ports')
	src = os.path.join(source, 'Ports','Sharc','CCES')
	print 'Copying ' + src + '\n'
	print 'To ' + dst + '\n'


	if os.path.isdir(dst):
		print 'Destination already exists '
		print 'Deleting'
		DeleteFolder(dstroot, 'Ports')
		#shutil.rmtree(dst, 1)

	# now copy:
	copytree(src, dst, ignore=ignore_patterns('.svn', 'ARM*'))

def CopyBlackfinPort(install, source):

	dstroot = os.path.join(install, 'uCOS-III','Blackfin','uCOS-III')
	dst = os.path.join(dstroot,'Ports')
	src = os.path.join(source, 'Ports','Blackfin','CCES')
	print 'Copying ' + src + '\n'
	print 'To ' + dst + '\n'
	
	
	if os.path.isdir(dst):
		print 'Destination already exists '
		print 'Destination already exists '
		print 'Deleting'
		DeleteFolder(dstroot, 'Ports')
		#shutil.rmtree(dst, 1)
	
	# now copy:
	copytree(src, dst, ignore=ignore_patterns('.svn', 'ARM*'))

def CopyTLS(install, source):

	dstroot = os.path.join(install, 'uCOS-III','common','uCOS-III','TLS')
	dst = os.path.join(dstroot, 'CCES')
	src = os.path.join(source, 'TLS','CCES')
	print 'Copying ' + src + '\n'
	print 'To ' + dst + '\n'


	if os.path.isdir(dst):
		print 'Destination already exists '
		print 'Deleting'
		DeleteFolder(dstroot, 'CCES')
		#shutil.rmtree(dst, 1)
	
	# now copy:
	copytree(src, dst, ignore=ignore_patterns('.svn', 'ARM*'))


#*********************************************************************************
# usage()
#*********************************************************************************
def usage():
	sys.stdout.write(__usage__)
	
#*********************************************************************************
# main()
#*********************************************************************************
def main( argv=sys.argv[1:] ):
	
	direct = ''	
	
	try:
		opts, args = getopt.getopt(argv, 'hd:', ['help', 'directory'])
	except getopt.GetoptError:
		usage()
		return
	for opt, arg in opts:
		if opt in ('-h', '--help'):
			usage()
			return
		if opt in ('-d', '--directory'):
			direct =  os.path.normpath(arg)
	
	currentdir = os.getcwd()
	
	print 'Copying from ' + currentdir + '\n'		
	print 'To ' + direct + '\n'		
	
	if os.path.isdir(direct):
		print 'Destination already exists '
	
	# I have to copy:
	
	CopySource(direct, currentdir)
	#CopyBlackfinPort(direct, currentdir)
	CopySharcPort(direct, currentdir)
	CopyTLS(direct, currentdir)
	
		
	cmd1= 'c:\\cygwin\\bin\\svn.exe upgrade'	
	print cmd1
	p1 = subprocess.Popen(cmd1)
	returncode = p1.wait()

	cmd1= 'c:\\cygwin\\bin\\svnversion '	
	print cmd1
	logfile = os.path.join(currentdir, 'version.txt')
	print 'Version output directed to ' + logfile + '\n'
	sys.stdout.flush()
	
	fh1 = open(logfile, 'w+')
	
	p1 = subprocess.Popen(cmd1, stdout=fh1)
	returncode = p1.wait()
	fh1.close()
	

	
#*********************************************************************************
if __name__ == '__main__':
	main()

