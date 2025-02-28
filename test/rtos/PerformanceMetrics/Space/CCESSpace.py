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

__usage__ = """ \

Usage:
  CCESSpace.py 

Description:
  
Options:
  -h,   --help      Display usage information (this message)
  -i,   --ipa       Test uses IPA (does not set ipa, just adds to text output)
  -p,   --proc      set the processor for the test.
  -d,   --dxe       the test name, the dxe is assumed to be in <test>/<test>.dxe
  -n,   --num       The number of statically allocated objects in the test.
  -o,   --objects   Creates sizingcfg.h header with a macro for the test itself.
  -t,   --test      Sets the test, either MUX, SEM, MSG, FLG, NONE, STAT
  -v,   --verbose   When sizing, will output each section, it's value, and what
                    section (CODE, DATA, BSS) it's being added to.
                    

EXAMPLE:
python ../CCESSpace.py -d 21469_uCOSIII_MIN_MUT -p ADSP-21469 -t MUT -ipa

"""

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
			'ADSP-BF561', 
			'ADSP-BF592-A', 
			'ADSP-BF707']

g_Dualcores= ['ADSP-BF606', 
			'ADSP-BF607', 
			'ADSP-BF608', 
			'ADSP-BF609']

g_Arms= ['ADSP-SC589',
			'ADSP-SC573']

# Blackfin memory checking
g_bfin_textsection = ['sec_program',
							'l1_code', 
							'L2_uncached',
							'L1_code']
g_bfin_datasection = ['L1_data_1', 
							'L1_data_2', 
							'L1_data_tables', 
							'rom_data', 
							'L1_data_a_prio3', 
							'L1_data_a_prio0', 
							'L1_data_b_prio0_tables', 
							'constdata_L1_data_a',
							'data_L1_data_a', 
							'L1_data_a', 
							'L1_data_a_1', 
							'L1_data_a_tables', 
							'L1_data_b_prio0', 
							'L1_data_b_prio2', 
							'L1_data_b_prio3', 
							'L1_tables', 
							'L2_tables', 
							'data_L1_data_b',
							'constdata_L1_data_b',
							'l2_sram',
							'L1_data_b_prio1',
							'L1_data_c_no_init_prio0',
							'L1_data_c_prio0',
							'L1_data_c_prio1',
							'L1_data_c_prio2',
							'L1_data_c_prio3',
							'L2_prio0',
							'L2_sram_uncached',
							'data_L1_data_a_tables']
g_bfin_bsssection = ['rom_zero', 
							'L1_data_bsz', 
							'L1_data_a_bsz',
							'L1_data_b_bsz_prio1',
							'L1_data_c_bsz_prio0',
							'L1_data_c_bsz_prio1',
							'L1_data_c_bsz_prio2',
							'L1_data_c_bsz_prio3',
							'L1_data_a_bsz_prio3', 
							'bsz_L1_data_a',
							'bsz_L1_data_b',
							'bsz_init', 
							'L1_data_b_bsz_prio2', 
							'L1_data_b_bsz_prio3',
							'l1_data_a_bsz']

g_bfin_ignoresection = ['.adi.attributes', 
							'.strtab',
							'.symtab',
							'.debug_abbrev', 
							'.debug_aranges',
							'.debug_info', 
							'.debug_pubnames',
							'.debug_line',
							'.meminit',
							'.processor',
							'.attributes',
							'.linker_version',
							'.segmentInfo', 
							'.annotations', 
							'.ipa']


# Blackfin dual core memory sections
g_bfind_textsection = ['sec_program',
							'l1_code', 
							'L1_code']

g_bfind_datasection = ['L1_data_b_prio0', 
							'L1_data_b_prio1', 
							'L1_data_b_prio2', 
							'L1_data_b_prio3', 
							'L2_prio0', 
							'L2_tables', 
							'constdata_L1_data_a',
							'data_L1_data_a', 
							'data_L1_data_b',
							'constdata_L1_data_b', 
							'l2_sram',
							'data_L1_data_a_tables']

g_bfind_bsssection = ['L1_data_b_bsz_prio2', 
							'L1_data_b_bsz_prio3', 
							'bsz_L1_data_a',
							'bsz_L1_data_b',
							'bsz_init',
							'l1_data_a_bsz']

