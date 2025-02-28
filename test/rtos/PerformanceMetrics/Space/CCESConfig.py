#*********************************************************************************
#  Global Definitions
#
#*********************************************************************************
from xml.dom.minidom import parse
from xml.dom.minidom import getDOMImplementation
from win32com.client import gencache
from pywintypes import com_error
from shutil import copy2
import pythoncom, time, msvcrt, os,string,threading
import sys
import getopt
import struct
import warnings
import shutil
import subprocess

__usage__ = """ \

Usage:
  CCESConfig.py 

Description:
	Create the projects for a given processor to test.   Creates a folder called
	<processor> with the test projects in place for testing.  Also copies
	and configures a config.txt file for TTH.
	For processors that use SWC in SHARC, two sets of projects are created:
		- A SWC version.
		- A NWC version (the generated folder is appended with '-NWC'.
	For BF70x processors, two sets of projects are created: 
		- One with uCOS in RAM.
		- One that uses ROM Configuration2 (the generated folder is appended 
		  with '-ROM2'.
Options:
  -p,   --processor    The processor to make the projects for, e.g. ADSP-BF512.
  -f,   --family       The processor family to make the projects for.  I could 
                       get this from the processor name, but I'm lazy. Either
                       'Blackfin' or 'Sharc'.
  -r    --rom_cfg      ROM configuration to get figures for. Options are: 1, or 2.
  -d    --dualcore     Whether the family is dual core or not, e.g. Devonshire.
  -c    --clean        Remove the output directory ('Footprint', 'Performance'), 
                       ready for a new build.
  -i    --install      Installation directory for the CCES being used.
  -v    --verbose      Be *even more* verbose than we currently are.
  -s    --si_revision  Force the silicon revision to a specific value.
  -e    --emulator     Use the emulator debug session to run the test.  Must
                       be used for a specific processor. Options are:
                       hpusb, usb, 100b, 1000, or 2000.

EXAMPLE:
 For specific processors:
python CCESConfig.py -p ADSP-BF532 -f Blackfin 
python CCESConfig.py -p ADSP-BF609 -f Blackfin -d
python CCESConfig.py -p ADSP-21469 -f Sharc 
python CCESConfig.py -p ADSP-SC589 -f Sharc -d
python CCESConfig.py -p ADSP-SC589 -f Arm 
 For an entire family: 
 Singlecores:
python CCESConfig.py -f Blackfin 
python CCESConfig.py -f Sharc 
 Dualcores:
python CCESConfig.py -f Blackfin -d  
 ROM metrics:
python CCESConfig.py -f Blackfin -r 2

"""

g_Projnames = ['FreeRTOS_MIN_ALL', 
		'FreeRTOS_MIN_FLG', 
		'FreeRTOS_MIN_MUT', 
		'FreeRTOS_MIN_NOP', 
		'FreeRTOS_MIN_SEM', 
		'FreeRTOS_MIN_MSG']

g_SplusProjnames = ['FreeRTOS_MIN_ALL_Core1',  
		'FreeRTOS_MIN_FLG_Core1',  
		'FreeRTOS_MIN_MUT_Core1', 
		'FreeRTOS_MIN_NOP_Core1', 
		'FreeRTOS_MIN_SEM_Core1', 
		'FreeRTOS_MIN_MSG_Core1']

g_ArmProjnames = ['FreeRTOS_MIN_ALL_Core0', 
		'FreeRTOS_MIN_FLG_Core0', 
		'FreeRTOS_MIN_MUT_Core0', 
		'FreeRTOS_MIN_NOP_Core0', 
		'FreeRTOS_MIN_SEM_Core0', 
		'FreeRTOS_MIN_MSG_Core0']


g_Sharcs = ['ADSP-21160', 
			'ADSP-21161', 
			'ADSP-21261', 
			'ADSP-21262', 
			'ADSP-21266', 
			'ADSP-21362', 
			'ADSP-21363', 
			'ADSP-21364', 
			'ADSP-21365', 
			'ADSP-21366', 
			'ADSP-21367', 
			'ADSP-21368', 
			'ADSP-21369', 
			'ADSP-21371', 
			'ADSP-21375', 
			'ADSP-21467', 
			'ADSP-21469', 
			'ADSP-21477', 
			'ADSP-21478', 
			'ADSP-21479', 
			'ADSP-21483', 
			'ADSP-21486', 
			'ADSP-21487', 
			'ADSP-21488', 
			'ADSP-21489',
			'ADSP-21569',
			'ADSP-SC589',
			'ADSP-SC594',
			'ADSP-SC573'] 


