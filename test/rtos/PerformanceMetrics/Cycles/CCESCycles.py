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
  CCESCycles.py [-f output_file] 

Description:
- The script builds and sizes all the projects specified in
  $(project_group_file).
  
  It is assumed that a VDSP 5.0 IDDE is running.
  
  All projects are assumed to be executables and if they are dependent
  on other libraries, those libraries have been built already.
  
  The elfdump utility is used to size the dxes built. The output of 
  the elfdump is logged in xml format in the $log_directory which is
  set to 'CurrentWorkingDirectory'\log. 
  The final code, data and total sizes are logged to 'space.tab' file
  in $log_directory.

Warnings:
   - It changes the project active configurations to 'Release' and sets
     the project output directory to the project directory.

Assumptions:
  
Options:
  -h,   --help      Display usage information (this message)
  -g,   --group     The name of $(project_group_file)
"""



def AddTestNode(doc, parent, text, cycles):
	
	node  = doc.createElement('Test')
	node.setAttribute('Text', text)
	node.setAttribute('Cycles', cycles)
	node.setAttribute('Type', g_test)
	parent.appendChild(node)


#*********************************************************************************
# Function: SetObjectNumber(num)
# Description:  Creates a header that is used in the sizing projects.  Simply
#               defines the number of objects being created in the test.
#*********************************************************************************
def SetObjectNumber(num):
	
	name = os.path.join(os.getcwd(), 'sizingcfg.h')
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
# Function: OpenXML(infile)
# Description: 
#             
#*********************************************************************************
def OpenXml(myxml):
	if not os.path.isfile(myxml):
		impl = getDOMImplementation()
		doc = impl.createDocument(None, "Timings", None)
	else:
		doc = parse(myxml)
	
	return doc
		

#*********************************************************************************
# Function: CollectTimings(infile, outfile)
# Description: Finds the metrics identifier amongst the output logfile, and then 
#              passes the data to a logging function with the output filename.
#*********************************************************************************
def CollectTimings(infile, outfile):
	
	line = '\n'
	
	newdoc = OpenXml(outfile)
	top_element = newdoc.documentElement

	nodes = newdoc.getElementsByTagName(g_proc)
	if nodes.length == 0:
		print 'Nodelist empty'
		node  = newdoc.createElement(g_proc)
		top_element.appendChild(node)
	else:
		node = nodes[0]
	
	ifile = open(infile, 'r')
	line = ifile.readline()
	
	#While the line isn't eof...
	while not '' == line:
		
		#..see if the metrics string is in the line...
		index = line.find('METRIC-CYC:')
		#...and if so write it out.
		if not -1 == index:
			# strip the contents after the metrics identifier.  
			# Assumes <text>:<number in text form>
			strarr = line[index:].split(':', 3)
			AddTestNode(newdoc, node, strarr[1], strarr[2])
		line = ifile.readline()
	
	f = open(outfile, 'w')
	f.write(newdoc.toprettyxml())
	f.close()
	
	ifile.close() 
	
#*********************************************************************************
# Function: LogResults()
# Description: Outputs code, data and total size into a logfile.  If the logfile
#              exists, append what is there, otherwise create with a header.
#*********************************************************************************
def LogResults(outfile, testtext, testmetric):
	
	if not os.path.isfile(outfile):
		f = open(outfile, 'w')
	else:
		f = open(outfile, 'a')
	
	f.writelines(testtext + ' , ' + testmetric + '\n')
	f.close() 
	
#*********************************************************************************
# Function: SizeDXE(dxename,mcname,projname,logfile)
# Description: 
#
#*********************************************************************************
def SizeDXE(dxename, mcname):

	tmpfile = 'mymap.xml'	
	cmd = 'elfdump -sh -xml ' + tmpfile + ' ' + dxename
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
	
	if mcname == 'ADSP-BF533':
		textsection = ['sec_program','l1_code', 'L1_code']
		datasection = ['constdata_L1_data_a','data_L1_data_a',
                               'data_L1_data_b','constdata_L1_data_b',
                               'l2_sram','data_L1_data_a_tables']
		bsssection = ['bsz_L1_data_a','bsz_L1_data_b','bsz_init',
								'l1_data_a_bsz']
		ignoresection = ['.strtab','.symtab','.debug_abbrev',
                                 '.debug_aranges','.debug_info',
                                 '.debug_pubnames','.debug_line',
				 '.meminit','.processor','.attributes',
                                 '.linker_version','.segmentInfo',
                                 '.annotations', '.ipa']
	elif mcname == 'ADSP-BF535':
		textsection = ['L1_code','l2_sram']
		datasection = ['L1_data_a','L1_data_b','l2_sram_tables']
		bsssection =  ['bsz_L1_data_b','bsz_L1_data_a','bsz_init',
								'l1_data_a_bsz']
		ignoresection = ['.strtab','.symtab','.debug_abbrev','.ipa',
                                 '.debug_aranges','.debug_info',
                                 '.debug_pubnames', '.debug_line',
				 '.meminit','.processor','.attributes',
                                 '.linker_version','.segmentInfo',
                                 '.annotations']
	elif mcname == 'ADSP-TS101':
		textsection = ['sec_code']
		datasection = ['sec_data1','sec_data2','sec_ctor','sec_heaptab',
                               '.gdt','.frt','.cht','.edt','seg_vtbl',
								'sec_data2_tables','sec_data1_1']
		bsssection = ['sec_bsz_init','sec_bsz','sec_data2_1']
		ignoresection = ['.strtab','.symtab','.debug_abbrev','.ipa',
                                 '.debug_aranges','.debug_info',
                                 '.debug_pubnames','.debug_line',
				 				 '.meminit','.processor','.attributes',
                                 '.linker_version','.segmentInfo',
                                 '.annotations', 'sec_INT_BUSLOCK',
                                 'sec_INT_DMA0','sec_INT_DMA1','sec_INT_DMA10',
                                 'sec_INT_DMA11', 'sec_INT_DMA12',
                                 'sec_INT_DMA13','sec_INT_DMA2','sec_INT_DMA3',
                                 'sec_INT_DMA4', 'sec_INT_DMA5','sec_INT_DMA6',
                                 'sec_INT_DMA7','sec_INT_DMA8','sec_INT_DMA9',
				 				 'sec_INT_HWERR','sec_INT_IRQ0','sec_INT_IRQ1',
                                 'sec_INT_IRQ2','sec_INT_IRQ3',
				 				 'sec_INT_LINK0','sec_INT_LINK1',
                                 'sec_INT_LINK2','sec_INT_LINK3',
                                 'sec_INT_TIMER1H',
                                 'sec_INT_TIMER1L','sec_INT_VIRPT']
	elif mcname == 'ADSP-TS201':
		textsection = ['sec_code']
		datasection = ['sec_data1','sec_data2','sec_ctor','sec_heaptab',
                               	'.gdt','.frt','.cht','.edt','seg_vtbl',
                               	'data1a','data1b','data2a','data2b', 'data3a',
                               	'data3b','data4a','data4b','data5a','data5b',
								'data4a_2','data4a_tables_1']					   
		bsssection = ['sec_bsz_init','sec_bsz']
		ignoresection = ['.strtab','.symtab','.debug_abbrev','.ipa',
                                 '.debug_aranges','.debug_info',
                                 '.debug_pubnames','.debug_line', '.meminit',
                                 '.processor','.attributes','.linker_version',
                                 '.segmentInfo', '.annotations',
                                 'sec_INT_BUSLOCK','sec_INT_DMA0',
                                 'sec_INT_DMA1','sec_INT_DMA10',
                                 'sec_INT_DMA11','sec_INT_DMA12',
                                 'sec_INT_DMA13','sec_INT_DMA2',
                                 'sec_INT_DMA3','sec_INT_DMA4', 'sec_INT_DMA5',
                                 'sec_INT_DMA6',
                                 'sec_INT_DMA7','sec_INT_DMA8','sec_INT_DMA9',
				 				 'sec_INT_HWERR','sec_INT_IRQ0','sec_INT_IRQ1',
                                 'sec_INT_IRQ2','sec_INT_IRQ3',
                                 'sec_INT_LINK0','sec_INT_LINK1',
                                 'sec_INT_LINK2','sec_INT_LINK3',
                                 'sec_INT_TIMER0H','sec_INT_TIMER0L',
                                 'sec_INT_TIMER1H','sec_INT_TIMER1L',
                                 'sec_INT_VIRPT']
	elif mcname == 'ADSP-21160':
		textsection = ['seg_pmco','seg_pmda','seg_init','seg_int_code']
		datasection = ['seg_dmda','seg_ctdm','.gdt','.frt','.cht',
                               '.edt','seg_vtbl','seg_dmda_internal']
		bsssection = ['.bss']
		ignoresection = ['.strtab','.symtab','.debug_abbrev','.ipa',
                                 '.debug_aranges','.debug_info',
                                 '.debug_pubnames','.debug_line',
				 				 '.meminit','.processor','.attributes',
                                 '.linker_version','.segmentInfo',
                                 '.annotations', 'seg_BKPI', 
				 				 'seg_EMULI', 'seg_P0I', 'seg_P10I', 'seg_P12I',
                                 'seg_P13I', 'seg_P15I', 'seg_P17I', 'seg_P18I',
				 				 'seg_P1I', 'seg_P2I', 'seg_P3I', 'seg_P4I',
                                 'seg_P5I', 'seg_P6I', 'seg_P7I', 'seg_P8I',
                                 'seg_P9I', 'seg_CB15I','seg_CB7I','seg_EP0I',
                                 'seg_EP1I','seg_EP2I','seg_EP3I','seg_FIXI',
                                 'seg_FLTII','seg_FLTOI','seg_FLTUI',
                                 'seg_IICDI','seg_IRQ0I','seg_IRQ1I',
                                 'seg_IRQ2I','seg_LP0I','seg_LP1I','seg_LP2I',
                                 'seg_LP3I','seg_LP4I','seg_LP5I','seg_LSRQI',
                                 'seg_RSTI','seg_SFT0I','seg_SFT1I',
                                 'seg_SFT2I','seg_SFT3I','seg_SOVFI',
                                 'seg_SPR0I','seg_SPR1I','seg_SPT0I',
				 				 'seg_SPT1I','seg_TMZHI','seg_TMZLI',
                                 'seg_VIRPTI']		
        elif mcname == 'ADSP-21369':
                textsection = ['seg_pmco','seg_pmda','seg_init','seg_int_code']
                datasection = ['seg_dmda','seg_ctdm','.gdt','.frt','.cht',
								'.edt','seg_vtbl','seg_dmda_internal']
                bsssection = ['.bss']
                ignoresection = ['.strtab','.symtab','.debug_abbrev','.ipa',
                                 '.debug_aranges','.debug_info','.debug_line',
                                 '.debug_pubnames', '.meminit', '.processor',
                                 '.annotations','.attributes','.linker_version',
                                 '.segmentInfo', 'seg_BKPI', 'seg_CB15I',
                                 'seg_CB7I', 'seg_EMULI', 'seg_FIXI',
                                 'seg_FLTII', 'seg_FLTOI', 'seg_FLTUI',
                                 'seg_IICDI', 'seg_IRQ0I', 'seg_IRQ1I',
                                 'seg_IRQ2I', 'seg_P0I', 'seg_P10I', 'seg_P11I',
                                 'seg_P12I', 'seg_P13I', 'seg_P14I','seg_P15I',
                                 'seg_P16I', 'seg_P17I', 'seg_P18I', 'seg_P1I',
                                 'seg_P2I', 'seg_P3I', 'seg_P4I', 'seg_P5I',
                                 'seg_P6I', 'seg_P7I', 'seg_P8I', 'seg_P9I',
                                 'seg_RSTI', 'seg_SFT0I', 'seg_SFT1I',
                                 'seg_SFT2I', 'seg_SFT3I', 'seg_SOVFI',
                                 'seg_TMZHI', 'seg_TMZLI', 'seg_SPERRI']
        elif mcname == 'ADSP-21469':
                textsection = ['dxe_block0_code', 'dxe_iv_code', 'dxe_block0_sw_code_prio3', 'dxe_block0_nw_code_prio0']
                datasection = ['dxe_block1_cpp_ctors', 'dxe_block1_dm_data_prio3', 'dxe_block1_exe_name']
                bsssection = ['.bss']
                ignoresection = ['.strtab','.symtab','.debug_abbrev','.ipa',
                                 '.debug_aranges','.debug_info','.debug_line',
                                 '.debug_pubnames', '.meminit', '.processor',
                                 '.annotations','.attributes','.linker_version',
                                 '.segmentInfo', 'seg_EMUI', 'seg_RSTI',
                                 'seg_IICDI', 'seg_SOVFI', 'seg_TMZHI',
                                 'seg_SPERRI', 'seg_BKPI', 'seg_RESV1',
                                 'seg_IRQ2I', 'seg_IRQ1I', 'seg_IRQ0I',
                                 'seg_P0I', 'seg_P1I', 'seg_P2I',
                                 'seg_P3I', 'seg_P4I', 'seg_P5I',
                                 'seg_P6I', 'seg_P7I', 'seg_P8I',
                                 'seg_P9I', 'seg_P10I', 'seg_P11I',
                                 'seg_P12I', 'seg_P13I', 'seg_P14I',
                                 'seg_P15I', 'seg_P16I', 'seg_P17I',
                                 'seg_P18I', 'seg_CB7I', 'seg_CB15I',
                                 'seg_TMZLI', 'seg_FIXI', 'seg_FLTOI',
                                 'seg_FLTUI', 'seg_FLTII', 'seg_EMULI',
                                 'seg_SFT0I', 'seg_SFT1I', 'seg_SFT2I',
                                 'seg_SFT3I']
	else:
		print 'Error: ' + mcname + ' unknown'
		return	

	bsz_size = 0
	code_size = 0
	data_size = 0
	
	for shEL in shELS:
		shname = RemoveWhitespace(shEL.getAttribute('name'))
		shsize = int(RemoveWhitespace(shEL.getAttribute('size')),16)
		if shname == '':
			continue
		elif shname in textsection:
			code_size = code_size + shsize
		elif shname in datasection:
			data_size = data_size + shsize
		elif shname in bsssection:
			bsz_size = bsz_size + shsize
		elif shname in ignoresection:
			continue
		else:
			print 'Warning ' + shname + ' unknown'
	total_size = code_size + data_size + bsz_size

	if mcname in ['ADSP-21469', 'ADSP-21369', 'ADSP-21160']:
		print g_name, ",", code_size, ", ", data_size, ", ", total_size
	else:
		print g_name, ",", code_size, ", ", data_size + bsz_size, ", ", total_size
	
	LogResults(g_name, code_size, data_size, total_size)
	
	
	#print "code ", code_size, "	data ", data_size, "	bss ", bsz_size, "	Total ", total_size
	#logfile.write('%(#)7d\t' % {"#":code_size})
	#logfile.write('%(#)7d\t' % {"#":data_size})
	#logfile.write('%(#)7d\t' % {"#":bsz_size})
	#logfile.write('%(#)7d\t' % {"#":total_size})
	#logfile.write('%(#)7x\t' % {"#":total_size})		
	#logfile.write(projname+'\n')
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
	
	g_basedir =  os.getcwd()
	g_xmlfile  = 'None'
	
	try:
		opts, args = getopt.getopt(argv, 'hf:p:t:', ['help', 'file=', 'proc=', 'test='])
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
		if opt in ('-t', '--test'):
			g_test = arg
		if opt in ('-f', '--file'):
			g_file = arg
		
	#print '\n'
	g_output = os.path.join(os.getcwd(), 'output', g_file)
	g_cycles = os.path.join(os.getcwd(), '..', 'cycles.xml')
	print g_output
	if os.path.isfile(g_output):
		print 'Is a file' 
	else: 
		print 'Is NOT a file' 
	
	
	CollectTimings(g_output, g_cycles)
		
	
#*********************************************************************************
if __name__ == '__main__':
	main()