g_bfind_ignoresection = ['.adi.attributes', 
							'.strtab',
							'.symtab',
							'.debug_abbrev', 
							'.debug_aranges',
							'.debug_info', 
							'.debug_pubnames',
							'.debug_line', 
							'.meminit',
							'.processor',
							'.attributes', 
							'.linker_version',
							'.segmentInfo', 
							'.annotations', 
							'.ipa']

g_sharc_textsection = ['dxe_block0_nw_code', 
							'dxe_block0_nw_code_prio3', 
							'dxe_block0_code', 
							'dxe_iv_code', 
							'dxe_block0_sw_code_prio3', 
							'dxe_block0_data_prio3_bw',
							'dxe_block3_nw_code_prio0',
							'dxe_block3_sw_code_prio0',
							'dxe_block3_sw_code_prio1',
							'dxe_block3_sw_code_prio2',
							'dxe_block3_sw_code_prio3',
							'dxe_l2_cpp_ctors_bw',
							'dxe_l2_cpp_ctors_nw',
							'dxe_block0_nw_code_prio0']

g_sharc_datasection = ['dxe_block1_dm_data', 
							'dxe_block1_cpp_ctors', 
							'dxe_block1_dm_data_prio3', 
							'dxe_block1_dm_data_prio3', 
							'dxe_block1_dm_data_prio3', 
							'dxe_block1_dm_data_prio3', 
							'dxe_block0_data_prio1',
							'dxe_block0_data_prio1_bw',
							'dxe_block0_data_prio2',
							'dxe_block0_data_prio2_bw',
							'dxe_block0_data_prio3',
							'dxe_block0_data_prio3_bw',
							'dxe_l2_executable_name',
							'dxe_block1_exe_name']

g_sharc_bsssection = ['.bss']

g_sharc_ignoresection = ['.adi.attributes', 
							'.strtab',
							'.symtab',
							'.debug_abbrev',
							'.ipa', 
							'.debug_aranges',
							'.debug_info',
							'.debug_line', 
							'.debug_pubnames', 
							'.meminit', 
							'.processor', 
							'.annotations',
							'.attributes',
							'.linker_version', 
							'.segmentInfo', 
							'seg_EMUI', 
							'seg_RSTI', 
							'seg_IICDI', 
							'seg_SOVFI', 
							'seg_TMZHI', 
							'seg_SPERRI', 
							'seg_BKPI', 
							'seg_RESV1', 
							'seg_IRQ2I', 
							'seg_IRQ1I', 
							'seg_IRQ0I', 
							'seg_P0I', 
							'seg_P1I', 
							'seg_P2I', 
							'seg_P3I', 
							'seg_P4I', 
							'seg_P5I', 
							'seg_P6I', 
							'seg_P7I', 
							'seg_P8I', 
							'seg_P9I', 
							'seg_P10I', 
							'seg_P11I', 
							'seg_P12I', 
							'seg_P13I', 
							'seg_P14I', 
							'seg_P15I', 
							'seg_P16I', 
							'seg_P17I', 
							'seg_P18I', 
							'seg_CB7I', 
							'seg_CB15I', 
							'seg_TMZLI', 
							'seg_FIXI', 
							'seg_FLTOI', 
							'seg_FLTUI', 
							'seg_FLTII', 
							'seg_EMULI', 
							'seg_SFT0I', 
							'seg_SFT1I', 
							'seg_SFT2I', 
							'seg_SFT3I']


g_Arm_datasection = ['.rodata' ,
                     '.ARM.exidx' ,
                     '.data' ,
                     '.eh_frame' ,
                     '.init_array' ,
                     '.fini_array' ,
                     '.jcr']

g_Arm_textsection = ['.text',
                     '.init' ,
                     '.fini' ]

g_Arm_bsssection = ['.bss' ]
g_Arm_ignoresection = ['.debug_aranges' ,
                       '.debug_info' ,
                       '.debug_abbrev' ,
                       '.debug_line' ,
                       '.debug_frame' ,
                       '.debug_str' ,
                       '.debug_loc' ,
                       '.debug_ranges' ,
                       '.ARM.attributes' ,
                       '.comment' ,
                       '.shstrtab' ,
                       '.symtab' ,
                       '.strtab',
                       '.heap' ,
                       '.stack_sys' ,
                       '.stack_sup' ,
                       '.stack_fiq' ,
                       '.stack_irq' ,
                       '.stack_abort' ,
                       '.stack_undef' ]