g_Arms = ['ADSP-SC589',
			'ADSP-SC573',
			'ADSP-SC594']


g_Blackfins =  ['ADSP-BF504', 
			'ADSP-BF504F', 
			'ADSP-BF506F', 
			'ADSP-BF512', 
			'ADSP-BF514', 
			'ADSP-BF516', 
			'ADSP-BF518', 
			'ADSP-BF522', 
			'ADSP-BF523', 
			'ADSP-BF524', 
			'ADSP-BF525', 
			'ADSP-BF526', 
			'ADSP-BF527', 
			'ADSP-BF531', 
			'ADSP-BF532', 
			'ADSP-BF533', 
			'ADSP-BF534', 
			'ADSP-BF536', 
			'ADSP-BF537', 
			'ADSP-BF538', 
			'ADSP-BF539', 
			'ADSP-BF542', 
			'ADSP-BF542M', 
			'ADSP-BF544', 
			'ADSP-BF544M', 
			'ADSP-BF547', 
			'ADSP-BF547M', 
			'ADSP-BF548', 
			'ADSP-BF548M', 
			'ADSP-BF549', 
			'ADSP-BF549M', 
			'ADSP-BF592-A',
			'ADSP-BF700', 
			'ADSP-BF701', 
			'ADSP-BF702', 
			'ADSP-BF703', 
			'ADSP-BF704', 
			'ADSP-BF705', 
			'ADSP-BF706', 
			'ADSP-BF707' ] 

g_DualCores = ['ADSP-BF561', 
			'ADSP-BF606', 
			'ADSP-BF607', 
			'ADSP-BF608', 
			'ADSP-BF609']

g_RomProcessors = ['ADSP-BF701', '0.0',
			'ADSP-BF702', '0.0',
			'ADSP-BF703', '0.0',
			'ADSP-BF704', '0.0',
			'ADSP-BF705', '0.0',
			'ADSP-BF706', '0.0',
			'ADSP-BF707', '0.0']

g_VisaSharcs = ['ADSP-21467',
			'ADSP-21469',
			'ADSP-21477',
			'ADSP-21478',
			'ADSP-21479',
			'ADSP-21483',
			'ADSP-21486',
			'ADSP-21487',
			'ADSP-21488',
			'ADSP-21489',
			'ADSP-21569']


# Copy the config file from the source directory into the newly-created
# destination folder.
#
def CreateConfig(proc, dst, src, nwc, si_rev):
	if g_verbose == 1:
		print 'Creating config file for ' + proc + ' si-rev ' + si_rev

	template = os.path.join(src, 'config.tmplate')
	output =  os.path.join(src, 'config.txt')

	fin  = open(template, 'r')
	fout = open(output, 'w')

	logfilename = proc.lower() + '-test-output'

	line = ' '
	while not line == '':
		line = fin.readline()
		line = line.replace('PROCESSOR_TEMPLATE', proc)
		line = line.replace('SESSION_TEMPLATE', g_emulator)
		line = line.replace('LOGFILE_TEMPLATE', logfilename)
		if nwc == 1:
			line = line.replace('PROCESSORTEST_TEMPLATE', proc + '-NWC')
		elif g_rom_cfg != 'none':
			line = line.replace('PROCESSORTEST_TEMPLATE', proc + '-ROM' + str(g_rom_cfg))
		else:
			line = line.replace('PROCESSORTEST_TEMPLATE', proc)
		fout.writelines(line)

	fin.close()
	fout.close()

# Create a new project.
#  TODO must not hardcode path for CCES
#
def CreateProject(src, dst, proc, pname, nwc, si_revision):

	srcprojects = src
	dstprojects = dst

	cces = os.path.join(g_install, "Eclipse", "ccesc.exe")
	print '\n'
	print '###Creating project ' + pname 

	# Only set the si revision if it is given,
	if (si_revision != ''):
		si_rev_str = ' -revision ' + si_revision
	else:
		si_rev_str = ''

	# splurge out the headless builder command.	
	cmd = cces
	cmd += ' -nosplash -consoleLog -application com.analog.crosscore.headlesstools '
	cmd += si_rev_str
	cmd += ' -data "' + dstprojects + '"'
	cmd += ' -project "' + srcprojects + '"' 
	cmd += ' -processor ' + proc + ' -regensrc -cleanBuild all'

	# For Sharc we might want to use Normal Word, or Short Word (when it can be specified).
	if nwc == 1:
		cmd += ' -remove-switch compiler -swc' 
		cmd += ' -append-switch compiler -nwc' 

	print cmd
	logfile = os.path.join(dst, pname.lower() + '_createlog.txt')

	if g_verbose == 1:
		print cmd
		print 'Build output directed to ' + logfile + '\n'
		sys.stdout.flush()

	fh1 = open(logfile, 'a+')

	p1 = subprocess.Popen(cmd, stdout=fh1)
	returncode = p1.wait()
	fh1.close()

	if g_verbose == 1:
		print 'We want the build to return 0 - indicates success'
		print 'Build returned ' + str(returncode)

	attempts = 1
	while (returncode != 0) and (attempts < 6):
		print 'Creation returned ' + str(returncode)
		sys.stdout.flush()
		time.sleep(2)
		if g_verbose == 1:
			print 'Deleting ' + os.path.join(dst, '.metadata')
			print 'Deleting ' + os.path.join(dst, pname)
		try:
			str(shutil.rmtree(os.path.join(dst, '.metadata')))
			str(shutil.rmtree(os.path.join(dst, pname)))
		except:
			print 'UNABLE TO DELETE FILES OR FOLDERS - MAY CAUSE PROBLEMS WITH REBUILD'
		sys.stdout.flush()
		fh1 = open(logfile, 'a+')

		p1 = subprocess.Popen(cmd, stdout=fh1)
		returncode = p1.wait()
		fh1.close()
		attempts += 1
		

	if (proc == 'ADSP-BF561'):
		#  This is a workaround to avoid the (very annoying) case where the headless
		#  builder does not remove the cplb tab file that was originally part of the 
		#  source BF609 projects that are used as the BlackfinDualCore source.  
		#  We now have to do this manually.
		cplbtabfile = os.path.join(dst, pname, 'system', 'startup_ldf', 'app_cplbtab.c')
		print '\nChecking if ' + cplbtabfile + ' exists'
		if os.path.isfile(cplbtabfile):
			print 'app_cplbtab.c file exists - removing for BF561'
			os.remove(cplbtabfile)

	if returncode == 0:
		print 'Creation successful.'
		if attempts > 1:
			print 'After ' +str(attempts) + ' attempts.'
	else:
		print 'Build returned ' + str(returncode) + '. ERRORS WITH BUILD. \n'


# Force update of the project resources.
#
def UpdateProject(proj, direct, si_rev):

	cces = os.path.join(g_install, "Eclipse", "ccesc.exe")

	cmd2 = cces
	cmd2 += ' -nosplash -consoleLog -application com.analog.crosscore.headlesstools' 
	cmd2 += ' -project "' + proj + '"' 
	cmd2 += ' -revision ' + si_rev 
	cmd2 += ' -data "' + direct + '"'
	cmd2 += ' -build release'

	if g_verbose == 1:
		print '\nUpdate and build the project, this checks that the project is correct.' 
		sys.stdout.flush()

	fname = os.path.join(direct, 'buildlog.txt')

	print 'Quick build....\n'
	sys.stdout.flush()

	fff = open(fname, 'a+')
	p1 = subprocess.Popen(cmd2, stdout=fff)
	retcode = p1.wait()

	if g_verbose == 1:
		print 'We want the build to return code 0 - indicates success'
		print 'Build returned ' + str(retcode)

	attempts = 1	
	while (retcode != 0) and (attempts < 6):
		print 'Build returned failure (code: ' + str(retcode) + ')'
		sys.stdout.flush()
		#time.sleep(2)
		p1 = subprocess.Popen(cmd2)
		retcode = p1.wait()
		attempts += 1

	if retcode == 0:
		print 'Build successful.'
		if attempts > 1:
			print 'After ' +str(attempts) + ' attempts.'
	else:
		print 'Rebuild FAILED on attempt: ' + str(attempts) + '\n'
	fff.close()



# Copy the system.svc file to the destination folder.  Currently this isn't
# happening properly for project copying.
#
def CopySystemSVC(src, dst):
	
	oldsvc = os.path.join(src, 'system.svc')
	
	print 'Copying: '
	print oldsvc
	print 'to'
	print dst
	shutil.copy2(oldsvc, dst)