# Copy the config file from the source directory into the newly-created
# destination folder.
#
def CreateConfig(proc, dst, src):
	print 'Creating config file for ' + proc
	template = os.path.join(src, 'config.tmplate')
	output =  os.path.join(dst, 'config.txt')
	
	fin  = open(template, 'r')
	fout = open(output, 'w')
	
	line = ' '
	while not line == '':
		line = fin.readline()
		line = line.replace('PROCESSOR_TEMPLATE', proc)
		fout.writelines(line)
	
	fin.close()
	fout.close()

# Create a new project.
#  TODO must not hardcode path for CCES
#
def CreateProject(src, dst, proc, pname):
	
	srcprojects = src
	dstprojects = dst
	
	print '\n'

def AddTestNode(doc, parent, text, data, code, total):
	
	node  = doc.createElement('Test')
	node.setAttribute('Text', text)
	node.setAttribute('Data', data)
	node.setAttribute('Code', code)
	node.setAttribute('Total', total)
	node.setAttribute('Type', g_type)
	node.setAttribute('NumObjects', str(g_numobjects))
	node.setAttribute('ipa', g_ipa)
	parent.appendChild(node)



#*********************************************************************************
# Function: OpenXML(infile)
# Description: 
#             
#*********************************************************************************
def OpenXml(myxml):
	print myxml
	if not os.path.isfile(myxml):
		print "This file does not exist, so trying to create now..."
		impl = getDOMImplementation()
		doc = impl.createDocument(None, "Sizing", None)
	else:
		doc = parse(myxml)
	
	return doc
		

#*********************************************************************************
# Function: LogResults()
# Description: Outputs code, data and total size into a logfile.  If the logfile
#              exists, append what is there, otherwise create with a header.
#*********************************************************************************
def LogResults(text, data, code, total):
	
	outfile = os.path.join(os.getcwd(), 'results.xml')
	
	newdoc = OpenXml(outfile)
	top_element = newdoc.documentElement
	
	nodes = newdoc.getElementsByTagName(g_proc)
	if nodes.length == 0:
		print 'There are no results logged yet (Nodelist is empty), so starting now.'
		node  = newdoc.createElement(g_proc)
		top_element.appendChild(node)
	else:
		node = nodes[0]
	
	AddTestNode(newdoc, node, text, str(code), str(data), str(total))
	
	f = open(outfile, 'w')
	f.write(newdoc.toxml())
	f.close()

#*********************************************************************************
# Function: SetObjectNumber(num)
# Description:  Creates a header that is used in the sizing projects.  Simply
#               defines the number of objects being created in the test.
#*********************************************************************************
def SetObjectNumber(num, test):
	
	# Update in two places, as the headless builder seems to copy links into the src 
	# folder regardless.
	name = os.path.join(os.getcwd(), '..', 'Source', 'sizingcfg.h')
	print 'Writing sizingcfg.h to: ' + name
	f=open(name, 'w')
	f.write('#define NUM_OBJECTS ' + num + '\n')
	f.close()
	name = os.path.join(os.getcwd(), test, 'src', 'sizingcfg.h')
	print 'Writing sizingcfg.h to: ' + name
	f=open(name, 'w')
	f.write('#define NUM_OBJECTS ' + num + '\n')
	f.close()
	
#*********************************************************************************
# Function: RemoveWhitespace(txt)
# Description:
#*********************************************************************************
def RemoveWhitespace(txt):
	return txt.lstrip().rstrip()
	

#*********************************************************************************
# Function: OutputSection(name, sizedec, sizehex, action)
# Description: Will output section details to screen, if verbose is enabled.
#*********************************************************************************
def OutputSection(name, sizedec, sizehex, action):
	if g_verbose == 1:
		print 'Section ' + name + ' Size(d/h): ' + sizehex + '/' + str(sizedec) + ' ' + action