# For a given processor:
# 	- Create a folder called <processor>
# 	- Copy the correct source projects into that new directory.
# 	- Create a config.txt for the new processor.
def CreateProcTests(family, proc, dualcore, nwc, si_rev):

	if (family == 'Blackfin') and (dualcore == 1):
		srcfolder = os.path.join(os.getcwd(), 'BlackfinDualCore')
		projnames = g_Projnames 
	elif (family == 'Blackfin') and (dualcore == 0) and (g_rom_cfg == 'none'):
		srcfolder = os.path.join(os.getcwd(), 'BlackfinSingleCore')
		projnames = g_Projnames 
	elif (family == 'Blackfin') and (g_rom_cfg == '1'):
		srcfolder = os.path.join(os.getcwd(), 'BigDeltaROM1')
		projnames = g_Projnames 
	elif (family == 'Blackfin') and (g_rom_cfg == '2'):
		srcfolder = os.path.join(os.getcwd(), 'BigDeltaROM2')
		projnames = g_Projnames 
	elif (family == 'Sharc') and (dualcore == 0):
		srcfolder = os.path.join(os.getcwd(), 'Sharc')
		projnames = g_Projnames 
	elif (family == 'Sharc') and (dualcore == 1):
		srcfolder = os.path.join(os.getcwd(), 'SharcPlus')
		projnames = g_SplusProjnames 
	elif (family == 'Arm') and (dualcore == 0):
		srcfolder = os.path.join(os.getcwd(), 'Arm')
		projnames = g_ArmProjnames 
	else:
		# Don't know what this is
		print 'family and dualcore options incorrect\n'
		usage()
		return
	
	if nwc == 1:
		dstfolder = os.path.join(os.getcwd(), proc + '-NWC')
		print 'NWC folder: ' + dstfolder
	elif (g_rom_cfg != 'none'):
		dstfolder = os.path.join(os.getcwd(), proc + '-ROM' + g_rom_cfg)
		print 'ROM folder: ' + dstfolder
	elif family  == 'Arm':
		dstfolder = os.path.join(os.getcwd(), proc + '-ARM')
		print 'Arm folder: ' + dstfolder
	elif (family == 'Sharc') and (dualcore == 1):
		dstfolder = os.path.join(os.getcwd(), proc + '-SHARC')
		print 'SHARC folder: ' + dstfolder
	else:
		dstfolder = os.path.join(os.getcwd(), proc)
	
	# Create the folder to use
	if os.path.isdir(dstfolder):
		print 'Destination folder exists:  ' + dstfolder
		oldresultsfile = os.path.join(dstfolder, 'results.xml')
		if os.path.isfile(oldresultsfile):
			print 'Results file for ' + proc + ' exists, removing: ' + oldresultsfile + '\n'
			os.remove(oldresultsfile)
	else:
		print 'Creating: ' + dstfolder
		os.mkdir(dstfolder)
	
	print '\n\n\n'
	print 'Source Folder: ' + srcfolder
	print 'Destination Folder: ' + dstfolder
	sys.stdout.flush()

	for pname in projnames:
		proj = os.path.join(srcfolder, pname)
		newproject = os.path.join(dstfolder, pname)
		CreateProject(proj, dstfolder, proc, pname, nwc, si_rev)
		CopySystemSVC(proj, newproject)
		UpdateProject(newproject, dstfolder, si_rev)

	# There is a single config file for all projects in the space case
	# and one per project in cycles
	CreateConfig(proc, dstfolder, srcfolder, nwc, si_rev)


# returns a string suitable for a config.txt file, to specify an
# emulator type.  
# emulator = either 'hpusb', 'usb', '100b', '1000' or '2000'.
def CreateEmulatorString(proc, emulator):
	if emulator == '':
		return ''

	# We need to extract the end of the processor string, i.e. everything
	# after the 'ADSP-'...so we just use the characters from 5 onwards.
	processor = proc[5:].lower()

	# Now form the emulator string for the config.txt
	# it should ultimately look something like this:
	# DEBUGSESSION:<emu-bf518-hpusb>

	return 'DEBUGSESSION:<emu-' + processor + '-' + emulator +'>'

#*********************************************************************************
# usage()
#*********************************************************************************
def usage():
	sys.stdout.write(__usage__)
	