#*********************************************************************************
# Function: SizeDXE(dxename,mcname,projname,logfile)
# Description: 
#
#*********************************************************************************
def SizeDXE(dxename, mcname):
	global g_Sharcs
	global g_Blackfins
	global g_Dualcores
	
	tmpfile = 'mymap.xml'	
	cmd = 'elfdump  -sh -xml ' + tmpfile + ' "' + dxename + '"'
	print 'Issuing elfdump command: '+ cmd
	os.system(cmd)
	
	if not os.path.isfile(tmpfile):
		print 'Error: Unsuccessful elfdump run for ' + dxename
		return
	
	try:
		fDOM = parse(tmpfile)
	except (IOError):
		print 'Error: ' + tmpfile + ' not found'
		return
		
	#try:
	#	shutil.move(tmpfile,g_logdir)
	#except:
	#	print 'Warning: ' + tmpfile + ' could not be moved to '+ g_logdir
		
	docDOM = fDOM.getElementsByTagName('DOCUMENT')[0]
	ofDOM = docDOM.getElementsByTagName('OBJECT_FILE')[0]
	eshDOM = ofDOM.getElementsByTagName('ELF_SECTION_HEADERS')[0]
	shELS  = eshDOM.getElementsByTagName('SECTION_HEADER')
	
	if mcname in g_Blackfins:
		textsection = g_bfin_textsection 
		datasection = g_bfin_datasection
		bsssection = g_bfin_bsssection
		ignoresection = g_bfin_ignoresection 
		
	elif mcname in g_Dualcores:
		textsection = g_bfind_textsection 
		datasection = g_bfind_datasection
		bsssection = g_bfind_bsssection 
		ignoresection = g_bfind_ignoresection
		
	elif (g_armflag == 0) and (mcname in g_Sharcs):
		textsection = g_sharc_textsection
		datasection = g_sharc_datasection
		bsssection = g_sharc_bsssection
		ignoresection = g_sharc_ignoresection
	elif mcname in g_Arms:
		textsection = g_Arm_textsection
		datasection = g_Arm_datasection
		bsssection = g_Arm_bsssection
		ignoresection = g_Arm_ignoresection
	else:
		print 'Error: ' + mcname + ' unknown'
		return	
	
	bsz_size = 0
	code_size = 0
	data_size = 0
	
	for shEL in shELS:
		shname = RemoveWhitespace(shEL.getAttribute('name'))
		shsizehex = RemoveWhitespace(shEL.getAttribute('size'))
		shsize = int(RemoveWhitespace(shEL.getAttribute('size')),16)
		if shname == '':
			continue
		elif shname in textsection:
			OutputSection(shname, shsize, shsizehex, 'Added to CODE')
			code_size = code_size + shsize
		elif shname in datasection:
			OutputSection(shname, shsize, shsizehex, 'Added to DATA')
			data_size = data_size + shsize
		elif shname in bsssection:
			OutputSection(shname, shsize, shsizehex, 'Added to BSS (data)')
			bsz_size = bsz_size + shsize
		elif shname in ignoresection:
			OutputSection(shname, shsize, shsizehex, 'IGNORED')
			continue
		else:
			print 'Warning ' + shname + ' unknown'
	total_size = code_size + data_size + bsz_size

	if mcname in ['ADSP-21469', 'ADSP-21369', 'ADSP-21160']:
		print g_testname, ",", code_size, ", ", data_size, ", ", total_size
	else:
		print g_testname, ",", code_size, ", ", data_size + bsz_size, ", ", total_size
	
	LogResults(g_testname, code_size, data_size + bsz_size, total_size)
	
	return

#*********************************************************************************
# Function: BuildnSizeProjects()
# Description: 
#
#*********************************************************************************
def BuildnSizeProjects():
	print 'Info: Building/Sizing projects...'
	projcfgs = g_domtree.getElementsByTagName('project')

	logname = os.path.normpath(os.path.join(g_logdir, g_dpgfilename + '.tab'))
	logfile = open(logname, 'w')
	logfile.write('   text\t   data\t    bss\t    dec\t    hex\tproject name\n')

	for projcfg in projcfgs:
		projpath = RemoveWhitespace(projcfg.getAttribute('file'))
		projpath = os.path.normpath(os.path.join(g_basedir, projpath))
		if not os.path.isfile(projpath):
			print '\nError: ' + projpath + ' not found'
			continue
			
		#Load Project
		proj = g_Idde.ProjectList.AddProject(projpath)
		proj.ActiveConfiguration = 'Release'
		proj.SetOutputDirectory(proj.ActiveConfiguration, proj.ProjectDirectory)
		
		outputWindow = g_Idde.OutputWindow
		outputWindow.ClearText(g_API.constants.tabBuild)
		
		bflag = 1
		try:
			proj.UpdateDependencies(True)
			proj.Clean()
			proj.BuildAll(True)
		except (com_error):
			bflag = 0
		
		#check for successful build
		if bflag == 1:
			if g_armflag == 1:
				dxename = os.path.join(proj.ProjectDirectory, proj.Name)
			else:
				dxename = os.path.join(proj.ProjectDirectory, proj.Name + '.dxe')
			if not os.path.isfile(dxename):
				bflag = 0

		# Size the dxe, and also copy the map.xml file to the log dir as a 
		# verbose sizing log
		if bflag == 0:
			print '\nError: ' + proj.Name + ' failed to build'
		else:
			SizeDXE(dxename,proj.Processor,proj.Name,logfile)
			mapname = os.path.join(proj.ProjectDirectory, proj.Name + '.map.xml')
			copy2(mapname, g_logdir)
			print proj.Name + ' complete'
		#Close Project
		g_Idde.ProjectList.RemoveProject( proj.FileName )
	print '\nInfo: Building/Sizing complete.'
	logfile.close()
	


#*********************************************************************************
# Set Test String()
#*********************************************************************************
def SetTestString(ipa, nobjects, tobject):
	
	text = 'Basic project'
	
	if not tobject == 'STAT':
		if nobjects > 1:
			text += ' using ' + str(nobjects) + ' static objects'
		elif nobjects == 1:
			text += ' using 1' + ' static object'
	else:
		text += ' using a statistics task'
	
	if ipa == '1':
		text += ' with ipa'  
	
	
	return text
	

#*********************************************************************************
# usage()
#*********************************************************************************
def usage():
	sys.stdout.write(__usage__)
	
#*********************************************************************************
# main()
#*********************************************************************************
def main( argv=sys.argv[1:] ):
	global g_basedir
	global g_logdir
	global g_xmlfile
	global g_domtree
	global g_dpgfilename
	global g_dxefile
	global g_proc
	global g_name
	global g_test
	global g_numobjects
	global g_ipa
	global g_type
	global g_verbose
	global g_armflag
	
	global g_testname
	
	g_basedir =  os.getcwd()
	g_xmlfile  = 'None'
	g_verbose  = 0 
	ipa = '0'
	num = 0
	test = ''	
	setobjects = 0
	g_armflag = 0 
	
	try:
		opts, args = getopt.getopt(argv, 'hivao:p:d:n:t:', ['help', 'ipa', 'verbose', 'arm', 'objects=', 'proc=', 'dxe=', 'num=', 'test='])
	except getopt.GetoptError:
		usage()
		return
	for opt, arg in opts:
		if opt in ('-h', '--help'):
			usage()
			return
		if opt in ('-d', '--dxe'):
			g_name = os.path.normpath(arg)
		if opt in ('-p', '--proc'):
			g_proc = arg
		if opt in ('-a', '--arm'):
			g_armflag = 1
		if opt in ('-n', '--num'):
			num = int(arg)
		if opt in ('-t', '--test'):
			test = arg
		if opt in ('-i', '--ipa'):
			ipa = '1'
		if opt in ('-v', '--verbose'):
			g_verbose = 1
		if opt in ('-o', '--objects'):
			num = int(arg)
			setobjects = 1
	
	if setobjects == 1:
		SetObjectNumber(str(num), g_name)
		return
	
	# Form the text used for this sizing measurement.  
	# Using the data passed-in.
	#
	g_testname = SetTestString(ipa, num, test)
	
	g_ipa = ipa
	g_numobjects = num
	g_type = test
	if g_armflag  == 1:
		g_dxefile = os.path.join(os.getcwd(), g_name, 'Release' ,g_name)
	else:
		g_dxefile = os.path.join(os.getcwd(), g_name, 'Release' ,g_name + '.dxe')
	print "Sizing DXE in :" + g_dxefile
	if not os.path.isfile(g_dxefile):
		print 'No DXE found' 
		return
	
	SizeDXE(g_dxefile, g_proc)
		
	
#*********************************************************************************
if __name__ == '__main__':
	main()
	exit(0)