#*********************************************************************************
# main()
#*********************************************************************************
def main( argv=sys.argv[1:] ):
	global g_Blackfins
	global g_DualCores
	global g_Sharcs
	global g_Projnames
	global g_verbose
	global g_install
	global g_emulator
	global g_rom_cfg

	proc =  ''	
	si_revision = 'none'	
	family = ''	
	dualcore  = 0
	use_rom  = 0
	
	families = ['Blackfin', 'Sharc', 'Arm']
	emulators = ['not_needed', 'hpusb', 'usb', '100b', '1000', '2000']
	rom_cfgs = ['none', '1', '2']

	# We set a default emulator in case that the -e option is not passed down since it
	# is not required for footprint metrics.
	g_emulator = 'not_needed'
	g_rom_cfg = 'none'
	verbose = 0
	g_install = ''
    
	try:
		opts, args = getopt.getopt(argv, 'hds:p:f:e:r:c:i:v', ['help', 'dualcore', 'si_revision=', 'processor=', 'family=', 'emulator=', 'rom_cfg=', 'clean=', 'install=', 'verbose'])
	except getopt.GetoptError:
		print 'Options error'
		usage()
		return
	for opt, arg in opts:
		if opt in ('-h', '--help'):
			usage()
			return
		if opt in ('-p', '--processor'):
			proc = arg 
		if opt in ('-s', '--si_revision'):
			si_revision = arg 
		if opt in ('-f', '--family'):
			family = arg
		if opt in ('-d', '--dualcore'):
			dualcore = 1
		if opt in ('-r', '--rom_cfg'):
			use_rom = 1
			g_rom_cfg = arg
		if opt in ('-i', '--install'):
			g_install = arg;
		if opt in ('-v', '--verbose'):
			verbose = 1
		if opt in ('-e', '--emulator'):
			g_emulator = arg 
		if opt in ('-c', '--clean'):
			# Just clean up the requested folder, if there, and return.
			folder = os.path.join(os.getcwd(), arg)
			if os.path.isdir(folder):
				print 'Removing: ' + folder
				shutil.rmtree(folder, 1)
				return
			else:
				print 'No such directory: ' + folder
				return

	if family not in families:
		print 'Processor family "' + family + '" not recognised\n'
		usage()
		return

	if g_emulator not in emulators:
		print 'Unrecognised emulator ' + g_emulator
		usage()
		return

	if g_rom_cfg not in rom_cfgs:
		print 'Unrecognised rom configuration ' + g_rom_cfg
		usage()
		return

	processors = proc

	g_verbose = verbose

	# You can only specify an emulator when you specify a single processor,
	# e.g. you can't say "test family blackfin, they're all connected to a hpusb".
	if (proc == '') and not (g_emulator == 'not_needed'):
		print '\n\n ERROR:: If you specify an emulator, you *MUST* specify a single processor to test'
		usage()
		return

	g_emulator = CreateEmulatorString(proc, g_emulator)
	print 'Emulator setting: ' + g_emulator

	if not g_install:
		g_install = os.environ.get('CCES_HOME')
	g_install = os.path.normpath(g_install)
	if not os.path.isdir(g_install):
		print 'The following install directory does not exist: ' + g_install
		return

	if family == 'Blackfin':
		# If a processor is specified, make sure it matches the family.
		if proc == '':
			print 'Creating projects for the entire ' + family + ' family'
			if dualcore == 1: 
				processors = g_DualCores
			elif use_rom == 1: 
				processors = g_RomProcessors
			else:
				processors = g_Blackfins
		elif not (proc in g_Blackfins) and not (proc in g_DualCores) and not (proc in g_RomProcessors):
			print 'Processor ' + proc + 'not recognised as a blackfin'
			usage()
			return
		# else we have specified one of the blackfins
		# so do nothing.
		
		
	elif family == 'Sharc':
		# If a processor is specified, make sure it matches the family.
		if proc == '':
			print 'Creating projects for the entire ' + family + ' family'
			processors = g_Sharcs
		elif proc not in g_Sharcs:
			print 'Processor ' + proc + 'not recognised as a sharc'
			usage()
			return
	elif family == 'Arm':
		# If a processor is specified, make sure it matches the family.
		if proc == '':
			print 'Creating projects for the entire ' + family + ' family'
			processors = g_Arms
		elif proc not in g_Arms:
			print 'Processor ' + proc + 'not recognised as an arm'
			usage()
			return
	else:
		print 'Family still not recognised'
		usage()
		return
	
	print 'Processors to create: ' + str(processors)
	if proc == '':
		for processor in processors:
			print 'Creating: ' + processor
			sys.stdout.flush()
			CreateProcTests(family, processor, dualcore, 0, "none")
	else:
		if ((use_rom == 1) and (proc in g_RomProcessors)):
			print 'Creating ROM projects only: ' + proc
			element_index = g_RomProcessors.index(proc)
			si_revision = g_RomProcessors[element_index + 1]
			print 'Creating ROM projects only: ' + si_revision
			sys.stdout.flush()
			CreateProcTests(family, proc, dualcore, 0, si_revision)
		else:
			print 'Creating: ' + proc
			sys.stdout.flush()
			CreateProcTests(family, proc, dualcore, 0, "none")

		if proc in g_VisaSharcs:
			print 'Creating visa projects...'
			sys.stdout.flush()
			CreateProcTests(family, proc, dualcore, 1, "none")


#*********************************************************************************
if __name__ == '__main__':
	main()

